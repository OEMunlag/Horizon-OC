/*
 * Copyright (C) Switch-OC-Suite
 *
 * Copyright (c) 2023 hanai3Bi
 *
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
 */

 #pragma once

 #define CUST_REV 11

 #include "oc_common.hpp"
 #include "pcv/pcv_common.hpp"

 namespace ams::ldr::oc {

 #include "mtc_timing_table.hpp"

 enum MtcConfig: u32 {
     AUTO_ADJ = 0,
     AUTO_ADJ_BL = 1,
 };

 using CustomizeCpuDvfsTable = pcv::cvb_entry_t[pcv::DvfsTableEntryLimit];
 using CustomizeGpuDvfsTable = pcv::cvb_entry_t[pcv::DvfsTableEntryLimit];
 static_assert(sizeof(CustomizeCpuDvfsTable) == sizeof(CustomizeGpuDvfsTable));
 static_assert(sizeof(CustomizeCpuDvfsTable) == sizeof(pcv::cvb_entry_t) * pcv::DvfsTableEntryLimit);

 constexpr uint32_t ERISTA_MTC_MAGIC = 0x43544D45; // EMTC
 constexpr uint32_t MARIKO_MTC_MAGIC = 0x43544D4D; // MMTC

 typedef struct CustomizeTable {
     u8  cust[4] = {'C', 'U', 'S', 'T'};
     u32 custRev = CUST_REV;
     u32 mtcConf = AUTO_ADJ;
     u32 hpMode;
     u32 commonCpuBoostClock;
     u32 commonEmcMemVolt;
     u32 eristaCpuMaxVolt;
     u32 eristaEmcMaxClock;
     u32 marikoCpuMaxVolt;
     u32 marikoEmcMaxClock;
     u32 marikoEmcVddqVolt;
     u32 marikoCpuUV;
     u32 marikoGpuUV;

     u32 eristaCpuUV;
     u32 eristaGpuUV;

     u32 enableMarikoGpuUnsafeFreqs;
     u32 enableEristaGpuUnsafeFreqs;

     u32 enableMarikoCpuUnsafeFreqs;
     u32 enableEristaCpuUnsafeFreqs;

     u32 commonGpuVoltOffset;

     u32 EmcDvbShift;

     // advanced config
     u32 t1_tRCD;
     u32 t2_tRP;
     u32 t3_tRAS;
     u32 t4_tRRD;
     u32 t5_tRFC;
     u32 t6_tRTW;
     u32 t7_tWTR;
     u32 t8_tREFI;
     u32 mem_burst_latency;

     u32 marikoCpuVmin;

     u32 eristaGpuVmin;
     u32 marikoGpuVmin;
     u32 marikoGpuVmax;

     u32 marikoGpuVoltArray[24];
     u32 eristaGpuVoltArray[14];

     CustomizeCpuDvfsTable eristaCpuDvfsTable;
     CustomizeCpuDvfsTable marikoCpuDvfsTable;
     CustomizeCpuDvfsTable marikoCpuDvfsTableSLT;

     CustomizeGpuDvfsTable eristaGpuDvfsTable;
     CustomizeGpuDvfsTable eristaGpuDvfsTableSLT;
     CustomizeGpuDvfsTable eristaGpuDvfsTableHigh;


     CustomizeGpuDvfsTable marikoGpuDvfsTable;
     CustomizeGpuDvfsTable marikoGpuDvfsTableSLT;
     CustomizeGpuDvfsTable marikoGpuDvfsTableHiOPT;
     //EristaMtcTable* eristaMtcTable;
     //MarikoMtcTable* marikoMtcTable;
     CustomizeGpuDvfsTable eristaGpuDvfsTableUv3UnsafeFreqs;
     CustomizeGpuDvfsTable marikoGpuDvfsTableUv3UnsafeFreqs;
     CustomizeCpuDvfsTable marikoCpuDvfsTableUnsafeFreqs;
     CustomizeCpuDvfsTable eristaCpuDvfsTableUnsafeFreqs;

 } CustomizeTable;
 //static_assert(sizeof(CustomizeTable) == sizeof(u8) * 4 + sizeof(u32) * 10 + sizeof(CustomizeCpuDvfsTable) * 5 + sizeof(void*) * 2);
 //static_assert(sizeof(CustomizeTable) == 7000);

 extern volatile CustomizeTable C;

 //extern volatile EristaMtcTable EristaMtcTablePlaceholder;
 //extern volatile MarikoMtcTable MarikoMtcTablePlaceholder;

 }
