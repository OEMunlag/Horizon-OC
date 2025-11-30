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

#include "../mtc_timing_value.hpp"
#include "timing_tables.hpp"

namespace ams::ldr::oc::pcv::mariko {

    const MiscTimings g_misc_table[] = {
        {1'866'000, 1, 0x20, 0x9,  0},
        {2'133'000, 1, 0x24, 0xA,  0},
        {2'166'000, 1,    0,   0,  0},
        {2'233'000, 0, 0x25,   0,  0},
        {2'300'000, 0, 0x26, 0xB,  0},
        {2'333'000, 0, 0x27,   0,  0},
        {2'366'000, 1, 0x26, 0xA,  0},
        {2'433'000, 0, 0x27,   0,  0},
        {2'466'000, 0, 0x2A,   0,  0},
        {2'500'000, 0, 0x28, 0xB,  0},
        {2'533'000, 0, 0x29,   0, -2},
        {2'566'000, 1,    0,   0,  0},
        {2'633'000, 0, 0x2A,   0,  0},
        {2'700'000, 0, 0x2B, 0xC,  0},
        {2'733'000, 0, 0x2C,   0,  0},
        {2'766'000, 1, 0x2B, 0xB,  0},
        {2'833'000, 0, 0x2C,   0, -2},
        {2'866'000, 0,    0,   0, -2},
        {2'900'000, 0,    0,   0, -2},
        {2'933'000, 0, 0x2E, 0xC,  0},
        {2'966'000, 1,    0,   0,  0},
        {3'033'000, 0, 0x2F,   0,  0},
        {3'100'000, 0,    0,   0, -2},
        {3'133'000, 1, 0x31, 0xD,  0},
    };

    const u32 g_misc_table_size = sizeof(g_misc_table) / sizeof(g_misc_table[0]);

    const ReplacePatch g_einput_patches[] = {
        {2'133'000, 0x16}, {2'166'000, 0x17}, {2'200'000, 0x17},
        {2'233'000, 0x16}, {2'266'000, 0x16}, {2'300'000, 0x15},
        {2'333'000, 0x14}, {2'366'000, 0x16}, {2'400'000, 0x16},
        {2'433'000, 0x15}, {2'466'000, 0x15}, {2'500'000, 0x14},
        {2'533'000, 0x13}, {2'566'000, 0x14}, {2'600'000, 0x14},
        {2'633'000, 0x13}, {2'666'000, 0x13}, {2'700'000, 0x12},
        {2'733'000, 0x11}, {2'766'000, 0x13}, {2'800'000, 0x13},
        {2'833'000, 0x12}, {2'866'000, 0x12}, {2'900'000, 0x12},
        {2'933'000, 0x10}, {2'966'000, 0x11}, {3'000'000, 0x11},
        {3'033'000, 0x10}, {3'066'000, 0x10}, {3'100'000, 0x10},
        {3'133'000, 0x0F},
    };

    const u32 g_einput_patches_size = sizeof(g_einput_patches) / sizeof(g_einput_patches[0]);

    const ReplacePatch *FindEinput() {
        for (u32 i = 0; i < g_einput_patches_size; i++)
            if (g_einput_patches[i].freq == C.marikoEmcMaxClock)
                return &g_einput_patches[i];
        return nullptr;
    }

    const ReplacePatch g_rext_table[] = {
        {2'133'000, 0x1A}, {2'166'000, 0x19}, {2'200'000, 0x19},
        {2'233'000, 0x19}, {2'266'000, 0x1A}, {2'300'000, 0x1B},
        {2'333'000, 0x1B}, {2'366'000, 0x1B}, {2'400'000, 0x1B},
        {2'433'000, 0x1B}, {2'466'000, 0x1B}, {2'500'000, 0x1A},
        {2'533'000, 0x1C}, {2'566'000, 0x1B}, {2'600'000, 0x17},
        {2'633'000, 0x1B}, {2'666'000, 0x1B}, {2'700'000, 0x1C},
        {2'733'000, 0x1C}, {2'766'000, 0x1D}, {2'800'000, 0x1D},
        {2'833'000, 0x1D}, {2'866'000, 0x1D}, {2'900'000, 0x1D},
        {2'933'000, 0x1C}, {2'966'000, 0x1D}, {3'000'000, 0x1D},
        {3'033'000, 0x1D}, {3'066'000, 0x1D}, {3'100'000, 0x1D},
        {3'133'000, 0x1D},
    };

    const u32 g_rext_table_size = sizeof(g_rext_table) / sizeof(g_rext_table[0]);

    const ReplacePatch *FindRext() {
        for (u32 i = 0; i < g_rext_table_size; i++)
            if (g_rext_table[i].freq == C.marikoEmcMaxClock)
                return &g_rext_table[i];
        return nullptr;
    }

    const FreqTW2R g_tw2r_table[] = {
        {2'300'000, 0x2B, 0xFFFFFFFF,  0},
        {2'400'000,    0,       0x2B,  0},
        {2'500'000, 0x2C, 0xFFFFFFFF, -1},
        {2'566'000, 0x2D, 0xFFFFFFFF, -1},
        {2'700'000, 0x2E, 0xFFFFFFFF, -1},
        {2'800'000, 0x2F, 0xFFFFFFFF, -1},
        {2'900'000, 0x30, 0xFFFFFFFF, -1},
        {3'000'000, 0x31, 0xFFFFFFFF, -1},
        {3'100'000, 0x32, 0xFFFFFFFF, -1},
        {0xFFFFFFFF,   0,       0x33,  0},
    };

    const u32 g_tw2r_table_size = sizeof(g_tw2r_table) / sizeof(g_tw2r_table[0]);

    const FreqTW2R *FindTW2R() {
        for (u32 i = 0; i < g_tw2r_table_size; i++)
            if (C.marikoEmcMaxClock <= g_tw2r_table[i].max_freq)
                return &g_tw2r_table[i];
        return nullptr;
    }

    const AdjustPatch g_ibdly_patches[] = {
        {2'133'000, -2},
        {2'166'000, -1},
        {2'200'000, -1},
        {2'233'000, -1},
        {2'266'000, -1},
        {2'300'000, -2},
        {2'333'000, -2},
        {2'500'000, -1},
        {2'533'000, -2},
        {2'566'000, -1},
        {2'633'000, -1},
        {2'666'000, -1},
        {2'700'000, -2},
        {2'733'000, -2},
        {2'833'000, -9},
        {2'933'000, -1},
        {3'100'000, -9},
        {3'133'000, -9},
    };

    const u32 g_ibdly_table_size = sizeof(g_ibdly_patches) / sizeof(g_ibdly_patches[0]);

    const AdjustPatch *FindIbdlyPatch() {
        for (u32 i = 0; i < g_ibdly_table_size; i++)
            if (g_ibdly_patches[i].freq == C.marikoEmcMaxClock)
                return &g_ibdly_patches[i];
        return nullptr;
    }

    const AdjustPatch g_tr2w_patches[] = {
        {2'500'000,  1},
        {2'533'000,  1},
        {2'566'000,  1},
        {2'866'000, -1},
        {3'100'000,  1},
        {3'133'000,  1},
    };

    const u32 g_tr2w_table_size = sizeof(g_tr2w_patches) / sizeof(g_tr2w_patches[0]);

    const AdjustPatch *FindTR2WPatch() {
        for (u32 i = 0; i < g_tr2w_table_size; i++)
            if (g_tr2w_patches[i].freq == C.marikoEmcMaxClock)
                return &g_tr2w_patches[i];
        return nullptr;
    }

    const AdjustPatch g_quse_patches[] = {
        {2'133'000, -1},
        {2'300'000, -1},
        {2'333'000, -1},
        {2'366'000, +1},
        {2'400'000, +1},
        {2'433'000, +1},
        {2'466'000, +1},
        {2'500'000, -1},
        {2'533'000, -1},
        {2'700'000, -1},
        {2'733'000, -1},
        {2'766'000, +1},
        {2'800'000, +1},
        {2'833'000, +1},
        {2'866'000, +1},
        {2'900'000, +1},
        {2'933'000, -1},
    };

    const u32 g_quse_table_size = sizeof(g_quse_patches) / sizeof(g_quse_patches[0]);

    const AdjustPatch *FindQusePatch() {
        for (u32 i = 0; i < g_quse_table_size; i++)
            if (g_quse_patches[i].freq == C.marikoEmcMaxClock)
                return &g_quse_patches[i];
        return nullptr;
    }

    const AdjustPatch g_qrst_patches[] = {
        {2'166'000,  1},
        {2'200'000,  1},
        {2'233'000,  1},
        {2'266'000,  1},
        {2'366'000,  2},
        {2'400'000,  2},
        {2'433'000,  1},
        {2'466'000,  2},
        {2'500'000,  1},
        {2'533'000, -1},
        {2'700'000, -1},
        {2'733'000, -1},
        {2'766'000,  1},
        {2'800'000,  4},
        {2'833'000,  1},
        {2'866'000,  1},
        {2'900'000,  1},
        {2'933'000, -1},
        {2'966'000,  1},
        {3'000'000,  1},
        {3'100'000,  1},
    };

    const u32 g_qrst_table_size = sizeof(g_qrst_patches) / sizeof(g_qrst_patches[0]);

    const AdjustPatch *FindQrstPatch() {
        for (u32 i = 0; i < g_qrst_table_size; i++)
            if (g_qrst_patches[i].freq == C.marikoEmcMaxClock)
                return &g_qrst_patches[i];
        return nullptr;
    }

    const AdjustPatch g_qsafe_patches[] = {
        {2'166'000,  1},
        {2'200'000,  1},
        {2'500'000, -1},
        {2'533'000, -1},
        {2'666'000, -1},
        {2'700'000, -1},
        {2'733'000, -1},
        {2'800'000, -1},
        {2'833'000, -1},
        {2'866'000, -1},
        {2'900'000, -1},
        {2'933'000, -2},
        {2'966'000, -1},
        {3'000'000, -1},
        {3'033'000, -1},
        {3'066'000, -2},
        {3'100'000, -2},
    };

    const u32 g_qsafe_table_size = sizeof(g_qsafe_patches) / sizeof(g_qsafe_patches[0]);

    const AdjustPatch *FindQsafePatch() {
        for (u32 i = 0; i < g_qsafe_table_size; i++)
            if (g_qsafe_patches[i].freq == C.marikoEmcMaxClock)
                return &g_qsafe_patches[i];
        return nullptr;
    }

    const AdjustPatch g_pdex2rw_patches[] = {
        {2'166'000,  1},
        {2'300'000,  1},
        {2'333'000,  1},
        {2'433'000,  1},
        {2'533'000,  0},
        {2'633'000, -1},
        {2'666'000, -1},
        {2'733'000, -1},
        {2'766'000, -1},
        {2'800'000, -1},
        {2'833'000, -1},
        {2'933'000, -1},
        {3'066'000,  1},
    };

    const u32 g_pdex2rw_table_size = sizeof(g_pdex2rw_patches) / sizeof(g_pdex2rw_patches[0]);

    const AdjustPatch *FindPdex2rwPatch() {
        for (u32 i = 0; i < g_pdex2rw_table_size; i++)
            if (g_pdex2rw_patches[i].freq == C.marikoEmcMaxClock)
                return &g_pdex2rw_patches[i];
        return nullptr;
    }

    const AdjustPatch g_cke2pden_patches[] = {
        {2'133'000, 1},
        {2'166'000, 1},
        {2'266'000, 1},
        {2'300'000, 1},
        {2'366'000, 1},
        {2'400'000, 1},
        {2'500'000, 1},
        {2'633'000, 1},
        {2'733'000, 1},
        {2'833'000, 1},
        {2'866'000, 1},
        {2'966'000, 1},
        {3'066'000, 1},
        {3'100'000, 1},
    };

    const u32 g_cke2pden_table_size = sizeof(g_cke2pden_patches) / sizeof(g_cke2pden_patches[0]);

    const AdjustPatch *FindCke2pdenPatch() {
        for (u32 i = 0; i < g_cke2pden_table_size; i++)
            if (g_cke2pden_patches[i].freq == C.marikoEmcMaxClock)
                return &g_cke2pden_patches[i];
        return nullptr;
    }

}
