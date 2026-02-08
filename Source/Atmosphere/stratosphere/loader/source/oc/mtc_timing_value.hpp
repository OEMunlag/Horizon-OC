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

namespace ams::ldr::hoc {
    #define MAX(A, B)   std::max(A, B)
    #define MIN(A, B)   std::min(A, B)
    #define CEIL(A)     std::ceil(A)
    #define FLOOR(A)    std::floor(A)
    #define ROUND(A)    std::lround(A)

    #define PACK_U32(high, low) ((static_cast<u32>(high) << 16) | (static_cast<u32>(low) & 0xFFFF))
    #define PACK_U32_NIBBLE_HIGH_BYTE_LOW(high, low) ((static_cast<u32>(high & 0xF) << 28) | (static_cast<u32>(low) & 0xFF))

    /* Primary timings. */
    const std::array<u32,  8> tRCD_values  =  { 18, 17, 16, 15, 14, 13, 12, 11 };
    const std::array<u32,  8> tRP_values   =  { 18, 17, 16, 15, 14, 13, 12, 11 };
    const std::array<u32, 10> tRAS_values  =  { 42, 36, 34, 32, 30, 28, 26, 24, 22, 20 };
    const std::array<double,    7>  tRRD_values   = { /*10.0,*/ 7.5, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0 }; /* 10.0 is used for <2133mhz; do we care? 8gb uses 7.5 tRRD on >=1331. */
    const std::array<u32,      11>  tRFC_values   = { 140, 130, 120, 110, 100, 90, 80, 70, 60, 50, 40 };
    const std::array<u32,      10>  tWTR_values   = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    const std::array<u32,       6>  tREFpb_values = { 3900, 5850, 7800, 11700, 15600, 99999 };

    /* Burst latency, not to be confused with base latency (tWRL). */
    const u32 BL = 16;

    /* Base latency for read and write (tWRL). */
    const u32 RL = C.mem_burst_read_latency - 4; /* (This is a lazy fix for now) */
    const u32 WL = C.mem_burst_write_latency;

    /* Switch uses RL_DBI, todo: get rid of non DBI_RL. */
    const u32 RL_DBI = RL + 4;

    /* Precharge to Precharge Delay. (tCK) */
    const u32 tPPD = 4;

    /* DQS output access time from CK_t/CK_c. */
    const double tDQSCK_max = 3.5;
    /* Write preamble. (tCK) */
    const u32 tWPRE = 2;

    /* Read postamble. (tCK) */
    const double tRPST = 0.5;

    /* Minimum Self-Refresh Time. (Entry to Exit) */
    const double tSR = 15.0;

    /* Exit power down to next valid command delay. */
    const double tXP = 7.5;

    /* Write command to first DQS transition (max) (tCK) */
    const double tDQSS_max = 1.25;

    /* DQ-to-DQS offset(max) (ns) */
    const double tDQS2DQ_max = 0.8;

    /* Write recovery time. */
    const u32 tWR = 18;

    /* TOOD: Fix erista */
    namespace pcv::erista {
        const double tCK_avg = 1000'000.0 / C.eristaEmcMaxClock;

        const u32 tRCD  = tRCD_values[C.t1_tRCD];
        const u32 tRPpb = tRP_values[C.t2_tRP];
        const u32 tRAS  = tRAS_values[C.t3_tRAS];
        const double tRRD = tRRD_values[C.t4_tRRD];
        const u32 tRFCpb = tRFC_values[C.t5_tRFC];
        const u32 tWTR   = 10 - tWTR_values[C.t7_tWTR];

        const u32 tRC = tRAS + tRPpb;
        const u32 tRFCab = tRFCpb * 2;
        const double tXSR = (double) (tRFCab + 7.5);
        const u32 tFAW = static_cast<u32>(tRRD * 4.0);
        const double tRPab = tRPpb + 3;

        const u32 tR2P = 12;

        const u32 tW2P = (CEIL(WL * 1.7303) * 2) - 5;
        const u32 tW2R = CEIL(MAX(WL + (0.010322547033278747 * (C.eristaEmcMaxClock / 1000.0)), (WL * -0.2067922202979121) + FLOOR(((RL_DBI * -0.1331159971685554) + WL) * 3.654131957826108)) - (tWTR / tCK_avg));
        
        const u32 wdv = WL;
        const u32 wsv = WL - 2;
        const u32 wev = 0xA + (WL - 14);

        const double freq_mhz = C.eristaEmcMaxClock / 1000.0;

        const u32 quse_width       = CEIL(((3.7165006256863955 - freq_mhz) + (-0.002446584377651142 * freq_mhz)) - FLOOR(freq_mhz / -0.9952024303111688));
        const u32 quse             = CEIL(MIN(RL_DBI + (2.991255208275918 - (quse_width + (-0.00511180626826906 * freq_mhz))), freq_mhz * 0.021333773138874437));
        const u32 ibdly            = 0x10000000 + FLOOR(MAX(RL_DBI - 1.9999956603408224, quse - 5.9999987787411175) + (-0.0011929079761504341 * freq_mhz));
        const u32 obdlyHigh = 3 / FLOOR(MIN(static_cast<double>(2), tCK_avg * (WL - 7)));
        const u32 obdlyLow = WL - MIN(static_cast<double>(WL), 12 - (CEIL(-0.0003991 * freq_mhz) * 2));
        const u32 obdly = PACK_U32_NIBBLE_HIGH_BYTE_LOW(obdlyHigh, obdlyLow);
        const u32 tCKE = CEIL(1.0795 * CEIL(0.0074472 * (C.eristaEmcMaxClock / 1000.0)));

        const double tMMRI = tRCD + (tCK_avg * 3);
        const double pdex2mrr = tMMRI + 10;
        const u32 tWTPDEN = tW2P + 1 + CEIL(tDQSS_max / tCK_avg) + CEIL(tDQS2DQ_max / tCK_avg) + 6;
        const u32 tR2W = CEIL(RL_DBI + (tDQSCK_max / tCK_avg) + (BL / 2) - WL + tWPRE + FLOOR(tRPST) + 9.0) - (C.t6_tRTW * 3);

        const double pdex_local = (0.011 * freq_mhz) - 1.443;
        const u32 pdex2rw = static_cast<u32>(ROUND(pdex_local)) < 22 ? 22 : (static_cast<u32>(ROUND(pdex_local)) > 33 ? 33 : static_cast<u32>(ROUND(pdex_local)));

        const double cke2pden = (static_cast<double>((C.eristaEmcMaxClock / 1000.0) * 0.00875) - 0.65);
    }

    namespace pcv::mariko {
        const double tCK_avg = 1000'000.0 / C.marikoEmcMaxClock;
        const double ramFreqMhz = C.marikoEmcMaxClock / 1000.0;

        const u32 tRCD    = tRCD_values[C.t1_tRCD];
        const u32 tRPpb   = tRP_values[C.t2_tRP];
        const u32 tRAS    = tRAS_values[C.t3_tRAS];
        const double tRRD = tRRD_values[C.t4_tRRD];
        const u32 tRFCpb  = tRFC_values[C.t5_tRFC];
        const u32 tWTR    = 10 - tWTR_values[C.t7_tWTR];

        const u32 tRC      = tRAS + tRPpb;
        const u32 tRFCab   = tRFCpb * 2;
        const double tXSR  = static_cast<double>(tRFCab + 7.5);
        const u32 tFAW     = static_cast<u32>(tRRD * 4.0);
        const double tRPab = tRPpb + 3;

        const u32 tR2P   = 12 + ((RL_DBI - 32) / 2);
        inline u32 tR2W;
        const u32 tRTM   = RL + 9 + (tDQSCK_max / tCK_avg) + FLOOR(tRPST) + CEIL(10 / tCK_avg); // Fix?
        const u32 tRATM  = tRTM + CEIL(10 / tCK_avg) - 12; // Fix?
        inline u32 rext;

        const u32 rdv              = FLOOR(17.02046755653219 + (RL_DBI + (ramFreqMhz * 0.00510056573299173)));
        const u32 qpop             = rdv - 14;
        const u32 quse_width       = CEIL(((3.7165006256863955 - ramFreqMhz) + (-0.002446584377651142 * ramFreqMhz)) - FLOOR(ramFreqMhz / -0.9952024303111688));
        const u32 quse             = CEIL(MIN(RL_DBI + (2.991255208275918 - (quse_width + (-0.00511180626826906 * ramFreqMhz))), ramFreqMhz * 0.021333773138874437));
        const u32 einput_duration  = CEIL(quse_width + (ramFreqMhz * 0.01) + 4);
        const u32 einput           = 5 + qpop - einput_duration;
        const u32 ibdly            = 0x10000000 + FLOOR(MAX(RL_DBI - 1.9999956603408224, quse - 5.9999987787411175) + (-0.0011929079761504341 * ramFreqMhz));
        const u32 qrst_duration    = FLOOR((ramFreqMhz * 0.001477125119082522) + 4.272302254983803);
        const u32 qrstLow          = MAX(static_cast<s32>(einput - qrst_duration - 2), static_cast<s32>(0));
        const u32 qrst             = PACK_U32(qrst_duration, qrstLow);
        const u32 qsafe            = (einput_duration + 3) + MAX(MIN(qrstLow * rdv, qrst_duration + qrst_duration), einput);

        const u32 tW2P  = (CEIL(WL * 1.7303) * 2) - 5;
        inline u32 tWTPDEN;
        const u32 tW2R  = CEIL(MAX(WL + (0.010322547033278747 * ramFreqMhz), (WL * -0.2067922202979121) + FLOOR(((RL_DBI * -0.1331159971685554) + WL) * 3.654131957826108)) - (tWTR / tCK_avg));
        const u32 tWTM  = WL + (BL / 2) + 1 + CEIL(7.5 / tCK_avg);
        const u32 tWATM = tWTM + CEIL(tWR / tCK_avg);

        const u32 wdv = WL;
        const u32 wsv = WL - 2;
        const u32 wev = 0xA + (WL - 14);

        const u32 obdlyHigh = 3 / FLOOR(MIN(static_cast<double>(2), tCK_avg * (WL - 7)));
        const u32 obdlyLow = WL - MIN(static_cast<double>(WL), 12 - (CEIL(-0.0003991 * ramFreqMhz) * 2));

        const u32 obdly = PACK_U32_NIBBLE_HIGH_BYTE_LOW(obdlyHigh, obdlyLow);

        inline u32 pdex2rw;
        inline u32 cke2pden;

        const u32 tCKE = CEIL(1.0795 * CEIL(0.0074472 * ramFreqMhz));

        const double tMMRI    = tRCD + (tCK_avg * 3);
        const double pdex2mrr = tMMRI + 10; /* Do this properly? */

        inline u8 mrw2;
    }

}

