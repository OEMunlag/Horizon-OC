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

    /* TODO: This function is quite uggly, refactor! */
    void CalculateMiscTimings() {
        einput_duration = 0x1C;

        for (u32 i = 0; i < g_misc_table_size; i++) {
            const auto& e = g_misc_table[i];
            if (C.marikoEmcMaxClock >= e.min_freq) {
                if (e.einput) einput_duration = e.einput;
            }
        }

        rext = GetRext();
    }

    void CalculateIbdly() {
        /* Ibdly is so inconsistent, I am using the most common value and then checking with a lookup table. */
        ibdly = 0x1000001D + C.mem_burst_read_latency;

        if (auto patch = FindIbdlyPatch()) {
            ibdly += patch->adjust;
        }
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

    void CalculateQsafe() {
        qsafe = ROUND((C.marikoEmcMaxClock / 1000.0) / 138.0 + 37.4) + C.mem_burst_read_latency;
        if (auto patch = FindQsafePatch()) {
            qsafe += patch->adjust;
        }
    }

    void CalculateQpop() {
        qpop = FLOOR(((C.marikoEmcMaxClock / 1000.0) - 2133 + 167) / 200.0) + 0x2D + C.mem_burst_read_latency;

        if (C.marikoEmcMaxClock >= 3'133'000) qpop++;
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

    void CalculateTimings() {
        CalculateMiscTimings();
        CalculateIbdly();
        CalculateTWTPDEN();
        CalculateTR2W();
        CalculateQsafe();
        CalculateQpop();
        CalculatePdex2rw();
        CalculateCke2pden();
    }

}
