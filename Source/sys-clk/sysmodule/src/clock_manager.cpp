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
                    case SysClkSocType_MarikoLite:
                        return 537600000;
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

void ClockManager::Tick()
{
    std::scoped_lock lock{this->contextMutex};
    if (this->RefreshContext() || this->config->Refresh())
    {
        std::uint32_t targetHz = 0;
        std::uint32_t maxHz = 0;
        std::uint32_t nearestHz = 0;
        std::uint32_t mode = 0;

        if(this->config->GetConfigValue(HocClkConfigValue_EMCVdd2VoltageUV) < 1400000) { // Safety Check
            set_sd1_voltage((u32)this->config->GetConfigValue(HocClkConfigValue_EMCVdd2VoltageUV));
        }

        AppletOperationMode opMode = appletGetOperationMode();
        Result rc = apmExtGetCurrentPerformanceConfiguration(&mode);
        ASSERT_RESULT_OK(rc, "apmExtGetCurrentPerformanceConfiguration");

        if(this->config->GetConfigValue(HocClkConfigValue_HandheldTDP) && opMode == AppletOperationMode_Handheld) {
                if(Board::GetSocType() == SysClkSocType_MarikoLite) {
                    if(Board::GetPowerMw(SysClkPowerSensor_Avg) < -(int)this->config->GetConfigValue(HocClkConfigValue_LiteTDPLimit)) {
                        ResetToStockClocks();
                        return;
                    }
                } else {
                    if(Board::GetPowerMw(SysClkPowerSensor_Avg) < -(int)this->config->GetConfigValue(HocClkConfigValue_HandheldTDPLimit)) {
                        ResetToStockClocks();
                        return;
                    }
                }
            } else if(opMode == AppletOperationMode_Console && this->config->GetConfigValue(HocClkConfigValue_EnforceBoardLimit)) {
                if(Board::GetPowerMw(SysClkPowerSensor_Avg) < 0) {
                    ResetToStockClocks();
                    return;
                }
            }

        if(apmExtIsBoostMode(mode) && !this->config->GetConfigValue(HocClkConfigValue_OverwriteBoostMode)) {
            ResetToStockClocks();
            return;
        }

        if(((tmp451TempSoc() / 1000) > (int)this->config->GetConfigValue(HocClkConfigValue_ThermalThrottleThreshold)) && this->config->GetConfigValue(HocClkConfigValue_ThermalThrottle)) {
            ResetToStockClocks();
            return;
        }
        if(this->config->GetConfigValue(HocClkConfigValue_HandheldGovernor) && opMode == AppletOperationMode_Handheld) {
            
        }
        if(this->config->GetConfigValue(HocClkConfigValue_DockedGovernor) && opMode == AppletOperationMode_Console) {
            
        }
        for (unsigned int module = 0; module < SysClkModule_EnumMax; module++)
        {
                targetHz = this->context->overrideFreqs[module];
                globalTargetHz = this->context->overrideFreqs[module];

                if (!targetHz)
                {
                    targetHz = this->config->GetAutoClockHz(this->context->applicationId, (SysClkModule)module, this->context->profile);
                    globalTargetHz = this->config->GetAutoClockHz(SYSCLK_GLOBAL_PROFILE_TID, (SysClkModule)module, this->context->profile);
                }

                if (targetHz || globalTargetHz)
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
    for (unsigned int loadSource = 0; loadSource < SysClkRamLoad_EnumMax; loadSource++)
    {
        this->context->ramLoad[loadSource] = Board::GetRamLoad((SysClkRamLoad)loadSource);
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

void ClockManager::set_sd1_voltage(uint32_t voltage_uv)
{
	// SD1 parameters
	const u32 uv_step = 12500;
	const u32 uv_min = 1100000;
	const u32 uv_max = 1350000;
	const u8 volt_addr = 0x17;      // MAX77620_REG_SD1
	const u8 volt_mask = 0x7F;      // MAX77620_SD1_VOLT_MASK

	// Validate input voltage
	if (voltage_uv < uv_min || voltage_uv > uv_max)
		return;

	// Calculate voltage multiplier
	u32 mult = (voltage_uv + uv_step - 1 - uv_min) / uv_step;
	mult = mult & volt_mask;

	// Open I2C session to MAX77620 PMIC
	I2cSession session;
	Result res = i2cOpenSession(&session, I2cDevice_Max77620Pmic);
	if (R_FAILED(res)) {
		return;
	}

	// Read current register value
	u8 current_val = 0;
	res = i2csessionSendAuto(&session, &volt_addr, 1, I2cTransactionOption_Start);
	if (R_FAILED(res)) {
		i2csessionClose(&session);
		return;
	}

	res = i2csessionReceiveAuto(&session, &current_val, 1, I2cTransactionOption_Stop);
	if (R_FAILED(res)) {
		i2csessionClose(&session);
		return;
	}

	// Mask in the new voltage bits, preserving other bits
	u8 new_val = (current_val & ~volt_mask) | mult;

	// Write back register with START and STOP conditions
	u8 write_buf[2] = {volt_addr, new_val};
	res = i2csessionSendAuto(&session, write_buf, sizeof(write_buf), I2cTransactionOption_All);

	i2csessionClose(&session);
}