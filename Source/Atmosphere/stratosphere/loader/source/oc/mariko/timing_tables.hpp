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

#pragma once
#include "../mtc_timing_value.hpp"

namespace ams::ldr::hoc::pcv::mariko {

    struct ReplacePatch {
        u32 freq;
        u32 correct;
    };

    extern const ReplacePatch  g_rext_table[];
    extern const u32           g_rext_table_size;
    const        ReplacePatch *FindRext();

    struct AdjustPatch {
        u32 freq;
        s32 adjust;
    };

    extern const AdjustPatch g_tr2w_patches[];
    extern const u32         g_tr2w_table_size;
    const AdjustPatch       *FindTR2WPatch();

    extern const AdjustPatch g_pdex2rw_patches[];
    extern const u32         g_pdex2rw_table_size;
    const AdjustPatch       *FindPdex2rwPatch();

    extern const AdjustPatch g_cke2pden_patches[];
    extern const u32         g_cke2pden_table_size;
    const AdjustPatch       *FindCke2pdenPatch();

}
