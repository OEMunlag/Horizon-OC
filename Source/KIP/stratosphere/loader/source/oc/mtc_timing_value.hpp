/*
 * Copyright (c) 2023 hanai3Bi
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
    const std::array<double,  8> tRCD_values  =  {18, 17, 16, 15, 14, 13, 12, 11};
    const std::array<double,  8> tRP_values   =  {18, 17, 16, 15, 14, 13, 12, 11};
    const std::array<double, 10> tRAS_values  =  {42, 36, 34, 32, 30, 28, 26, 24, 22, 20};

    /* Secondary timings. */
    const std::array<double, 8>  tRRD_values   = {10.0, 7.5, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0};
    const std::array<double, 6>  tRFC_values   = {140, 120, 100, 80, 60, 40};
    const std::array<u32,    10>  tRTW_values   = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; /* Is this even correct? */
    const std::array<double, 10>  tWTR_values   = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    const std::array<u32,    7>  tREFpb_values = {488, 732, 488 * 2, 488 * 3, 488 * 4, 488 * 6, 488 * 8}; /* TODO: Figure out if it's actually 8 and if this is even right. */

    const u32 BL = 16;
    const u32 RL = 28 + C.mem_burst_latency;
    const u32 WL = 14 + C.mem_burst_latency;

    /* Refresh Cycle time. (All Banks) */
    const u32 tRFCab = (u32)(tRFC_values[C.t5_tRFC] * 1.5);

    /* Precharge to Precharge Delay. (Cycles) */
    /* Don't touch! */
    const u32 tPPD = 4;

    /* Four-bank ACTIVATE Window */
    const u32 tFAW = 30;

    /* DQS output access time from CK_t/CK_c. */
    const double tDQSCK_max = 3.5;
    const double tWPRE = 2.0;

    /* tCK Read postamble. */
    const double tRPST = 0.5;

    namespace pcv::erista {
        /* tCK_avg may have to be improved... */
        const double tCK_avg =  1000'000.0 / C.eristaEmcMaxClock;

        /* Primary timings. */
        const double tRCD  = MAX(tRCD_values[C.t1_tRCD], 4.0 * tCK_avg);
        const double tRPpb = MAX(tRP_values[C.t2_tRP],   4.0 * tCK_avg);
        const double tRAS  = MAX(tRAS_values[C.t3_tRAS], 3.0 * tCK_avg);

        /* Secondary timings. */
        const double tRRD   = MAX(tRRD_values[C.t4_tRRD], 4.0 * tCK_avg);
        const double tRFCpb = tRFC_values[C.t5_tRFC];
        const u32    tRTW   = tRTW_values[C.t6_tRTW];
        const double tWTR   = MAX(tWTR_values[C.t7_tWTR], 8.0 * tCK_avg);
        const u32    tREFpb = tREFpb_values[C.t8_tREFI];

        /* Latency stuff. */
        const u32 R2W = CEIL(RL + CEIL(tDQSCK_max/tCK_avg) + (BL/2) - WL + tWPRE + FLOOR(tRPST)) + 6;
        const u32 W2R = WL + (BL/2) + 1 + tWTR - 4;
        const u32 WTP = WL + (BL/2) + 1 + tWTR - 6;

        /* Refresh stuff. */
        const u32 numOfRows = 65536;
        const u32 REFRESH = MIN((u32)65472, u32(std::ceil((double(tREFpb) * C.eristaEmcMaxClock / numOfRows * 1.048 / 2 - 64))) / 4 * 4);
        const u32 REFBW = MIN((u32)65536, REFRESH+64);

        /* Do not touch stuff. */
        /* ACTIVATE-to-ACTIVATE command period. (same bank) */
        const double tRC = tRAS + tRPpb;

        /* Minimum Self-Refresh Time. (Entry to Exit) */
        const double tSR = MAX(15.0, 3.0 * tCK_avg);
        /* SELF REFRESH exit to next valid command delay. */
        const double tXSR = MAX(tRFCab + 7.5, 2.0 * tCK_avg);

        /* Exit power down to next valid command delay. */
        const double tXP = MAX(7.5, 5.0 * tCK_avg);

        /* Internal READ to PRECHARGE command delay. */
        const double tRTP = MAX(7.5, 8.0 * tCK_avg);

        /* Row Precharge Time. (all banks) */
        const double tRPab = MAX(21.0, 4.0 * tCK_avg);
    }

    namespace pcv::mariko {
        /* tCK_avg may have to be improved... */
        const double tCK_avg = 1000'000.0 / C.marikoEmcMaxClock;

        /* Primary timings. */
        const double tRCD  = MAX(tRCD_values[C.t1_tRCD], 4.0 * tCK_avg);
        const double tRPpb = MAX(tRP_values[C.t2_tRP],   4.0 * tCK_avg);
        const double tRAS  = MAX(tRAS_values[C.t3_tRAS], 3.0 * tCK_avg);

        /* Secondary timings. */
        const double tRRD   = MAX(tRRD_values[C.t4_tRRD], 4.0 * tCK_avg);
        const double tRFCpb = tRFC_values[C.t5_tRFC];
        const u32    tRTW   = tRTW_values[C.t6_tRTW];
        const double tWTR   = MAX(tWTR_values[C.t7_tWTR], 8.0 * tCK_avg);
        const u32    tREFpb = tREFpb_values[C.t8_tREFI];

        /* Latency stuff. */
        const u32 R2W = CEIL(RL + CEIL(tDQSCK_max/tCK_avg) + (BL/2) - WL + tWPRE + FLOOR(tRPST)) + 6;
        const u32 W2R = WL + (BL/2) + 1 + tWTR - 6;
        const u32 WTP = WL + (BL/2) + 1 + tWTR - 8;

        /* Refresh stuff. */
        const u32 numOfRows = 65536;
        const u32 REFRESH = MIN((u32)65472, u32(std::ceil((double(tREFpb) * C.eristaEmcMaxClock / numOfRows * 1.048 / 2 - 64))) / 4 * 4);
        const u32 REFBW = MIN((u32)65536, REFRESH+64);

        /* Do not touch stuff. */
        /* ACTIVATE-to-ACTIVATE command period. (same bank) */
        const double tRC = tRAS + tRPpb;

        /* Minimum Self-Refresh Time. (Entry to Exit) */
        const double tSR = MAX(15.0, 3.0 * tCK_avg);
        /* SELF REFRESH exit to next valid command delay. */
        const double tXSR = MAX(tRFCab + 7.5, 2.0 * tCK_avg);

        /* Exit power down to next valid command delay. */
        const double tXP = MAX(7.5, 5.0 * tCK_avg);

        /* Internal READ to PRECHARGE command delay. */
        const double tRTP = MAX(7.5, 8.0 * tCK_avg);

        /* Row Precharge Time. (all banks) */
        const double tRPab = MAX(21.0, 4.0 * tCK_avg);

    }

}
