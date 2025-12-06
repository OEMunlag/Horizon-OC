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


#pragma once

#include <stdint.h>
#include <stddef.h>

typedef enum {
    SysClkConfigValue_PollingIntervalMs = 0,
    SysClkConfigValue_TempLogIntervalMs,
    SysClkConfigValue_FreqLogIntervalMs,
    SysClkConfigValue_PowerLogIntervalMs,
    SysClkConfigValue_CsvWriteIntervalMs,

    HocClkConfigValue_UncappedClocks,
    HocClkConfigValue_OverwriteBoostMode,

    HocClkConfigValue_EristaMaxCpuClock,
    HocClkConfigValue_EristaMaxGpuClock,
    HocClkConfigValue_EristaMaxMemClock,
    HocClkConfigValue_MarikoMaxCpuClock,
    HocClkConfigValue_MarikoMaxGpuClock,
    HocClkConfigValue_MarikoMaxMemClock,

    HocClkConfigValue_ThermalThrottle,
    HocClkConfigValue_ThermalThrottleThreshold,

    HocClkConfigValue_HandheldGovernor,
    HocClkConfigValue_DockedGovernor,

    HocClkConfigValue_HandheldTDP,
    HocClkConfigValue_HandheldTDPLimit,

    HocClkConfigValue_LiteTDPLimit,

    HocClkConfigValue_EnforceBoardLimit,

    KipConfigValue_MTCConf,
    KipConfigValue_commonCpuBoostClock,
    KipConfigValue_commonEmcMemVolt,
    KipConfigValue_eristaCpuMaxVolt,
    KipConfigValue_eristaEmcMaxClock,
    KipConfigValue_marikoCpuMaxVolt,
    KipConfigValue_marikoEmcMaxClock,
    KipConfigValue_marikoEmcVddqVolt,
    KipConfigValue_marikoCpuUV,
    KipConfigValue_marikoGpuUV,
    KipConfigValue_eristaCpuUV,
    KipConfigValue_eristaGpuUV,
    KipConfigValue_commonGpuVoltOffset,
    KipConfigValue_marikoEmcDvbShift,
    KipConfigValue_t1_tRCD,
    KipConfigValue_t2_tRP,
    KipConfigValue_t3_tRAS,
    KipConfigValue_t4_tRRD,
    KipConfigValue_t5_tRFC,
    KipConfigValue_t6_tRTW,
    KipConfigValue_t7_tWTR,
    KipConfigValue_t8_tREFI,

    KipConfigValue_mem_burst_read_latency,
    KipConfigValue_mem_burst_write_latency,

    KipConfigValue_marikoCpuHighVmin,
    KipConfigValue_marikoCpuLowVmin,

    KipConfigValue_eristaGpuVmin,
    KipConfigValue_marikoGpuVmin,
    KipConfigValue_marikoGpuVmax,

    KipConfigValue_marikoGpuFullUnlock,

    // Mariko GPU voltages
    KipConfigValue_g_volt_76800,
    KipConfigValue_g_volt_153600,
    KipConfigValue_g_volt_230400,
    KipConfigValue_g_volt_307200,
    KipConfigValue_g_volt_384000,
    KipConfigValue_g_volt_460800,
    KipConfigValue_g_volt_537600,
    KipConfigValue_g_volt_614400,
    KipConfigValue_g_volt_691200,
    KipConfigValue_g_volt_768000,
    KipConfigValue_g_volt_844800,
    KipConfigValue_g_volt_921600,
    KipConfigValue_g_volt_998400,
    KipConfigValue_g_volt_1075200,
    KipConfigValue_g_volt_1152000,
    KipConfigValue_g_volt_1228800,
    KipConfigValue_g_volt_1267200,
    KipConfigValue_g_volt_1305600,
    KipConfigValue_g_volt_1344000,
    KipConfigValue_g_volt_1382400,
    KipConfigValue_g_volt_1420800,
    KipConfigValue_g_volt_1459200,
    KipConfigValue_g_volt_1497600,
    KipConfigValue_g_volt_1536000,

    // Erista GPU voltages
    KipConfigValue_g_volt_e_76800,
    KipConfigValue_g_volt_e_115200,
    KipConfigValue_g_volt_e_153600,
    KipConfigValue_g_volt_e_192000,
    KipConfigValue_g_volt_e_230400,
    KipConfigValue_g_volt_e_268800,
    KipConfigValue_g_volt_e_307200,
    KipConfigValue_g_volt_e_345600,
    KipConfigValue_g_volt_e_384000,
    KipConfigValue_g_volt_e_422400,
    KipConfigValue_g_volt_e_460800,
    KipConfigValue_g_volt_e_499200,
    KipConfigValue_g_volt_e_537600,
    KipConfigValue_g_volt_e_576000,
    KipConfigValue_g_volt_e_614400,
    KipConfigValue_g_volt_e_652800,
    KipConfigValue_g_volt_e_691200,
    KipConfigValue_g_volt_e_729600,
    KipConfigValue_g_volt_e_768000,
    KipConfigValue_g_volt_e_806400,
    KipConfigValue_g_volt_e_844800,
    KipConfigValue_g_volt_e_883200,
    KipConfigValue_g_volt_e_921600,
    KipConfigValue_g_volt_e_960000,
    KipConfigValue_g_volt_e_998400,
    KipConfigValue_g_volt_e_1036800,
    KipConfigValue_g_volt_e_1075200,

    SysClkConfigValue_EnumMax,
} SysClkConfigValue;

typedef struct {
    uint64_t values[SysClkConfigValue_EnumMax];
} SysClkConfigValueList;

static inline const char* sysclkFormatConfigValue(SysClkConfigValue val, bool pretty)
{
    switch(val)
    {
        case SysClkConfigValue_PollingIntervalMs:
            return pretty ? "Polling Interval (ms)" : "poll_interval_ms";
        case SysClkConfigValue_TempLogIntervalMs:
            return pretty ? "Temperature logging interval (ms)" : "temp_log_interval_ms";
        case SysClkConfigValue_FreqLogIntervalMs:
            return pretty ? "Frequency logging interval (ms)" : "freq_log_interval_ms";
        case SysClkConfigValue_PowerLogIntervalMs:
            return pretty ? "Power logging interval (ms)" : "power_log_interval_ms";
        case SysClkConfigValue_CsvWriteIntervalMs:
            return pretty ? "CSV write interval (ms)" : "csv_write_interval_ms";
        case HocClkConfigValue_UncappedClocks:
            return pretty ? "Uncapped Clocks" : "uncapped_clocks";
        case HocClkConfigValue_OverwriteBoostMode:
            return pretty ? "Overwrite Boost Mode" : "ow_boost";

        case HocClkConfigValue_EristaMaxCpuClock:
            return pretty ? "Max CPU Clock" : "cpu_max_e";
        case HocClkConfigValue_EristaMaxGpuClock:
            return pretty ? "Max GPU Clock" : "gpu_max_e";
        case HocClkConfigValue_EristaMaxMemClock:
            return pretty ? "Max MEM Clock" : "mem_max_e";

        case HocClkConfigValue_MarikoMaxCpuClock:
            return pretty ? "Max CPU Clock" : "cpu_max_m";
        case HocClkConfigValue_MarikoMaxGpuClock:
            return pretty ? "Max GPU Clock" : "gpu_max_m";
        case HocClkConfigValue_MarikoMaxMemClock:
            return pretty ? "Max MEM Clock" : "mem_max_m";

        case HocClkConfigValue_ThermalThrottle:
            return pretty ? "Thermal Throttle" : "thermal_throttle";

        case HocClkConfigValue_ThermalThrottleThreshold:
            return pretty ? "Thermal Throttle Threshold" : "thermal_throttle_threshold";

        case HocClkConfigValue_DockedGovernor:
            return pretty ? "Docked Governor" : "governor_docked";
        case HocClkConfigValue_HandheldGovernor:
            return pretty ? "Handheld Governor" : "governor_handheld";

        case HocClkConfigValue_HandheldTDP:
            return pretty ? "Handheld TDP" : "handheld_tdp";

        case HocClkConfigValue_HandheldTDPLimit:
            return pretty ? "TDP Limit" : "tdp_limit";

        case HocClkConfigValue_LiteTDPLimit:
            return pretty ? "Lite TDP Limit" : "tdp_limit_l";

        default:
            return pretty ? "Null" : "null";
    }
}

static inline uint64_t sysclkDefaultConfigValue(SysClkConfigValue val)
{
    switch(val)
    {
        case SysClkConfigValue_PollingIntervalMs:
            return 300ULL;
        case SysClkConfigValue_TempLogIntervalMs:
        case SysClkConfigValue_FreqLogIntervalMs:
        case SysClkConfigValue_PowerLogIntervalMs:
        case SysClkConfigValue_CsvWriteIntervalMs:
        case HocClkConfigValue_UncappedClocks:
        case HocClkConfigValue_OverwriteBoostMode:
            return 0ULL;
        case HocClkConfigValue_EristaMaxCpuClock:
            return 1785ULL;
        case HocClkConfigValue_EristaMaxGpuClock:
            return 921ULL;
        case HocClkConfigValue_EristaMaxMemClock:
            return 1600ULL;

        case HocClkConfigValue_MarikoMaxCpuClock:
            return 1963ULL;
        case HocClkConfigValue_MarikoMaxGpuClock:
            return 1075ULL;
        case HocClkConfigValue_MarikoMaxMemClock:
            return 1862ULL;

        case HocClkConfigValue_ThermalThrottle:
        case HocClkConfigValue_DockedGovernor:
        case HocClkConfigValue_HandheldGovernor:
        case HocClkConfigValue_HandheldTDP:
            return 1ULL;
        case HocClkConfigValue_ThermalThrottleThreshold:
            return 70ULL;
        case HocClkConfigValue_HandheldTDPLimit:
            return 8600ULL;
        case HocClkConfigValue_LiteTDPLimit:
            return 6400ULL;
        default:
            return 0ULL;
    }
}

static inline uint64_t sysclkValidConfigValue(SysClkConfigValue val, uint64_t input)
{
    switch(val)
    {
        case HocClkConfigValue_EristaMaxCpuClock:
        case HocClkConfigValue_EristaMaxGpuClock:
        case HocClkConfigValue_EristaMaxMemClock:
        case HocClkConfigValue_MarikoMaxCpuClock:
        case HocClkConfigValue_MarikoMaxGpuClock:
        case HocClkConfigValue_MarikoMaxMemClock:
        case HocClkConfigValue_ThermalThrottleThreshold:
        case HocClkConfigValue_HandheldTDPLimit:
        case HocClkConfigValue_LiteTDPLimit:
        case SysClkConfigValue_PollingIntervalMs:
            return input > 0;
        case SysClkConfigValue_TempLogIntervalMs:
        case SysClkConfigValue_FreqLogIntervalMs:
        case SysClkConfigValue_PowerLogIntervalMs:
        case SysClkConfigValue_CsvWriteIntervalMs:
        case HocClkConfigValue_UncappedClocks:
        case HocClkConfigValue_OverwriteBoostMode:
        case HocClkConfigValue_ThermalThrottle:
        case HocClkConfigValue_DockedGovernor:
        case HocClkConfigValue_HandheldGovernor:
        case HocClkConfigValue_HandheldTDP:
            return (input & 0x1) == input;
        default:
            return false;
    }
}