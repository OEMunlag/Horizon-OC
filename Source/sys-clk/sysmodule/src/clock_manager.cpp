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

#define HOSPPC_HAS_BOOST (hosversionAtLeast(7,0,0))

ClockManager *ClockManager::instance = NULL;

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
}

ClockManager::~ClockManager()
{
    delete this->config;
    delete this->context;
    delete this->kip;
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

void ClockManager::Tick()
{
    std::scoped_lock lock{this->contextMutex};
    std::uint32_t mode = 0;
    AppletOperationMode opMode = appletGetOperationMode();
    Result rc = apmExtGetCurrentPerformanceConfiguration(&mode);
    ASSERT_RESULT_OK(rc, "apmExtGetCurrentPerformanceConfiguration");

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

    u64 currentFreqIndex = findIndex(freqTable[SysClkModule_GPU].list, SYSCLK_FREQ_LIST_MAX, Board::GetHz(SysClkModule_GPU));
    if(this->config->GetConfigValue(HocClkConfigValue_HandheldGovernor)) {
        u64 targetHz = this->context->overrideFreqs[SysClkModule_GPU];
            if (!targetHz)
            {
                targetHz = this->config->GetAutoClockHz(this->context->applicationId, SysClkModule_GPU, this->context->profile);
                if(!targetHz)
                    targetHz = this->config->GetAutoClockHz(GLOBAL_PROFILE_ID, SysClkModule_GPU, this->context->profile);
            }
        if(Board::GetPartLoad(HocClkPartLoad_GPU) < 600) {
            currentFreqIndex--;
            if(currentFreqIndex < 0) {
                currentFreqIndex = 0;
            }
            Board::SetHz(SysClkModule_GPU, freqTable[SysClkModule_GPU].list[currentFreqIndex]);
        }
        if(Board::GetPartLoad(HocClkPartLoad_GPU) > 800) {
            currentFreqIndex++;

            if(!targetHz) {
                if(IsAssignableHz(SysClkModule_GPU, freqTable[SysClkModule_GPU].list[currentFreqIndex])) {
                    if(Board::GetSocType() == SysClkSocType_Mariko) {
                        if(freqTable[SysClkModule_GPU].list[currentFreqIndex] / 1000000 < this->config->GetConfigValue(HocClkConfigValue_MarikoMaxGpuClock))
                            currentFreqIndex++; // TODO: make this properly go back to target freq if the old target freq is more than one index away
                        else                    // probably needs the max clocks to be stored in hz instead of mhz to compare easily
                            currentFreqIndex--;
                    } else {
                        if(freqTable[SysClkModule_GPU].list[currentFreqIndex] / 1000000 < this->config->GetConfigValue(HocClkConfigValue_EristaMaxGpuClock))
                            currentFreqIndex++;
                        else
                            currentFreqIndex--;
                    }
                } else {
                    currentFreqIndex--;
                }
            } else {
                if(currentFreqIndex > findIndex(freqTable[SysClkModule_GPU].list, SYSCLK_FREQ_LIST_MAX, targetHz))
                    currentFreqIndex--;
            }
            Board::SetHz(SysClkModule_GPU, freqTable[SysClkModule_GPU].list[currentFreqIndex]);

        }
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

    return hasChanged;
}

void ClockManager::SetRNXRTMode(ReverseNXMode mode)
{
    this->rnxSync->SetRTMode(mode);
}

void ClockManager::SetKipData() {
    // ClockManager::GetInstance()->SetRNXRTMode(mode);
    // General KIP values
    this->kip->setMtcConf(this->config->GetConfigValue(KipConfigValue_MTCConf));
    this->kip->setCommonCpuBoostClock(this->config->GetConfigValue(KipConfigValue_commonCpuBoostClock));
    this->kip->setCommonEmcMemVolt(this->config->GetConfigValue(KipConfigValue_commonEmcMemVolt));

    // Erista specific
    this->kip->setEristaCpuMaxVolt(this->config->GetConfigValue(KipConfigValue_eristaCpuMaxVolt));
    this->kip->setEristaEmcMaxClock(this->config->GetConfigValue(KipConfigValue_eristaEmcMaxClock));

    // Mariko specific
    this->kip->setMarikoCpuMaxVolt(this->config->GetConfigValue(KipConfigValue_marikoCpuMaxVolt));
    this->kip->setMarikoEmcMaxClock(this->config->GetConfigValue(KipConfigValue_marikoEmcMaxClock));
    this->kip->setMarikoEmcVddqVolt(this->config->GetConfigValue(KipConfigValue_marikoEmcVddqVolt));

    // Undervolt / UV values
    this->kip->setMarikoCpuUV(this->config->GetConfigValue(KipConfigValue_marikoCpuUV));
    this->kip->setMarikoGpuUV(this->config->GetConfigValue(KipConfigValue_marikoGpuUV));
    this->kip->setEristaCpuUV(this->config->GetConfigValue(KipConfigValue_eristaCpuUV));
    this->kip->setEristaGpuUV(this->config->GetConfigValue(KipConfigValue_eristaGpuUV));

    // GPU offset / EMC DVB
    this->kip->setCommonGpuVoltOffset(this->config->GetConfigValue(KipConfigValue_commonGpuVoltOffset));
    this->kip->setMarikoEmcDvbShift(this->config->GetConfigValue(KipConfigValue_marikoEmcDvbShift));

    // Memory timing values
    this->kip->setT1_tRCD(this->config->GetConfigValue(KipConfigValue_t1_tRCD));
    this->kip->setT2_tRP(this->config->GetConfigValue(KipConfigValue_t2_tRP));
    this->kip->setT3_tRAS(this->config->GetConfigValue(KipConfigValue_t3_tRAS));
    this->kip->setT4_tRRD(this->config->GetConfigValue(KipConfigValue_t4_tRRD));
    this->kip->setT5_tRFC(this->config->GetConfigValue(KipConfigValue_t5_tRFC));
    this->kip->setT6_tRTW(this->config->GetConfigValue(KipConfigValue_t6_tRTW));
    this->kip->setT7_tWTR(this->config->GetConfigValue(KipConfigValue_t7_tWTR));
    this->kip->setT8_tREFI(this->config->GetConfigValue(KipConfigValue_t8_tREFI));

    this->kip->setMemBurstReadLatency(this->config->GetConfigValue(KipConfigValue_mem_burst_read_latency));
    this->kip->setMemBurstWriteLatency(this->config->GetConfigValue(KipConfigValue_mem_burst_write_latency));

    // Additional voltages
    this->kip->setMarikoCpuHighVmin(this->config->GetConfigValue(KipConfigValue_marikoCpuHighVmin));
    this->kip->setMarikoCpuLowVmin(this->config->GetConfigValue(KipConfigValue_marikoCpuLowVmin));

    this->kip->setEristaGpuVmin(this->config->GetConfigValue(KipConfigValue_eristaGpuVmin));
    this->kip->setMarikoGpuVmin(this->config->GetConfigValue(KipConfigValue_marikoGpuVmin));
    this->kip->setMarikoGpuVmax(this->config->GetConfigValue(KipConfigValue_marikoGpuVmax));

    this->kip->setMarikoGpuFullUnlock(this->config->GetConfigValue(KipConfigValue_marikoGpuFullUnlock));

    // Mariko GPU voltages
    this->kip->setGVolt76800(this->config->GetConfigValue(KipConfigValue_g_volt_76800));
    this->kip->setGVolt153600(this->config->GetConfigValue(KipConfigValue_g_volt_153600));
    this->kip->setGVolt230400(this->config->GetConfigValue(KipConfigValue_g_volt_230400));
    this->kip->setGVolt307200(this->config->GetConfigValue(KipConfigValue_g_volt_307200));
    this->kip->setGVolt384000(this->config->GetConfigValue(KipConfigValue_g_volt_384000));
    this->kip->setGVolt460800(this->config->GetConfigValue(KipConfigValue_g_volt_460800));
    this->kip->setGVolt537600(this->config->GetConfigValue(KipConfigValue_g_volt_537600));
    this->kip->setGVolt614400(this->config->GetConfigValue(KipConfigValue_g_volt_614400));
    this->kip->setGVolt691200(this->config->GetConfigValue(KipConfigValue_g_volt_691200));
    this->kip->setGVolt768000(this->config->GetConfigValue(KipConfigValue_g_volt_768000));
    this->kip->setGVolt844800(this->config->GetConfigValue(KipConfigValue_g_volt_844800));
    this->kip->setGVolt921600(this->config->GetConfigValue(KipConfigValue_g_volt_921600));
    this->kip->setGVolt998400(this->config->GetConfigValue(KipConfigValue_g_volt_998400));
    this->kip->setGVolt1075200(this->config->GetConfigValue(KipConfigValue_g_volt_1075200));
    this->kip->setGVolt1152000(this->config->GetConfigValue(KipConfigValue_g_volt_1152000));
    this->kip->setGVolt1228800(this->config->GetConfigValue(KipConfigValue_g_volt_1228800));
    this->kip->setGVolt1267200(this->config->GetConfigValue(KipConfigValue_g_volt_1267200));
    this->kip->setGVolt1305600(this->config->GetConfigValue(KipConfigValue_g_volt_1305600));
    this->kip->setGVolt1344000(this->config->GetConfigValue(KipConfigValue_g_volt_1344000));
    this->kip->setGVolt1382400(this->config->GetConfigValue(KipConfigValue_g_volt_1382400));
    this->kip->setGVolt1420800(this->config->GetConfigValue(KipConfigValue_g_volt_1420800));
    this->kip->setGVolt1459200(this->config->GetConfigValue(KipConfigValue_g_volt_1459200));
    this->kip->setGVolt1497600(this->config->GetConfigValue(KipConfigValue_g_volt_1497600));
    this->kip->setGVolt1536000(this->config->GetConfigValue(KipConfigValue_g_volt_1536000));

    // Erista GPU voltages
    this->kip->setGVoltE76800(this->config->GetConfigValue(KipConfigValue_g_volt_e_76800));
    this->kip->setGVoltE115200(this->config->GetConfigValue(KipConfigValue_g_volt_e_115200));
    this->kip->setGVoltE153600(this->config->GetConfigValue(KipConfigValue_g_volt_e_153600));
    this->kip->setGVoltE192000(this->config->GetConfigValue(KipConfigValue_g_volt_e_192000));
    this->kip->setGVoltE230400(this->config->GetConfigValue(KipConfigValue_g_volt_e_230400));
    this->kip->setGVoltE268800(this->config->GetConfigValue(KipConfigValue_g_volt_e_268800));
    this->kip->setGVoltE307200(this->config->GetConfigValue(KipConfigValue_g_volt_e_307200));
    this->kip->setGVoltE345600(this->config->GetConfigValue(KipConfigValue_g_volt_e_345600));
    this->kip->setGVoltE384000(this->config->GetConfigValue(KipConfigValue_g_volt_e_384000));
    this->kip->setGVoltE422400(this->config->GetConfigValue(KipConfigValue_g_volt_e_422400));
    this->kip->setGVoltE460800(this->config->GetConfigValue(KipConfigValue_g_volt_e_460800));
    this->kip->setGVoltE499200(this->config->GetConfigValue(KipConfigValue_g_volt_e_499200));
    this->kip->setGVoltE537600(this->config->GetConfigValue(KipConfigValue_g_volt_e_537600));
    this->kip->setGVoltE576000(this->config->GetConfigValue(KipConfigValue_g_volt_e_576000));
    this->kip->setGVoltE614400(this->config->GetConfigValue(KipConfigValue_g_volt_e_614400));
    this->kip->setGVoltE652800(this->config->GetConfigValue(KipConfigValue_g_volt_e_652800));
    this->kip->setGVoltE691200(this->config->GetConfigValue(KipConfigValue_g_volt_e_691200));
    this->kip->setGVoltE729600(this->config->GetConfigValue(KipConfigValue_g_volt_e_729600));
    this->kip->setGVoltE768000(this->config->GetConfigValue(KipConfigValue_g_volt_e_768000));
    this->kip->setGVoltE806400(this->config->GetConfigValue(KipConfigValue_g_volt_e_806400));
    this->kip->setGVoltE844800(this->config->GetConfigValue(KipConfigValue_g_volt_e_844800));
    this->kip->setGVoltE883200(this->config->GetConfigValue(KipConfigValue_g_volt_e_883200));
    this->kip->setGVoltE921600(this->config->GetConfigValue(KipConfigValue_g_volt_e_921600));
    this->kip->setGVoltE960000(this->config->GetConfigValue(KipConfigValue_g_volt_e_960000));
    this->kip->setGVoltE998400(this->config->GetConfigValue(KipConfigValue_g_volt_e_998400));
    this->kip->setGVoltE1036800(this->config->GetConfigValue(KipConfigValue_g_volt_e_1036800));
    this->kip->setGVoltE1075200(this->config->GetConfigValue(KipConfigValue_g_volt_e_1075200));

    // Persist changes to KIP
    this->kip->writeKip();

}

void ClockManager::GetKipConfigValues() {
    // General KIP values
    this->config->SetConfigValue(KipConfigValue_MTCConf, this->kip->getMtcConf());
    this->config->SetConfigValue(KipConfigValue_commonCpuBoostClock, this->kip->getCommonCpuBoostClock());
    this->config->SetConfigValue(KipConfigValue_commonEmcMemVolt, this->kip->getCommonEmcMemVolt());

    // Erista specific
    this->config->SetConfigValue(KipConfigValue_eristaCpuMaxVolt, this->kip->getEristaCpuMaxVolt());
    this->config->SetConfigValue(KipConfigValue_eristaEmcMaxClock, this->kip->getEristaEmcMaxClock());

    // Mariko specific
    this->config->SetConfigValue(KipConfigValue_marikoCpuMaxVolt, this->kip->getMarikoCpuMaxVolt());
    this->config->SetConfigValue(KipConfigValue_marikoEmcMaxClock, this->kip->getMarikoEmcMaxClock());
    this->config->SetConfigValue(KipConfigValue_marikoEmcVddqVolt, this->kip->getMarikoEmcVddqVolt());

    // Undervolt / UV values
    this->config->SetConfigValue(KipConfigValue_marikoCpuUV, this->kip->getMarikoCpuUV());
    this->config->SetConfigValue(KipConfigValue_marikoGpuUV, this->kip->getMarikoGpuUV());
    this->config->SetConfigValue(KipConfigValue_eristaCpuUV, this->kip->getEristaCpuUV());
    this->config->SetConfigValue(KipConfigValue_eristaGpuUV, this->kip->getEristaGpuUV());

    // GPU offset / EMC DVB
    this->config->SetConfigValue(KipConfigValue_commonGpuVoltOffset, this->kip->getCommonGpuVoltOffset());
    this->config->SetConfigValue(KipConfigValue_marikoEmcDvbShift, this->kip->getMarikoEmcDvbShift());

    // Memory timing values
    this->config->SetConfigValue(KipConfigValue_t1_tRCD, this->kip->getT1_tRCD());
    this->config->SetConfigValue(KipConfigValue_t2_tRP, this->kip->getT2_tRP());
    this->config->SetConfigValue(KipConfigValue_t3_tRAS, this->kip->getT3_tRAS());
    this->config->SetConfigValue(KipConfigValue_t4_tRRD, this->kip->getT4_tRRD());
    this->config->SetConfigValue(KipConfigValue_t5_tRFC, this->kip->getT5_tRFC());
    this->config->SetConfigValue(KipConfigValue_t6_tRTW, this->kip->getT6_tRTW());
    this->config->SetConfigValue(KipConfigValue_t7_tWTR, this->kip->getT7_tWTR());
    this->config->SetConfigValue(KipConfigValue_t8_tREFI, this->kip->getT8_tREFI());

    this->config->SetConfigValue(KipConfigValue_mem_burst_read_latency, this->kip->getMemBurstReadLatency());
    this->config->SetConfigValue(KipConfigValue_mem_burst_write_latency, this->kip->getMemBurstWriteLatency());

    // Additional voltages
    this->config->SetConfigValue(KipConfigValue_marikoCpuHighVmin, this->kip->getMarikoCpuHighVmin());
    this->config->SetConfigValue(KipConfigValue_marikoCpuLowVmin, this->kip->getMarikoCpuLowVmin());

    this->config->SetConfigValue(KipConfigValue_eristaGpuVmin, this->kip->getEristaGpuVmin());
    this->config->SetConfigValue(KipConfigValue_marikoGpuVmin, this->kip->getMarikoGpuVmin());
    this->config->SetConfigValue(KipConfigValue_marikoGpuVmax, this->kip->getMarikoGpuVmax());

    this->config->SetConfigValue(KipConfigValue_marikoGpuFullUnlock, this->kip->getMarikoGpuFullUnlock());

    // Mariko GPU voltages
    this->config->SetConfigValue(KipConfigValue_g_volt_76800, this->kip->getGVolt76800());
    this->config->SetConfigValue(KipConfigValue_g_volt_153600, this->kip->getGVolt153600());
    this->config->SetConfigValue(KipConfigValue_g_volt_230400, this->kip->getGVolt230400());
    this->config->SetConfigValue(KipConfigValue_g_volt_307200, this->kip->getGVolt307200());
    this->config->SetConfigValue(KipConfigValue_g_volt_384000, this->kip->getGVolt384000());
    this->config->SetConfigValue(KipConfigValue_g_volt_460800, this->kip->getGVolt460800());
    this->config->SetConfigValue(KipConfigValue_g_volt_537600, this->kip->getGVolt537600());
    this->config->SetConfigValue(KipConfigValue_g_volt_614400, this->kip->getGVolt614400());
    this->config->SetConfigValue(KipConfigValue_g_volt_691200, this->kip->getGVolt691200());
    this->config->SetConfigValue(KipConfigValue_g_volt_768000, this->kip->getGVolt768000());
    this->config->SetConfigValue(KipConfigValue_g_volt_844800, this->kip->getGVolt844800());
    this->config->SetConfigValue(KipConfigValue_g_volt_921600, this->kip->getGVolt921600());
    this->config->SetConfigValue(KipConfigValue_g_volt_998400, this->kip->getGVolt998400());
    this->config->SetConfigValue(KipConfigValue_g_volt_1075200, this->kip->getGVolt1075200());
    this->config->SetConfigValue(KipConfigValue_g_volt_1152000, this->kip->getGVolt1152000());
    this->config->SetConfigValue(KipConfigValue_g_volt_1228800, this->kip->getGVolt1228800());
    this->config->SetConfigValue(KipConfigValue_g_volt_1267200, this->kip->getGVolt1267200());
    this->config->SetConfigValue(KipConfigValue_g_volt_1305600, this->kip->getGVolt1305600());
    this->config->SetConfigValue(KipConfigValue_g_volt_1344000, this->kip->getGVolt1344000());
    this->config->SetConfigValue(KipConfigValue_g_volt_1382400, this->kip->getGVolt1382400());
    this->config->SetConfigValue(KipConfigValue_g_volt_1420800, this->kip->getGVolt1420800());
    this->config->SetConfigValue(KipConfigValue_g_volt_1459200, this->kip->getGVolt1459200());
    this->config->SetConfigValue(KipConfigValue_g_volt_1497600, this->kip->getGVolt1497600());
    this->config->SetConfigValue(KipConfigValue_g_volt_1536000, this->kip->getGVolt1536000());

    // Erista GPU voltages
    this->config->SetConfigValue(KipConfigValue_g_volt_e_76800, this->kip->getGVoltE76800());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_115200, this->kip->getGVoltE115200());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_153600, this->kip->getGVoltE153600());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_192000, this->kip->getGVoltE192000());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_230400, this->kip->getGVoltE230400());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_268800, this->kip->getGVoltE268800());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_307200, this->kip->getGVoltE307200());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_345600, this->kip->getGVoltE345600());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_384000, this->kip->getGVoltE384000());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_422400, this->kip->getGVoltE422400());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_460800, this->kip->getGVoltE460800());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_499200, this->kip->getGVoltE499200());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_537600, this->kip->getGVoltE537600());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_576000, this->kip->getGVoltE576000());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_614400, this->kip->getGVoltE614400());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_652800, this->kip->getGVoltE652800());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_691200, this->kip->getGVoltE691200());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_729600, this->kip->getGVoltE729600());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_768000, this->kip->getGVoltE768000());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_806400, this->kip->getGVoltE806400());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_844800, this->kip->getGVoltE844800());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_883200, this->kip->getGVoltE883200());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_921600, this->kip->getGVoltE921600());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_960000, this->kip->getGVoltE960000());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_998400, this->kip->getGVoltE998400());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_1036800, this->kip->getGVoltE1036800());
    this->config->SetConfigValue(KipConfigValue_g_volt_e_1075200, this->kip->getGVoltE1075200());
}