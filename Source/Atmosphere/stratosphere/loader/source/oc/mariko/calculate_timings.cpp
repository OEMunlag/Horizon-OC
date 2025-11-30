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

    s32 FixEinput(s32 val) {
        if (auto patch = FindEinput()) {
            return patch->correct;
        }
        return val;
    }

    u32 GetRext() {
        if (auto r = FindRext()) {
            return r->correct;
        }
        return 0x1A;
    }

    /* TODO: This function is quite uggly, refactor! */
    void CalculateMiscTimings() {
        tW2P            = 0x2d;
        rdv             = 0x39       + C.mem_burst_read_latency;
        obdly           = 0x10000002 + C.mem_burst_write_latency;
        einput_duration = 0x1C;
        quse_width      = 0x8;

        for (u32 i = 0; i < g_misc_table_size; i++) {
            const auto& e = g_misc_table[i];
            if (C.marikoEmcMaxClock >= e.min_freq) {
                rdv += e.rdv_inc;
                if (e.einput) einput_duration = e.einput;
                if (e.quse_width) quse_width = e.quse_width;
                obdly += e.obdly_delta;
            }
        }

        if (WL >= 16) tW2P += 6;
        if (WL >= 18) tW2P += 8;

        s32 einput_calc = quse_width - (0.010182 * (C.marikoEmcMaxClock / 1000.0) - 0.0879) + 0.5;
        einput = FixEinput(einput_calc);
        rext   = GetRext();
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
        tR2W = CEIL(RL_DBI + (tDQSCK_max / tCK_avg) + (BL / 2) - WL + tWPRE + FLOOR(tRPST) + 9.0);

        if (auto patch = FindTR2WPatch()) {
            tR2W += patch->adjust;
        }
    }

    void CalculateTW2R() {
        tW2R = WL + (BL / 2) - 6 + CEIL(tWTR / tCK_avg);

        const FreqTW2R* t = FindTW2R();
        if (!t) return;

        tW2R += t->adjust;

        if (t->min_val) {
            tW2R = MAX(tW2R, t->min_val);
        }

        if (t->max_val) {
            tW2R = MIN(tW2R, t->max_val);
        }
    }

    void CalculateTW2RDerivedWriteTimings() {
        tWTM = WL + (BL / 2) + 1 + CEIL(tW2R / tCK_avg);
        tWATM = tWTM + CEIL(tWR / tCK_avg);
    }

    void CalculateQuse() {
        quse = ROUND(0.002266 * (C.marikoEmcMaxClock / 1000.0) + 31.88) + C.mem_burst_read_latency;

        if (auto patch = FindQusePatch()) {
            quse += patch->adjust;
        }
    }

    void CalculateQrst() {
        qrst = 0x00070000;
        u32 qrst_calc = ROUND(22.1 - (C.marikoEmcMaxClock / 1000000.0) * 8.0) + C.mem_burst_read_latency;
        u32 qrst_low = MAX(static_cast<u32>(0), qrst_calc);

        if (C.marikoEmcMaxClock >= 2'533'000) {
            qrst = INCREMENT_HIGH_BYTES_BY(qrst, 1);
        } else if (C.marikoEmcMaxClock == 2'800'000) {
            qrst = SET_HIGH_BYTES(qrst, 6);
        }

        qrst = SET_LOW_BYTES(qrst, qrst_low);

        if (auto patch = FindQrstPatch()) {
            qrst = INCREMENT_LOW_BYTES_BY(qrst, patch->adjust);
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

        if      (C.marikoEmcMaxClock >= 3'100'000) qpop = 0x2B; /* TODO: Check if this is actually true. */
        else if (C.marikoEmcMaxClock >= 3'133'000) ++qpop;
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
        CalculateTW2R();
        CalculateTW2RDerivedWriteTimings();
        CalculateQuse();
        CalculateQrst();
        CalculateQsafe();
        CalculateQpop();
        CalculatePdex2rw();
        CalculateCke2pden();
    }

}
