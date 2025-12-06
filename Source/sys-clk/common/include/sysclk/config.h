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

        // KIP config values
        case KipConfigValue_MTCConf:
            return pretty ? "MTC Config" : "kip_mtc_conf";
        case KipConfigValue_commonCpuBoostClock:
            return pretty ? "Common CPU Boost Clock" : "common_cpu_boost_clock";
        case KipConfigValue_commonEmcMemVolt:
            return pretty ? "Common EMC/MEM Voltage" : "common_emc_mem_volt";
        case KipConfigValue_eristaCpuMaxVolt:
            return pretty ? "Erista CPU Max Voltage" : "erista_cpu_max_volt";
        case KipConfigValue_eristaEmcMaxClock:
            return pretty ? "Erista EMC Max Clock" : "erista_emc_max_clock";
        case KipConfigValue_marikoCpuMaxVolt:
            return pretty ? "Mariko CPU Max Voltage" : "mariko_cpu_max_volt";
        case KipConfigValue_marikoEmcMaxClock:
            return pretty ? "Mariko EMC Max Clock" : "mariko_emc_max_clock";
        case KipConfigValue_marikoEmcVddqVolt:
            return pretty ? "Mariko EMC VDDQ Voltage" : "mariko_emc_vddq_volt";
        case KipConfigValue_marikoCpuUV:
            return pretty ? "Mariko CPU Undervolt" : "mariko_cpu_uv";
        case KipConfigValue_marikoGpuUV:
            return pretty ? "Mariko GPU Undervolt" : "mariko_gpu_uv";
        case KipConfigValue_eristaCpuUV:
            return pretty ? "Erista CPU Undervolt" : "erista_cpu_uv";
        case KipConfigValue_eristaGpuUV:
            return pretty ? "Erista GPU Undervolt" : "erista_gpu_uv";
        case KipConfigValue_commonGpuVoltOffset:
            return pretty ? "Common GPU Voltage Offset" : "common_gpu_volt_offset";
        case KipConfigValue_marikoEmcDvbShift:
            return pretty ? "Mariko EMC DVB Shift" : "mariko_emc_dvb_shift";
        case KipConfigValue_t1_tRCD:
            return pretty ? "t1 - tRCD" : "t1_trcd";
        case KipConfigValue_t2_tRP:
            return pretty ? "t2 - tRP" : "t2_trp";
        case KipConfigValue_t3_tRAS:
            return pretty ? "t3 - tRAS" : "t3_tras";
        case KipConfigValue_t4_tRRD:
            return pretty ? "t4 - tRRD" : "t4_trrd";
        case KipConfigValue_t5_tRFC:
            return pretty ? "t5 - tRFC" : "t5_trfc";
        case KipConfigValue_t6_tRTW:
            return pretty ? "t6 - tRTW" : "t6_trtw";
        case KipConfigValue_t7_tWTR:
            return pretty ? "t7 - tWTR" : "t7_twtr";
        case KipConfigValue_t8_tREFI:
            return pretty ? "t8 - tREFI" : "t8_trefi";
        case KipConfigValue_mem_burst_read_latency:
            return pretty ? "Memory Burst Read Latency" : "mem_burst_read_latency";
        case KipConfigValue_mem_burst_write_latency:
            return pretty ? "Memory Burst Write Latency" : "mem_burst_write_latency";
        case KipConfigValue_marikoCpuHighVmin:
            return pretty ? "Mariko CPU High Vmin" : "mariko_cpu_high_vmin";
        case KipConfigValue_marikoCpuLowVmin:
            return pretty ? "Mariko CPU Low Vmin" : "mariko_cpu_low_vmin";
        case KipConfigValue_eristaGpuVmin:
            return pretty ? "Erista GPU Vmin" : "erista_gpu_vmin";
        case KipConfigValue_marikoGpuVmin:
            return pretty ? "Mariko GPU Vmin" : "mariko_gpu_vmin";
        case KipConfigValue_marikoGpuVmax:
            return pretty ? "Mariko GPU Vmax" : "mariko_gpu_vmax";
        case KipConfigValue_marikoGpuFullUnlock:
            return pretty ? "Mariko GPU Full Unlock" : "mariko_gpu_full_unlock";

        // Mariko GPU voltages
        case KipConfigValue_g_volt_76800: return pretty ? "Mariko GPU Volt 76 MHz" : "g_volt_76800";
        case KipConfigValue_g_volt_153600: return pretty ? "Mariko GPU Volt 153 MHz" : "g_volt_153600";
        case KipConfigValue_g_volt_230400: return pretty ? "Mariko GPU Volt 230 MHz" : "g_volt_230400";
        case KipConfigValue_g_volt_307200: return pretty ? "Mariko GPU Volt 307 MHz" : "g_volt_307200";
        case KipConfigValue_g_volt_384000: return pretty ? "Mariko GPU Volt 384 MHz" : "g_volt_384000";
        case KipConfigValue_g_volt_460800: return pretty ? "Mariko GPU Volt 460 MHz" : "g_volt_460800";
        case KipConfigValue_g_volt_537600: return pretty ? "Mariko GPU Volt 537 MHz" : "g_volt_537600";
        case KipConfigValue_g_volt_614400: return pretty ? "Mariko GPU Volt 614 MHz" : "g_volt_614400";
        case KipConfigValue_g_volt_691200: return pretty ? "Mariko GPU Volt 691 MHz" : "g_volt_691200";
        case KipConfigValue_g_volt_768000: return pretty ? "Mariko GPU Volt 768 MHz" : "g_volt_768000";
        case KipConfigValue_g_volt_844800: return pretty ? "Mariko GPU Volt 844 MHz" : "g_volt_844800";
        case KipConfigValue_g_volt_921600: return pretty ? "Mariko GPU Volt 921 MHz" : "g_volt_921600";
        case KipConfigValue_g_volt_998400: return pretty ? "Mariko GPU Volt 998 MHz" : "g_volt_998400";
        case KipConfigValue_g_volt_1075200: return pretty ? "Mariko GPU Volt 1075 MHz" : "g_volt_1075200";
        case KipConfigValue_g_volt_1152000: return pretty ? "Mariko GPU Volt 1152 MHz" : "g_volt_1152000";
        case KipConfigValue_g_volt_1228800: return pretty ? "Mariko GPU Volt 1228 MHz" : "g_volt_1228800";
        case KipConfigValue_g_volt_1267200: return pretty ? "Mariko GPU Volt 1267 MHz" : "g_volt_1267200";
        case KipConfigValue_g_volt_1305600: return pretty ? "Mariko GPU Volt 1305 MHz" : "g_volt_1305600";
        case KipConfigValue_g_volt_1344000: return pretty ? "Mariko GPU Volt 1344 MHz" : "g_volt_1344000";
        case KipConfigValue_g_volt_1382400: return pretty ? "Mariko GPU Volt 1382 MHz" : "g_volt_1382400";
        case KipConfigValue_g_volt_1420800: return pretty ? "Mariko GPU Volt 1420 MHz" : "g_volt_1420800";
        case KipConfigValue_g_volt_1459200: return pretty ? "Mariko GPU Volt 1459 MHz" : "g_volt_1459200";
        case KipConfigValue_g_volt_1497600: return pretty ? "Mariko GPU Volt 1497 MHz" : "g_volt_1497600";
        case KipConfigValue_g_volt_1536000: return pretty ? "Mariko GPU Volt 1536 MHz" : "g_volt_1536000";

        // Erista GPU voltages
        case KipConfigValue_g_volt_e_76800: return pretty ? "Erista GPU Volt 76 MHz" : "g_volt_e_76800";
        case KipConfigValue_g_volt_e_115200: return pretty ? "Erista GPU Volt 115 MHz" : "g_volt_e_115200";
        case KipConfigValue_g_volt_e_153600: return pretty ? "Erista GPU Volt 153 MHz" : "g_volt_e_153600";
        case KipConfigValue_g_volt_e_192000: return pretty ? "Erista GPU Volt 192 MHz" : "g_volt_e_192000";
        case KipConfigValue_g_volt_e_230400: return pretty ? "Erista GPU Volt 230 MHz" : "g_volt_e_230400";
        case KipConfigValue_g_volt_e_268800: return pretty ? "Erista GPU Volt 268 MHz" : "g_volt_e_268800";
        case KipConfigValue_g_volt_e_307200: return pretty ? "Erista GPU Volt 307 MHz" : "g_volt_e_307200";
        case KipConfigValue_g_volt_e_345600: return pretty ? "Erista GPU Volt 345 MHz" : "g_volt_e_345600";
        case KipConfigValue_g_volt_e_384000: return pretty ? "Erista GPU Volt 384 MHz" : "g_volt_e_384000";
        case KipConfigValue_g_volt_e_422400: return pretty ? "Erista GPU Volt 422 MHz" : "g_volt_e_422400";
        case KipConfigValue_g_volt_e_460800: return pretty ? "Erista GPU Volt 460 MHz" : "g_volt_e_460800";
        case KipConfigValue_g_volt_e_499200: return pretty ? "Erista GPU Volt 499 MHz" : "g_volt_e_499200";
        case KipConfigValue_g_volt_e_537600: return pretty ? "Erista GPU Volt 537 MHz" : "g_volt_e_537600";
        case KipConfigValue_g_volt_e_576000: return pretty ? "Erista GPU Volt 576 MHz" : "g_volt_e_576000";
        case KipConfigValue_g_volt_e_614400: return pretty ? "Erista GPU Volt 614 MHz" : "g_volt_e_614400";
        case KipConfigValue_g_volt_e_652800: return pretty ? "Erista GPU Volt 652 MHz" : "g_volt_e_652800";
        case KipConfigValue_g_volt_e_691200: return pretty ? "Erista GPU Volt 691 MHz" : "g_volt_e_691200";
        case KipConfigValue_g_volt_e_729600: return pretty ? "Erista GPU Volt 729 MHz" : "g_volt_e_729600";
        case KipConfigValue_g_volt_e_768000: return pretty ? "Erista GPU Volt 768 MHz" : "g_volt_e_768000";
        case KipConfigValue_g_volt_e_806400: return pretty ? "Erista GPU Volt 806 MHz" : "g_volt_e_806400";
        case KipConfigValue_g_volt_e_844800: return pretty ? "Erista GPU Volt 844 MHz" : "g_volt_e_844800";
        case KipConfigValue_g_volt_e_883200: return pretty ? "Erista GPU Volt 883 MHz" : "g_volt_e_883200";
        case KipConfigValue_g_volt_e_921600: return pretty ? "Erista GPU Volt 921 MHz" : "g_volt_e_921600";
        case KipConfigValue_g_volt_e_960000: return pretty ? "Erista GPU Volt 960 MHz" : "g_volt_e_960000";
        case KipConfigValue_g_volt_e_998400: return pretty ? "Erista GPU Volt 998 MHz" : "g_volt_e_998400";
        case KipConfigValue_g_volt_e_1036800: return pretty ? "Erista GPU Volt 1036 MHz" : "g_volt_e_1036800";
        case KipConfigValue_g_volt_e_1075200: return pretty ? "Erista GPU Volt 1075 MHz" : "g_volt_e_1075200";

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
        
        case KipConfigValue_MTCConf:
        case KipConfigValue_commonCpuBoostClock:
        case KipConfigValue_commonEmcMemVolt:
        case KipConfigValue_eristaCpuMaxVolt:
        case KipConfigValue_eristaEmcMaxClock:
        case KipConfigValue_marikoCpuMaxVolt:
        case KipConfigValue_marikoEmcMaxClock:
        case KipConfigValue_marikoEmcVddqVolt:
        case KipConfigValue_marikoCpuUV:
        case KipConfigValue_marikoGpuUV:
        case KipConfigValue_eristaCpuUV:
        case KipConfigValue_eristaGpuUV:
        case KipConfigValue_commonGpuVoltOffset:
        case KipConfigValue_marikoEmcDvbShift:
        case KipConfigValue_t1_tRCD:
        case KipConfigValue_t2_tRP:
        case KipConfigValue_t3_tRAS:
        case KipConfigValue_t4_tRRD:
        case KipConfigValue_t5_tRFC:
        case KipConfigValue_t6_tRTW:
        case KipConfigValue_t7_tWTR:
        case KipConfigValue_t8_tREFI:
        case KipConfigValue_mem_burst_read_latency:
        case KipConfigValue_mem_burst_write_latency:
        case KipConfigValue_marikoCpuHighVmin:
        case KipConfigValue_marikoCpuLowVmin:
        case KipConfigValue_eristaGpuVmin:
        case KipConfigValue_marikoGpuVmin:
        case KipConfigValue_marikoGpuVmax:
        case KipConfigValue_marikoGpuFullUnlock:
        case KipConfigValue_g_volt_76800:
        case KipConfigValue_g_volt_153600:
        case KipConfigValue_g_volt_230400:
        case KipConfigValue_g_volt_307200:
        case KipConfigValue_g_volt_384000:
        case KipConfigValue_g_volt_460800:
        case KipConfigValue_g_volt_537600:
        case KipConfigValue_g_volt_614400:
        case KipConfigValue_g_volt_691200:
        case KipConfigValue_g_volt_768000:
        case KipConfigValue_g_volt_844800:
        case KipConfigValue_g_volt_921600:
        case KipConfigValue_g_volt_998400:
        case KipConfigValue_g_volt_1075200:
        case KipConfigValue_g_volt_1152000:
        case KipConfigValue_g_volt_1228800:
        case KipConfigValue_g_volt_1267200:
        case KipConfigValue_g_volt_1305600:
        case KipConfigValue_g_volt_1344000:
        case KipConfigValue_g_volt_1382400:
        case KipConfigValue_g_volt_1420800:
        case KipConfigValue_g_volt_1459200:
        case KipConfigValue_g_volt_1497600:
        case KipConfigValue_g_volt_1536000:
        case KipConfigValue_g_volt_e_76800:
        case KipConfigValue_g_volt_e_115200:
        case KipConfigValue_g_volt_e_153600:
        case KipConfigValue_g_volt_e_192000:
        case KipConfigValue_g_volt_e_230400:
        case KipConfigValue_g_volt_e_268800:
        case KipConfigValue_g_volt_e_307200:
        case KipConfigValue_g_volt_e_345600:
        case KipConfigValue_g_volt_e_384000:
        case KipConfigValue_g_volt_e_422400:
        case KipConfigValue_g_volt_e_460800:
        case KipConfigValue_g_volt_e_499200:
        case KipConfigValue_g_volt_e_537600:
        case KipConfigValue_g_volt_e_576000:
        case KipConfigValue_g_volt_e_614400:
        case KipConfigValue_g_volt_e_652800:
        case KipConfigValue_g_volt_e_691200:
        case KipConfigValue_g_volt_e_729600:
        case KipConfigValue_g_volt_e_768000:
        case KipConfigValue_g_volt_e_806400:
        case KipConfigValue_g_volt_e_844800:
        case KipConfigValue_g_volt_e_883200:
        case KipConfigValue_g_volt_e_921600:
        case KipConfigValue_g_volt_e_960000:
        case KipConfigValue_g_volt_e_998400:
        case KipConfigValue_g_volt_e_1036800:
        case KipConfigValue_g_volt_e_1075200:
            return input >= 0;
        default:
            return false;
    }
}