/*
 * Copyright (c) Souldbminer and Horizon OC Contributors
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */
 
/* --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */


#include "clock_manager.h"
#include <cstring>
#include "file_utils.h"
#include "board.h"
#include "process_management.h"
#include "errors.h"
#include "ipc_service.h"
#include "kip.h"
#include "i2c_reg.h"
#include "notification.h"

#define HOSPPC_HAS_BOOST (hosversionAtLeast(7,0,0))

ClockManager *ClockManager::instance = NULL;
Thread governorTHREAD;

ClockManager *ClockManager::GetInstance()
{
    return instance;
}

void ClockManager::Exit()
{
    if (instance)
    {
        delete instance;
    }
}

void ClockManager::Initialize()
{
    if (!instance)
    {
        instance = new ClockManager();
    }
}

ClockManager::ClockManager()
{
    this->config = Config::CreateDefault();
    this->context = new SysClkContext;
    this->context->applicationId = 0;
    this->context->profile = SysClkProfile_Handheld;
    this->context->enabled = false;
    for (unsigned int module = 0; module < SysClkModule_EnumMax; module++)
    {
        this->context->freqs[module] = 0;
        this->context->realFreqs[module] = 0;
        this->context->overrideFreqs[module] = 0;
        this->RefreshFreqTableRow((SysClkModule)module);
    }

    this->running = false;
    this->lastTempLogNs = 0;
    this->lastCsvWriteNs = 0;

    this->rnxSync = new ReverseNXSync;

    if(this->config->GetConfigValue(HocClkConfigValue_KipEditing))
        this->GetKipData();   
    threadCreate(
        &governorTHREAD,
        ClockManager::GovernorThread,
        this,
        NULL,
        0x2000,
        0x3F,
        -2
    );

	threadStart(&governorTHREAD);
 
}

ClockManager::~ClockManager()
{
    delete this->config;
    delete this->context;
    threadClose(&governorTHREAD);
}

SysClkContext ClockManager::GetCurrentContext()
{
    std::scoped_lock lock{this->contextMutex};
    return *this->context;
}

Config *ClockManager::GetConfig()
{
    return this->config;
}

void ClockManager::SetRunning(bool running)
{
    this->running = running;
}

bool ClockManager::Running()
{
    return this->running;
}

void ClockManager::GetFreqList(SysClkModule module, std::uint32_t *list, std::uint32_t maxCount, std::uint32_t *outCount)
{
    ASSERT_ENUM_VALID(SysClkModule, module);

    *outCount = std::min(maxCount, this->freqTable[module].count);
    memcpy(list, &this->freqTable[module].list[0], *outCount * sizeof(this->freqTable[0].list[0]));
}

bool ClockManager::IsAssignableHz(SysClkModule module, std::uint32_t hz)
{
    switch (module)
    {
    case SysClkModule_CPU:
        return hz >= 400000000;
    case SysClkModule_MEM:
        return hz == 204000000 || hz >= 665600000;
    default:
        return true;
    }
}

std::uint32_t ClockManager::GetMaxAllowedHz(SysClkModule module, SysClkProfile profile)
{
    if (this->config->GetConfigValue(HocClkConfigValue_UncappedClocks))
    {
        return 4294967294; // Integer limit, uncapped clocks ON
    }
    else
    {
        if (module == SysClkModule_GPU)
        {
            if (profile < SysClkProfile_HandheldCharging)
            {
                switch(Board::GetSocType()) {
                    case SysClkSocType_Erista:
                        return 460800000;
                    case SysClkSocType_Mariko:
                        return 614400000;
                    default:
                        return 4294967294;
                }
            }
            else if (profile <= SysClkProfile_HandheldChargingUSB)
            {
                return 768000000;
            }
        }
    }
    return 0;
}

std::uint32_t ClockManager::GetNearestHz(SysClkModule module, std::uint32_t inHz, std::uint32_t maxHz)
{
    std::uint32_t *freqs = &this->freqTable[module].list[0];
    size_t count = this->freqTable[module].count - 1;

    size_t i = 0;
    while (i < count)
    {
        if (maxHz > 0 && freqs[i] >= maxHz)
        {
            break;
        }

        if (inHz <= ((std::uint64_t)freqs[i] + freqs[i + 1]) / 2)
        {
            break;
        }

        i++;
    }

    return freqs[i];
}

bool ClockManager::ConfigIntervalTimeout(SysClkConfigValue intervalMsConfigValue, std::uint64_t ns, std::uint64_t *lastLogNs)
{
    std::uint64_t logInterval = this->GetConfig()->GetConfigValue(intervalMsConfigValue) * 1000000ULL;
    bool shouldLog = logInterval && ((ns - *lastLogNs) > logInterval);

    if (shouldLog)
    {
        *lastLogNs = ns;
    }

    return shouldLog;
}

void ClockManager::RefreshFreqTableRow(SysClkModule module)
{
    std::scoped_lock lock{this->contextMutex};

    std::uint32_t freqs[SYSCLK_FREQ_LIST_MAX];
    std::uint32_t count;

    FileUtils::LogLine("[mgr] %s freq list refresh", Board::GetModuleName(module, true));
    Board::GetFreqList(module, &freqs[0], SYSCLK_FREQ_LIST_MAX, &count);

    std::uint32_t *hz = &this->freqTable[module].list[0];
    this->freqTable[module].count = 0;
    for (std::uint32_t i = 0; i < count; i++)
    {
        if (!this->IsAssignableHz(module, freqs[i]))
        {
            continue;
        }

        *hz = freqs[i];
        FileUtils::LogLine("[mgr] %02u - %u - %u.%u MHz", this->freqTable[module].count, *hz, *hz / 1000000, *hz / 100000 - *hz / 1000000 * 10);

        this->freqTable[module].count++;
        hz++;
    }

    FileUtils::LogLine("[mgr] count = %u", this->freqTable[module].count);
}

u32 findIndex(u32 arr[], u32 size, u32 value) {
    for (u32 i = 0; i < size; i++) {
        if (arr[i] == value) {
            return i;
        }
    }
    return 0;
}

u32 findIndexMHz(u32 arr[], u32 size, u32 value) {
    for (u32 i = 0; i < size; i++) {
        if (arr[i] / 1000000 == value) {
            return i;
        }
    }
    return 0;
}

void ClockManager::GovernorThread(void* arg)
{
    ClockManager* mgr = static_cast<ClockManager*>(arg);

    for (;;)
    {
        if (!mgr->running)
        {
            svcSleepThread(50'000'000);
            continue;
        }

        std::scoped_lock lock{mgr->contextMutex};

        if (!mgr->config->GetConfigValue(HocClkConfigValue_HandheldGovernor))
        {
            svcSleepThread(50'000'000);
            continue;
        }

        auto& table = mgr->freqTable[SysClkModule_GPU];
        if (table.count == 0)
        {
            svcSleepThread(50'000'000);
            continue;
        }

        u32 currentHz = Board::GetHz(SysClkModule_GPU);

        u32 index = 0;
        for (u32 i = 0; i < table.count; i++)
        {
            if (table.list[i] == currentHz)
            {
                index = i;
                break;
            }
        }

        u32 targetHz = mgr->context->overrideFreqs[SysClkModule_GPU];
        if (!targetHz)
        {
            targetHz = mgr->config->GetAutoClockHz(
                mgr->context->applicationId,
                SysClkModule_GPU,
                mgr->context->profile
            );

            if (!targetHz)
            {
                targetHz = mgr->config->GetAutoClockHz(
                    GLOBAL_PROFILE_ID,
                    SysClkModule_GPU,
                    mgr->context->profile
                );
            }
        }

        int load = Board::GetPartLoad(HocClkPartLoad_GPU);

        if (load < 600 && index > 0)
        {
            index--;
        }
        else if (load > 800 && index + 1 < table.count)
        {
            index++;
        }

        if (targetHz)
        {
            u32 targetIndex = index;
            for (u32 i = 0; i < table.count; i++)
            {
                if (table.list[i] >= targetHz)
                {
                    targetIndex = i;
                    break;
                }
            }

            if (index > targetIndex && index > 0)
                index--;
            else if (index < targetIndex && index + 1 < table.count)
                index++;
        }

        u32 newHz = table.list[index];
        if (mgr->IsAssignableHz(SysClkModule_GPU, newHz))
        {
            Board::SetHz(SysClkModule_GPU, newHz);
            mgr->context->freqs[SysClkModule_GPU] = newHz;
        }

        svcSleepThread(50'000'000);
    }
}


void ClockManager::Tick()
{
    std::scoped_lock lock{this->contextMutex};
    std::uint32_t mode = 0;
    AppletOperationMode opMode = appletGetOperationMode();
    Result rc = apmExtGetCurrentPerformanceConfiguration(&mode);
    ASSERT_RESULT_OK(rc, "apmExtGetCurrentPerformanceConfiguration");

    if(this->config->GetConfigValue(HorizonOCConfigValue_BatteryChargeCurrent)) {
        I2c_Bq24193_SetFastChargeCurrentLimit(this->config->GetConfigValue(HorizonOCConfigValue_BatteryChargeCurrent));
    }

    if(this->config->GetConfigValue(HocClkConfigValue_HandheldTDP) && opMode == AppletOperationMode_Handheld) {
        if(Board::GetConsoleType() == HorizonOCConsoleType_Lite) {
            if(Board::GetPowerMw(SysClkPowerSensor_Now) < -(int)this->config->GetConfigValue(HocClkConfigValue_LiteTDPLimit)) {
                ResetToStockClocks();
                return;
            }
        } else {
            if(Board::GetPowerMw(SysClkPowerSensor_Now) < -(int)this->config->GetConfigValue(HocClkConfigValue_HandheldTDPLimit)) {
                ResetToStockClocks();
                return;
            }
        }
    }


    if(((tmp451TempSoc() / 1000) > (int)this->config->GetConfigValue(HocClkConfigValue_ThermalThrottleThreshold)) && this->config->GetConfigValue(HocClkConfigValue_ThermalThrottle)) {
        ResetToStockClocks();
        return;
    }

    bool noGPU = false;

    if (this->RefreshContext() || this->config->Refresh())
    {
        std::uint32_t targetHz = 0;
        std::uint32_t maxHz = 0;
        std::uint32_t nearestHz = 0;

            if(apmExtIsBoostMode(mode) && !this->config->GetConfigValue(HocClkConfigValue_OverwriteBoostMode)) {
                ResetToStockClocks();
                return;
            }
            for (unsigned int module = 0; module < SysClkModule_EnumMax; module++)
            {
                    if(this->config->GetConfigValue(HocClkConfigValue_HandheldGovernor)) {
                        noGPU = true;
                    } else {
                        noGPU = false;
                    }
                    if(noGPU && module == SysClkModule_GPU)
                        continue;
                    targetHz = this->context->overrideFreqs[module];
                    if (!targetHz)
                    {
                        targetHz = this->config->GetAutoClockHz(this->context->applicationId, (SysClkModule)module, this->context->profile);
                        if(!targetHz)
                            targetHz = this->config->GetAutoClockHz(GLOBAL_PROFILE_ID, (SysClkModule)module, this->context->profile);
                    }

                    if (targetHz)
                    {

                        maxHz = this->GetMaxAllowedHz((SysClkModule)module, this->context->profile);
                        nearestHz = this->GetNearestHz((SysClkModule)module, targetHz, maxHz);
                        if (nearestHz != this->context->freqs[module] && this->context->enabled) {
                            FileUtils::LogLine(
                                "[mgr] %s clock set : %u.%u MHz (target = %u.%u MHz)",
                                Board::GetModuleName((SysClkModule)module, true),
                                nearestHz / 1000000, nearestHz / 100000 - nearestHz / 1000000 * 10,
                                targetHz / 1000000, targetHz / 100000 - targetHz / 1000000 * 10);

                            Board::SetHz((SysClkModule)module, nearestHz);
                            this->context->freqs[module] = nearestHz;
                    }
                    }

            }
        }
    
}

void ClockManager::ResetToStockClocks() {
    Board::ResetToStockCpu();
    Board::ResetToStockGpu();
}

void ClockManager::WaitForNextTick()
{
    svcSleepThread(this->GetConfig()->GetConfigValue(SysClkConfigValue_PollingIntervalMs) * 1000000ULL);
}

bool ClockManager::RefreshContext()
{
    bool hasChanged = false;

    bool enabled = this->GetConfig()->Enabled();
    if (enabled != this->context->enabled)
    {
        this->context->enabled = enabled;
        FileUtils::LogLine("[mgr] " TARGET " status: %s", enabled ? "enabled" : "disabled");
        hasChanged = true;
    }

    std::uint64_t applicationId = ProcessManagement::GetCurrentApplicationId();
    if (applicationId != this->context->applicationId)
    {
        FileUtils::LogLine("[mgr] TitleID change: %016lX", applicationId);
        this->context->applicationId = applicationId;
        hasChanged = true;
        this->rnxSync->Reset(applicationId);
    }

    SysClkProfile profile = Board::GetProfile();
    if (profile != this->context->profile)
    {
        FileUtils::LogLine("[mgr] Profile change: %s", Board::GetProfileName(profile, true));
        this->context->profile = profile;
        hasChanged = true;
    }

    // restore clocks to stock values on app or profile change
    if (hasChanged)
    {
        // this->rnxSync->ToggleSync(this->GetConfig()->GetConfigValue(HocClkConfigValue_SyncReverseNXMode));
        Board::ResetToStock();
        this->WaitForNextTick();
    }

    std::uint32_t hz = 0;
    for (unsigned int module = 0; module < SysClkModule_EnumMax; module++)
    {
        hz = Board::GetHz((SysClkModule)module);
        if (hz != 0 && hz != this->context->freqs[module])
        {
            FileUtils::LogLine("[mgr] %s clock change: %u.%u MHz", Board::GetModuleName((SysClkModule)module, true), hz / 1000000, hz / 100000 - hz / 1000000 * 10);
            this->context->freqs[module] = hz;
            hasChanged = true;
        }

        hz = this->GetConfig()->GetOverrideHz((SysClkModule)module);
        if (hz != this->context->overrideFreqs[module])
        {
            if (hz)
            {
                FileUtils::LogLine("[mgr] %s override change: %u.%u MHz", Board::GetModuleName((SysClkModule)module, true), hz / 1000000, hz / 100000 - hz / 1000000 * 10);
            }
            else
            {
                FileUtils::LogLine("[mgr] %s override disabled", Board::GetModuleName((SysClkModule)module, true));
                switch (module)
                {
                case SysClkModule_CPU:
                    Board::ResetToStockCpu();
                    break;
                case SysClkModule_GPU:
                    Board::ResetToStockGpu();
                    break;
                case SysClkModule_MEM:
                    Board::ResetToStockMem();
                    break;
                }
            }
            this->context->overrideFreqs[module] = hz;
            hasChanged = true;
        }
    }

    std::uint64_t ns = armTicksToNs(armGetSystemTick());

    // temperatures do not and should not force a refresh, hasChanged untouched
    std::uint32_t millis = 0;
    bool shouldLogTemp = this->ConfigIntervalTimeout(SysClkConfigValue_TempLogIntervalMs, ns, &this->lastTempLogNs);
    for (unsigned int sensor = 0; sensor < SysClkThermalSensor_EnumMax; sensor++)
    {
        millis = Board::GetTemperatureMilli((SysClkThermalSensor)sensor);
        if (shouldLogTemp)
        {
            FileUtils::LogLine("[mgr] %s temp: %u.%u °C", Board::GetThermalSensorName((SysClkThermalSensor)sensor, true), millis / 1000, (millis - millis / 1000 * 1000) / 100);
        }
        this->context->temps[sensor] = millis;
    }

    // power stats do not and should not force a refresh, hasChanged untouched
    std::int32_t mw = 0;
    bool shouldLogPower = this->ConfigIntervalTimeout(SysClkConfigValue_PowerLogIntervalMs, ns, &this->lastPowerLogNs);
    for (unsigned int sensor = 0; sensor < SysClkPowerSensor_EnumMax; sensor++)
    {
        mw = Board::GetPowerMw((SysClkPowerSensor)sensor);
        if (shouldLogPower)
        {
            FileUtils::LogLine("[mgr] Power %s: %d mW", Board::GetPowerSensorName((SysClkPowerSensor)sensor, false), mw);
        }
        this->context->power[sensor] = mw;
    }

    // real freqs do not and should not force a refresh, hasChanged untouched
    std::uint32_t realHz = 0;
    bool shouldLogFreq = this->ConfigIntervalTimeout(SysClkConfigValue_FreqLogIntervalMs, ns, &this->lastFreqLogNs);
    for (unsigned int module = 0; module < SysClkModule_EnumMax; module++)
    {
        realHz = Board::GetRealHz((SysClkModule)module);
        if (shouldLogFreq)
        {
            FileUtils::LogLine("[mgr] %s real freq: %u.%u MHz", Board::GetModuleName((SysClkModule)module, true), realHz / 1000000, realHz / 100000 - realHz / 1000000 * 10);
        }
        this->context->realFreqs[module] = realHz;
    }

    // ram load do not and should not force a refresh, hasChanged untouched
    for (unsigned int loadSource = 0; loadSource < SysClkPartLoad_EnumMax; loadSource++)
    {
        this->context->PartLoad[loadSource] = Board::GetPartLoad((SysClkPartLoad)loadSource);
    }

    for (unsigned int voltageSource = 0; voltageSource < HocClkVoltage_EnumMax; voltageSource++)
    {
        this->context->voltages[voltageSource] = Board::GetVoltage((HocClkVoltage)voltageSource);
    }

    if (this->ConfigIntervalTimeout(SysClkConfigValue_CsvWriteIntervalMs, ns, &this->lastCsvWriteNs))
    {
        FileUtils::WriteContextToCsv(this->context);
    }

    this->context->fanLevel = Board::GetFanRotationLevel();

    return hasChanged;
}

void ClockManager::SetRNXRTMode(ReverseNXMode mode)
{
    this->rnxSync->SetRTMode(mode);
}

void ClockManager::SetKipData() {

    if(Board::GetSocType() == SysClkSocType_Mariko) {
        if(I2c_BuckConverter_SetMvOut(&I2c_Mariko_DRAM_VDDQ, this->config->GetConfigValue(KipConfigValue_marikoEmcVddqVolt) / 1000)) {
            FileUtils::LogLine("[clock_manager] Failed set i2c vddq");
            writeNotification("Horizon OC\nFailed to write I2C\nwhile setting vddq");
        }
    }
    CustomizeTable table;

    if (!cust_read_and_cache(this->config->GetConfigValue(HocClkConfigValue_KipFileName) ? "sdmc:/atmosphere/kips/loader.kip" : "sdmc:/atmosphere/kips/hoc.kip", &table)) {
        FileUtils::LogLine("[clock_manager] Failed to read KIP file");
        writeNotification("Horizon OC\nKip read failed");
        return;
    }

    CUST_WRITE_FIELD_BATCH(&table, custRev, this->config->GetConfigValue(KipConfigValue_custRev));
    CUST_WRITE_FIELD_BATCH(&table, mtcConf, this->config->GetConfigValue(KipConfigValue_mtcConf));
    CUST_WRITE_FIELD_BATCH(&table, hpMode, this->config->GetConfigValue(KipConfigValue_hpMode));

    CUST_WRITE_FIELD_BATCH(&table, commonEmcMemVolt, this->config->GetConfigValue(KipConfigValue_commonEmcMemVolt));
    CUST_WRITE_FIELD_BATCH(&table, eristaEmcMaxClock, this->config->GetConfigValue(KipConfigValue_eristaEmcMaxClock));
    CUST_WRITE_FIELD_BATCH(&table, marikoEmcMaxClock, this->config->GetConfigValue(KipConfigValue_marikoEmcMaxClock));
    CUST_WRITE_FIELD_BATCH(&table, marikoEmcVddqVolt, this->config->GetConfigValue(KipConfigValue_marikoEmcVddqVolt));
    CUST_WRITE_FIELD_BATCH(&table, emcDvbShift, this->config->GetConfigValue(KipConfigValue_emcDvbShift));

    CUST_WRITE_FIELD_BATCH(&table, t1_tRCD, this->config->GetConfigValue(KipConfigValue_t1_tRCD));
    CUST_WRITE_FIELD_BATCH(&table, t2_tRP, this->config->GetConfigValue(KipConfigValue_t2_tRP));
    CUST_WRITE_FIELD_BATCH(&table, t3_tRAS, this->config->GetConfigValue(KipConfigValue_t3_tRAS));
    CUST_WRITE_FIELD_BATCH(&table, t4_tRRD, this->config->GetConfigValue(KipConfigValue_t4_tRRD));
    CUST_WRITE_FIELD_BATCH(&table, t5_tRFC, this->config->GetConfigValue(KipConfigValue_t5_tRFC));
    CUST_WRITE_FIELD_BATCH(&table, t6_tRTW, this->config->GetConfigValue(KipConfigValue_t6_tRTW));
    CUST_WRITE_FIELD_BATCH(&table, t7_tWTR, this->config->GetConfigValue(KipConfigValue_t7_tWTR));
    CUST_WRITE_FIELD_BATCH(&table, t8_tREFI, this->config->GetConfigValue(KipConfigValue_t8_tREFI));
    CUST_WRITE_FIELD_BATCH(&table, mem_burst_read_latency, this->config->GetConfigValue(KipConfigValue_mem_burst_read_latency));
    CUST_WRITE_FIELD_BATCH(&table, mem_burst_write_latency, this->config->GetConfigValue(KipConfigValue_mem_burst_write_latency));

    CUST_WRITE_FIELD_BATCH(&table, eristaCpuUV, this->config->GetConfigValue(KipConfigValue_eristaCpuUV));
    CUST_WRITE_FIELD_BATCH(&table, eristaCpuVmin, this->config->GetConfigValue(KipConfigValue_eristaCpuVmin));
    CUST_WRITE_FIELD_BATCH(&table, eristaCpuMaxVolt, this->config->GetConfigValue(KipConfigValue_eristaCpuMaxVolt));
    CUST_WRITE_FIELD_BATCH(&table, eristaCpuUnlock, this->config->GetConfigValue(KipConfigValue_eristaCpuUnlock));

    CUST_WRITE_FIELD_BATCH(&table, marikoCpuUVLow, this->config->GetConfigValue(KipConfigValue_marikoCpuUVLow));
    CUST_WRITE_FIELD_BATCH(&table, marikoCpuUVHigh, this->config->GetConfigValue(KipConfigValue_marikoCpuUVHigh));
    CUST_WRITE_FIELD_BATCH(&table, tableConf, this->config->GetConfigValue(KipConfigValue_tableConf));
    CUST_WRITE_FIELD_BATCH(&table, marikoCpuLowVmin, this->config->GetConfigValue(KipConfigValue_marikoCpuLowVmin));
    CUST_WRITE_FIELD_BATCH(&table, marikoCpuHighVmin, this->config->GetConfigValue(KipConfigValue_marikoCpuHighVmin));
    CUST_WRITE_FIELD_BATCH(&table, marikoCpuMaxVolt, this->config->GetConfigValue(KipConfigValue_marikoCpuMaxVolt));
    CUST_WRITE_FIELD_BATCH(&table, marikoCpuMaxClock, this->config->GetConfigValue(KipConfigValue_marikoCpuMaxClock) * 1000);

    CUST_WRITE_FIELD_BATCH(&table, eristaCpuBoostClock, this->config->GetConfigValue(KipConfigValue_eristaCpuBoostClock) * 1000);
    CUST_WRITE_FIELD_BATCH(&table, marikoCpuBoostClock, this->config->GetConfigValue(KipConfigValue_marikoCpuBoostClock) * 1000);

    CUST_WRITE_FIELD_BATCH(&table, eristaGpuUV, this->config->GetConfigValue(KipConfigValue_eristaGpuUV));
    CUST_WRITE_FIELD_BATCH(&table, eristaGpuVmin, this->config->GetConfigValue(KipConfigValue_eristaGpuVmin));

    CUST_WRITE_FIELD_BATCH(&table, marikoGpuUV, this->config->GetConfigValue(KipConfigValue_marikoGpuUV));
    CUST_WRITE_FIELD_BATCH(&table, marikoGpuVmin, this->config->GetConfigValue(KipConfigValue_marikoGpuVmin));
    CUST_WRITE_FIELD_BATCH(&table, marikoGpuVmax, this->config->GetConfigValue(KipConfigValue_marikoGpuVmax));

    CUST_WRITE_FIELD_BATCH(&table, commonGpuVoltOffset, this->config->GetConfigValue(KipConfigValue_commonGpuVoltOffset));
    CUST_WRITE_FIELD_BATCH(&table, gpuSpeedo, this->config->GetConfigValue(KipConfigValue_gpuSpeedo));
    CUST_WRITE_FIELD_BATCH(&table, marikoGpuFullUnlock, this->config->GetConfigValue(KipConfigValue_marikoGpuFullUnlock));

    for (int i = 0; i < 24; i++) {
        table.marikoGpuVoltArray[i] = this->config->GetConfigValue((SysClkConfigValue)(KipConfigValue_g_volt_76800 + i));
    }

    for (int i = 0; i < 27; i++) {
        table.eristaGpuVoltArray[i] = this->config->GetConfigValue((SysClkConfigValue)(KipConfigValue_g_volt_e_76800 + i));
    }

    if (!cust_write_table(this->config->GetConfigValue(HocClkConfigValue_KipFileName) ? "sdmc:/atmosphere/kips/loader.kip" : "sdmc:/atmosphere/kips/hoc.kip", &table)) {
        FileUtils::LogLine("[clock_manager] Failed to write KIP file");
        writeNotification("Horizon OC\nKip write failed");
    }
}

// I know this is very hacky, but the config system in the sysmodule doesn't really support writing

void ClockManager::GetKipData() {
    if(this->config->Refresh()) {
        CustomizeTable table;

        if (!cust_read_and_cache(this->config->GetConfigValue(HocClkConfigValue_KipFileName) ? "sdmc:/atmosphere/kips/loader.kip" : "sdmc:/atmosphere/kips/hoc.kip", &table)) {
            FileUtils::LogLine("[clock_manager] Failed to read KIP file for GetKipData");
            writeNotification("Horizon OC\nKip read failed");
            return;
        }

        SysClkConfigValueList configValues;
        this->config->GetConfigValues(&configValues);

        configValues.values[KipConfigValue_mtcConf] = cust_get_mtc_conf(&table);
        configValues.values[KipConfigValue_hpMode] = cust_get_hp_mode(&table);

        configValues.values[KipConfigValue_commonEmcMemVolt] = cust_get_common_emc_volt(&table);
        configValues.values[KipConfigValue_eristaEmcMaxClock] = cust_get_erista_emc_max(&table);
        configValues.values[KipConfigValue_marikoEmcMaxClock] = cust_get_mariko_emc_max(&table);
        configValues.values[KipConfigValue_marikoEmcVddqVolt] = cust_get_mariko_emc_vddq(&table);
        configValues.values[KipConfigValue_emcDvbShift] = cust_get_emc_dvb_shift(&table);

        configValues.values[KipConfigValue_t1_tRCD] = cust_get_tRCD(&table);
        configValues.values[KipConfigValue_t2_tRP] = cust_get_tRP(&table);
        configValues.values[KipConfigValue_t3_tRAS] = cust_get_tRAS(&table);
        configValues.values[KipConfigValue_t4_tRRD] = cust_get_tRRD(&table);
        configValues.values[KipConfigValue_t5_tRFC] = cust_get_tRFC(&table);
        configValues.values[KipConfigValue_t6_tRTW] = cust_get_tRTW(&table);
        configValues.values[KipConfigValue_t7_tWTR] = cust_get_tWTR(&table);
        configValues.values[KipConfigValue_t8_tREFI] = cust_get_tREFI(&table);
        configValues.values[KipConfigValue_mem_burst_read_latency] = cust_get_burst_read_lat(&table);
        configValues.values[KipConfigValue_mem_burst_write_latency] = cust_get_burst_write_lat(&table);

        configValues.values[KipConfigValue_eristaCpuUV] = cust_get_erista_cpu_uv(&table);
        configValues.values[KipConfigValue_eristaCpuVmin] = cust_get_eristaCpuVmin(&table);
        configValues.values[KipConfigValue_eristaCpuMaxVolt] = cust_get_erista_cpu_max_volt(&table);
        configValues.values[KipConfigValue_eristaCpuUnlock] = cust_get_eristaCpuUnlock(&table);


        configValues.values[KipConfigValue_marikoCpuUVLow] = cust_get_mariko_cpu_uv_low(&table);
        configValues.values[KipConfigValue_marikoCpuUVHigh] = cust_get_mariko_cpu_uv_high(&table);
        configValues.values[KipConfigValue_tableConf] = cust_get_table_conf(&table);
        configValues.values[KipConfigValue_marikoCpuLowVmin] = cust_get_mariko_cpu_low_vmin(&table);
        configValues.values[KipConfigValue_marikoCpuHighVmin] = cust_get_mariko_cpu_high_vmin(&table);
        configValues.values[KipConfigValue_marikoCpuMaxVolt] = cust_get_mariko_cpu_max_volt(&table);
        configValues.values[KipConfigValue_marikoGpuFullUnlock] = cust_get_marikoCpuMaxClock(&table) / 1000;
        configValues.values[KipConfigValue_eristaCpuBoostClock] = cust_get_erista_cpu_boost(&table) / 1000;
        configValues.values[KipConfigValue_marikoCpuBoostClock] = cust_get_mariko_cpu_boost(&table) / 1000;

        configValues.values[KipConfigValue_eristaGpuUV] = cust_get_erista_gpu_uv(&table);
        configValues.values[KipConfigValue_eristaGpuVmin] = cust_get_erista_gpu_vmin(&table);
        configValues.values[KipConfigValue_marikoGpuUV] = cust_get_mariko_gpu_uv(&table);
        configValues.values[KipConfigValue_marikoGpuVmin] = cust_get_mariko_gpu_vmin(&table);
        configValues.values[KipConfigValue_marikoGpuVmax] = cust_get_mariko_gpu_vmax(&table);
        configValues.values[KipConfigValue_commonGpuVoltOffset] = cust_get_common_gpu_offset(&table);
        configValues.values[KipConfigValue_gpuSpeedo] = cust_get_gpu_speedo(&table);
        configValues.values[KipConfigValue_marikoGpuFullUnlock] = cust_get_mariko_gpu_unlock(&table);

        for (int i = 0; i < 24; i++) {
            configValues.values[KipConfigValue_g_volt_76800 + i] = cust_get_mariko_gpu_volt(&table, i);
        }

        for (int i = 0; i < 27; i++) {
            configValues.values[KipConfigValue_g_volt_e_76800 + i] = cust_get_erista_gpu_volt(&table, i);
        }

        // if(cust_get_cust_rev(&table) == KIP_CUST_REV)
        //     return;

        if (sizeof(SysClkConfigValueList) <= sizeof(configValues)) {
            if (this->config->SetConfigValues(&configValues, false)) {
                FileUtils::LogLine("[clock_manager] Successfully loaded KIP data into config");
            } else {
                FileUtils::LogLine("[clock_manager] Warning: Failed to set config values from KIP");
                writeNotification("Horizon OC\nKip config set failed");
            }
        } else {
            FileUtils::LogLine("[clock_manager] Error: Config value list buffer size mismatch");
            writeNotification("Horizon OC\nConfig Buffer Mismatch");
        }
    } else {
        FileUtils::LogLine("[clock_manager] Config refresh error in GetKipData!");
        writeNotification("Horizon OC\nConfig refresh failed");
    }
}