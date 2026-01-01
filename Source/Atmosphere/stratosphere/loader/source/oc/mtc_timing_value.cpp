/*
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

#include "mtc_timing_value.hpp"

namespace ams::ldr::oc::pcv::mariko {
    double tCK_avg = 0;
    u32 tRCD  = tRCD_values[C.t1_tRCD];
    u32 tRPpb = tRP_values[C.t2_tRP];
    u32 tRAS  = tRAS_values[C.t3_tRAS];
    double tRRD = tRRD_values[C.t4_tRRD];
    u32 tRFCpb = tRFC_values[C.t5_tRFC];
    u32 tWTR   = 10 - tWTR_values[C.t7_tWTR];
    u32 tRC = tRAS + tRPpb;
    u32 tRFCab = tRFCpb * 2;
    double tXSR = static_cast<double>(tRFCab + 7.5);
    u32 tFAW = static_cast<u32>(tRRD * 4.0);
    double tRPab = tRPpb + 3;
    u32 tR2P = 0;
    u32 tR2W = 0;
    u32 tRTM = 0;
    u32 tRATM = 0;
    u32 rdv = 0;
    u32 quse = 0;
    u32 einput = 0;
    u32 einput_duration = 0;
    u32 ibdly = 0;
    u32 obdly = 0;
    u32 quse_width = 0;
    u32 rext = 0;
    u32 qrst = 0;
    u32 qsafe = 0;
    u32 qpop = 0;
    u32 tW2P = 0;
    u32 tWTPDEN = 0;
    u32 tW2R = 0;
    u32 tWTM = 0;
    u32 tWATM = 0;
    u32 wdv = 0;
    u32 wsv = 0;
    u32 wev = 0;
    u32 pdex2rw = 0;
    u32 cke2pden = 0;
    u32 tCKE = 0;
    double tMMRI = 0;
    double pdex2mrr = 0;
}
