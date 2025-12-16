/*
 * Copyright (c) 2023 hanai3Bi
 *
 * Copyright (c) 2025 Lightos_
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

#include "oc_common.hpp"

namespace ams::ldr::oc {
    #define MAX(A, B)   std::max(A, B)
    #define MIN(A, B)   std::min(A, B)
    #define CEIL(A)     std::ceil(A)
    #define FLOOR(A)    std::floor(A)
    #define ROUND(A)    std::lround(A)

    #define GET_LOW_BYTES(x)        ((u16)((x) & 0xFFFF))
    #define SET_LOW_BYTES(x, val)   (((x) & 0xFFFF0000) | ((u32)(val) & 0xFFFF))
    #define INCREMENT_LOW_BYTES_BY(x, n)  SET_LOW_BYTES((x), (GET_LOW_BYTES(x) + (n)))
    #define GET_HIGH_BYTES(x)       ((u16)(((x) >> 16) & 0xFFFF))
    #define SET_HIGH_BYTES(x, val)  (((x) & 0x0000FFFF) | (((u32)(val) & 0xFFFF) << 16))
    #define INCREMENT_HIGH_BYTES_BY(x, n) SET_HIGH_BYTES((x), (GET_HIGH_BYTES(x) + (n)))

    /* Primary timings. */
    const std::array<u32,  8> tRCD_values  =  {18, 17, 16, 15, 14, 13, 12, 11};
    const std::array<u32,  8> tRP_values   =  {18, 17, 16, 15, 14, 13, 12, 11};
    const std::array<u32, 10> tRAS_values  =  {42, 36, 34, 32, 30, 28, 26, 24, 22, 20};
    const std::array<double,    7>  tRRD_values   = {/*10.0,*/ 7.5, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0}; /* 10.0 is used for <2133mhz; do we care? */
    const std::array<u32,      11>  tRFC_values   = {140, 130, 120, 110, 100, 90, 80, 70, 60, 50, 40};
    const std::array<u32,      10>  tWTR_values   = {10, 9, 8, 7, 6, 5, 4, 3, 3, 1};
    const std::array<u32,       6>  tREFpb_values = {3900, 5850, 7800, 11700, 15600, 99999};

    const u32 BL = 16;

    const u32 RL = 28 + C.mem_burst_read_latency;
    const u32 WL = 14 + C.mem_burst_write_latency;

    const u32 RL_DBI = RL + 4;

    /* Precharge to Precharge Delay. (Cycles) */
    const u32 tPPD = 4;

    /* DQS output access time from CK_t/CK_c. */
    const double tDQSCK_max = 3.5;
    const u32 tWPRE = 2;

    /* tCK Read postamble. */
    const double tRPST = 0.5;

    /* Minimum Self-Refresh Time. (Entry to Exit) */
    const double tSR = 15.0;

    /* Exit power down to next valid command delay. */
    const double tXP = 7.5;

    const double tDQSS_max = 1.25;
    const double tDQS2DQ_max = 0.8;

    const u32 tWR = 18;

    /* TOOD: Fix erista */
    namespace pcv::erista {
     //   const double tCK_avg = 1000'000.0 / C.eristaEmcMaxClock;
     //
     //   /* Primary timings. */
     //   const u32 tRCD  = tRCD_values[C.t1_tRCD];
     //   const u32 tRPpb = tRP_values[C.t2_tRP];
     //   const u32 tRAS  = tRAS_values[C.t3_tRAS];
     //
     //   /* Secondary timings. */
     //   const double tRRD = tRRD_values[C.t4_tRRD];
     //   const u32 tRFCpb = tRFC_values[C.t5_tRFC];
     //   const u32 tWTR   = tWTR_values[C.t7_tWTR];
     //   const u32 tREFpb = tREFpb_values[C.t8_tREFI];
     //
     //   /* Four-bank ACTIVATE Window */
     //   const u32 tFAW = (u32) (tRRD * 4.0);
     //
     //   /* Latency stuff. */
     //   const int tR2W = (int)((3.5 / tCK_avg) + 32 + (BL / 2) - 14 - 6 + tWPRE + 12 - (C.t6_tRTW * 3));
     //   const int tW2R = (int)((tWTR / tCK_avg) + 18 - (BL / 2));
     //   const int tRW2PDEN = (int)((tDQSS_max / tCK_avg) + 46 + (tDQS2DQ_max / tCK_avg) + 6);
     //
     //   /* Refresh Cycle time. (All Banks) */
     //   const u32 tRFCab = tRFCpb * 2;
     //
     //   /* ACTIVATE-to-ACTIVATE command period. (same bank) */
     //   const u32 tRC = tRAS + tRPpb;
     //
     //   /* SELF REFRESH exit to next valid command delay. */
     //   const double tXSR = (double) (tRFCab + 7.5);
     //
     //   /* u32ernal READ to PRECHARGE command delay. */
     //   const int pdex2mrr = (tCK_avg * 3.0) + tRCD_values[C.t1_tRCD] + 1;
     //
     //   /* Row Precharge Time. (all banks) */
     //   const u32 tRPab = tRPpb + 3;
    }

    namespace pcv::mariko {
        const double tCK_avg = 1000'000.0 / C.marikoEmcMaxClock;

        const u32 tRCD  = tRCD_values[C.t1_tRCD];
        const u32 tRPpb = tRP_values[C.t2_tRP];
        const u32 tRAS  = tRAS_values[C.t3_tRAS];
        const double tRRD = tRRD_values[C.t4_tRRD];
        const u32 tRFCpb = tRFC_values[C.t5_tRFC];
        const u32 tWTR   = MAX(static_cast<u32>(0), 10 - tWTR_values[C.t7_tWTR]);

        const u32 tRC = tRAS + tRPpb;
        const u32 tRFCab = tRFCpb * 2;
        const double tXSR = (double) (tRFCab + 7.5);
        const u32 tFAW = static_cast<u32>(tRRD * 4.0);
        const double tRPab = tRPpb + 3;

        const u32 tR2P = 12 + (C.mem_burst_read_latency / 2);
        inline u32 tR2W;
        const u32 tRTM = RL + 9 + (tDQSCK_max / tCK_avg) + FLOOR(tRPST) + CEIL(10 / tCK_avg); // Fix?
        const u32 tRATM = tRTM + CEIL(10 / tCK_avg) - 12; // Fix?
        inline u32 rdv;
        const u32 quse = FLOOR((-0.0048159 * (C.marikoEmcMaxClock / 1000.0)) + RL_DBI) + (FLOOR((C.marikoEmcMaxClock / 1000.0) * 0.0050997) * 1.5134);
        const u32 einput = quse - ((C.marikoEmcMaxClock / 1000.0) * 0.01);
        inline u32 einput_duration;
        inline u32 ibdly;
        inline u32 obdly;
        inline u32 quse_width;
        inline u32 rext;
        inline u32 qrst;
        inline u32 qsafe;
        inline u32 qpop;

        const u32 tW2P = (CEIL(WL * 1.7303) * 2) - 5;
        inline u32 tWTPDEN;
        const u32 tW2R = CEIL(MAX(WL + (0.010322547033278747 * (C.marikoEmcMaxClock / 1000.0)), (WL * -0.2067922202979121) + FLOOR(((RL_DBI * -0.1331159971685554) + WL) * 3.654131957826108)) - (tWTR / tCK_avg));
        const u32 tWTM = WL + (BL / 2) + 1 + CEIL(7.5 / tCK_avg);
        const u32 tWATM = tWTM + CEIL(tWR / tCK_avg);

        const u32 wdv = WL;
        const u32 wsv = WL - 2;
        const u32 wev = 0xA + C.mem_burst_write_latency;

        inline u32 pdex2rw;
        inline u32 cke2pden;

        const double tMMRI = tRCD + (tCK_avg * 3);
        const double pdex2mrr = tMMRI + 10; /* Do this properly? */
    }

}
