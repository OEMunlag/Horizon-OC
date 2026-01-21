/*
 * Copyright (c) Lightos_
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

#include "../oc_common.hpp"
#include "../mtc_timing_value.hpp"
#include "timing_tables.hpp"

namespace ams::ldr::oc::pcv::mariko {

    u32 GetRext() {
        if (auto r = FindRext()) {
            return r->correct;
        }
        return 0x1A;
    }

    void CalculateTWTPDEN() {
        tWTPDEN = tW2P + 1 + CEIL(tDQSS_max / tCK_avg) + CEIL(tDQS2DQ_max / tCK_avg) + 6;
        if (C.marikoEmcMaxClock >= 2'233'000 && C.marikoEmcMaxClock < 2'533'000) tWTPDEN++;
        if (C.marikoEmcMaxClock >= 2'433'000 && C.marikoEmcMaxClock < 2'800'000) tWTPDEN--;
    }

    void CalculateTR2W() {
        tR2W = CEIL(RL_DBI + (tDQSCK_max / tCK_avg) + (BL / 2) - WL + tWPRE + FLOOR(tRPST) + 9.0) - (C.t6_tRTW * 3);

        if (auto patch = FindTR2WPatch()) {
            tR2W += patch->adjust;
        }
    }

    void CalculatePdex2rw() {
        double freq_mhz = C.marikoEmcMaxClock / 1000.0;

        double pdex_local = (0.011 * freq_mhz) - 1.443;
        pdex2rw = static_cast<u32>(ROUND(pdex_local));

        if (pdex2rw < 22) pdex2rw = 22;
        if (pdex2rw > 33) pdex2rw = 33;

        if (auto patch = FindPdex2rwPatch()) {
            pdex2rw += patch->adjust;
        }
    }

    void CalculateCke2pden() {
        cke2pden = (static_cast<double>((C.marikoEmcMaxClock / 1000.0) * 0.00875) - 0.65);

        if (auto patch = FindCke2pdenPatch()) {
            cke2pden += patch->adjust;
        }
    }

    void CalculateMrw2() {
        static const u8 rlMapDBI[8] = {
            6, 12, 16, 22, 28, 32, 36, 40
        };

        static const u8 wlMapSetA[8] = {
            4, 6, 8, 10, 12, 14, 16, 18
        };

        u32 rlIndex = 0;
        u32 wlIndex = 0;

        for (u32 i = 0; i < std::size(rlMapDBI); ++i) {
            if (rlMapDBI[i] == RL_DBI) {
                rlIndex = i;
                break;
            }
        }

        for (u32 i = 0; i < std::size(wlMapSetA); ++i) {
            if (wlMapSetA[i] == WL) {
                wlIndex = i;
                break;
            }
        }

        mrw2 = static_cast<u8>(((rlIndex & 0x7) | ((wlIndex & 0x7) << 3) | ((0 & 0x1) << 6)));
    }

    void CalculateTimings() {
        rext = GetRext();
        CalculateTWTPDEN();
        CalculateTR2W();
        CalculatePdex2rw();
        CalculateCke2pden();
        CalculateMrw2();
    }

}
