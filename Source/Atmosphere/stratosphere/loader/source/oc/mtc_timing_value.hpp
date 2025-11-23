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

    /* Primary timings. */
    const std::array<u32,  8> tRCD_values  =  {18, 17, 16, 15, 14, 13, 12, 11};
    const std::array<u32,  8> tRP_values   =  {18, 17, 16, 15, 14, 13, 12, 11};
    const std::array<u32, 10> tRAS_values  =  {42, 36, 34, 32, 30, 28, 26, 24, 22, 20};
    const std::array<double,    8>  tRRD_values   = {10.0, 7.5, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0};
    const std::array<u32,      11>  tRFC_values   = {140, 130, 120, 110, 100, 90, 80, 70, 60, 50, 40};
    const std::array<u32,      10>  tWTR_values   = {10, 9, 8, 7, 6, 5, 4, 3, 3, 1};
    const std::array<u32,       6>  tREFpb_values = {3900, 5850, 7800, 11700, 15600, 99999};

    const u32 BL = 16;

    /* Set to 4 read and 2 write for 1866bl. */
    /* For 2131bl: 8 read and 4 write. */
    const u32 rl_offset = 8;
    const u32 wl_offset = 4;

    const u32 RL = 28 + rl_offset;
    const u32 WL = 14 + wl_offset;

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

    /* TOOD: Fix erista */
    namespace pcv::erista {
        const double tCK_avg = 1000'000.0 / C.eristaEmcMaxClock;

        /* Primary timings. */
        const u32 tRCD  = tRCD_values[C.t1_tRCD];
        const u32 tRPpb = tRP_values[C.t2_tRP];
        const u32 tRAS  = tRAS_values[C.t3_tRAS];

        /* Secondary timings. */
        const double tRRD = tRRD_values[C.t4_tRRD];
        const u32 tRFCpb = tRFC_values[C.t5_tRFC];
        const u32 tWTR   = tWTR_values[C.t7_tWTR];
        const u32 tREFpb = tREFpb_values[C.t8_tREFI];

        /* Four-bank ACTIVATE Window */
        const u32 tFAW = (u32) (tRRD * 4.0);

        /* Latency stuff. */
        const int tR2W = (int)((3.5 / tCK_avg) + 32 + (BL / 2) - 14 - 6 + tWPRE + 12 - (C.t6_tRTW * 3));
        const int tW2R = (int)((tWTR / tCK_avg) + 18 - (BL / 2));
        const int tRW2PDEN = (int)((tDQSS_max / tCK_avg) + 46 + (tDQS2DQ_max / tCK_avg) + 6);

        /* Refresh Cycle time. (All Banks) */
        const u32 tRFCab = tRFCpb * 2;

        /* ACTIVATE-to-ACTIVATE command period. (same bank) */
        const u32 tRC = tRAS + tRPpb;

        /* SELF REFRESH exit to next valid command delay. */
        const double tXSR = (double) (tRFCab + 7.5);

        /* u32ernal READ to PRECHARGE command delay. */
        const int pdex2mrr = (tCK_avg * 3.0) + tRCD_values[C.t1_tRCD] + 1;

        /* Row Precharge Time. (all banks) */
        const u32 tRPab = tRPpb + 3;
    }

    namespace pcv::mariko {
        const double tCK_avg = 1000'000.0 / C.marikoEmcMaxClock;

        const u32 tRCD  = tRCD_values[C.t1_tRCD];
        const u32 tRPpb = tRP_values[C.t2_tRP];
        const u32 tRAS  = tRAS_values[C.t3_tRAS];
        const double tRRD = tRRD_values[C.t4_tRRD];
        const u32 tRFCpb = tRFC_values[C.t5_tRFC];
        const u32 tWTR   = tWTR_values[C.t7_tWTR];

        const u32 tRC = tRAS + tRPpb;
        const u32 tRFCab = tRFCpb * 2;
        const double tXSR = (double) (tRFCab + 7.5);
        const u32 tFAW = (u32) (tRRD * 4.0);
        const double tRPab = tRPpb + 3;

        const double tR2P = 7.5; /* Tighten up? */
        const u32 tR2W = CEIL(RL + (tDQSCK_max / tCK_avg) + (BL / 2) - WL + tWPRE + FLOOR(tRPST)); /* TODO */
        const u32 tRTM = RL + 9 + (tDQSCK_max / tCK_avg) + FLOOR(tRPST) + CEIL(7.5 / tCK_avg);
        const u32 tRATM = tRTM + CEIL(tR2P / tCK_avg) - (BL / 2);

        /* Note: Dividing WL is probably incorect but it works out by pure chance :) */
        const u32 tW2P = WL + (BL / 2) + 1 + CEIL(WL / tCK_avg); /* Tighten? */
        const u32 tW2R = WL + (BL / 2) + 1 + CEIL(tWTR / tCK_avg);
        const u32 tWTM = WL + (BL / 2) + 1 + CEIL(7.5 / tCK_avg);
        const u32 tWATM = tWTM + CEIL(WL / tCK_avg);

        const double tMMRI = tRCD + (tCK_avg * 3);
        const double tPDEX2MRR = tMMRI + 10;
        const u32 tWTPDEN = tW2P + 1 + CEIL(tDQSS_max / tCK_avg) + CEIL(tDQS2DQ_max / tCK_avg) + 6.0;

        inline u32 obdly = 0x10000002 + wl_offset;
        const u32 wdv = 0xE + wl_offset;
        const u32 wsv = 0xC + wl_offset;
        const u32 wev = 0xA + wl_offset;
    }

}
