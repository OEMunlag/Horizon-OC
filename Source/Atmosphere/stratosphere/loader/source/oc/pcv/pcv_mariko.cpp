/*
 * Copyright (C) Switch-OC-Suite
 *
 * Copyright (c) 2023 hanai3Bi
 *
 * Copyright (c) Souldbminer and Horizon OC Contributors
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

#include "pcv.hpp"
#include "../mtc_timing_value.hpp"
#include "../mariko/calculate_timings.hpp"

namespace ams::ldr::oc::pcv::mariko {

    Result GpuVmin(u32 *ptr) {
        if (!C.marikoGpuVmin)
            R_SKIP();
        PATCH_OFFSET(ptr, (int)C.marikoGpuVmin);
        R_SUCCEED();
    }

    Result GpuVmax(u32 *ptr) {
        if (!C.marikoGpuVmax)
            R_SKIP();
        PATCH_OFFSET(ptr, (int)C.marikoGpuVmax);
        R_SUCCEED();
    }

    Result CpuFreqVdd(u32 *ptr) {
        dvfs_rail *entry = reinterpret_cast<dvfs_rail *>(reinterpret_cast<u8 *>(ptr) - offsetof(dvfs_rail, freq));

        R_UNLESS(entry->id == 1, ldr::ResultInvalidCpuFreqVddEntry());
        R_UNLESS(entry->min_mv == 250'000, ldr::ResultInvalidCpuFreqVddEntry());
        R_UNLESS(entry->step_mv == 5000, ldr::ResultInvalidCpuFreqVddEntry());
        R_UNLESS(entry->max_mv == 1525'000, ldr::ResultInvalidCpuFreqVddEntry());
        if (C.marikoCpuUV)
        {
            PATCH_OFFSET(ptr, GetDvfsTableLastEntry(C.marikoCpuDvfsTableSLT)->freq);
        } else {
            PATCH_OFFSET(ptr, GetDvfsTableLastEntry(C.marikoCpuDvfsTable)->freq);
        }
        R_SUCCEED();
    }

    Result CpuVoltRange(u32 *ptr) {
        u32 min_volt_got = *(ptr - 1);
        for (const auto &mv : CpuMinVolts) {
            if (min_volt_got != mv)
                continue;

            if (!C.marikoCpuMaxVolt)
                R_SKIP();

            PATCH_OFFSET(ptr, C.marikoCpuMaxVolt);
            // Patch vmin for slt
            if (C.marikoCpuUV) {
                if (*(ptr - 5) == 620) {
                    PATCH_OFFSET((ptr - 5), C.marikoCpuLowVmin); // hf vmin
                }
                if (*(ptr - 1) == 620) {
                    PATCH_OFFSET((ptr - 1), C.marikoCpuHighVmin); // lf vmin
                }
            }
            R_SUCCEED();
        }
        R_THROW(ldr::ResultInvalidCpuMinVolt());
    }

    Result CpuVoltDfll(u32 *ptr) {
        cvb_cpu_dfll_data *entry = reinterpret_cast<cvb_cpu_dfll_data *>(ptr);

        R_UNLESS(entry->tune0_low == 0x0000FFCF, ldr::ResultInvalidCpuVoltDfllEntry());
        R_UNLESS(entry->tune0_high == 0x00000000, ldr::ResultInvalidCpuVoltDfllEntry());
        R_UNLESS(entry->tune1_low == 0x012207FF, ldr::ResultInvalidCpuVoltDfllEntry());
        R_UNLESS(entry->tune1_high == 0x03FFF7FF, ldr::ResultInvalidCpuVoltDfllEntry());
        switch (C.marikoCpuUV) {
        case 0:
            break;
        case 1:
            PATCH_OFFSET(&(entry->tune0_low), 0x0000FF90); // process_id 0 // EOS UV1
            PATCH_OFFSET(&(entry->tune0_high), 0x0000FFFF);
            PATCH_OFFSET(&(entry->tune1_low), 0x021107FF);
            PATCH_OFFSET(&(entry->tune1_high), 0x00000000);
            break;
        case 2:
            PATCH_OFFSET(&(entry->tune0_low), 0x0000FF92); /// EOS Uv2
            PATCH_OFFSET(&(entry->tune0_high), 0x0000FFFF);
            PATCH_OFFSET(&(entry->tune1_low), 0x021107FF);
            PATCH_OFFSET(&(entry->tune1_high), 0x00000000);
            break;
        case 3:
            PATCH_OFFSET(&(entry->tune0_low), 0x0000FF9A); // EOS UV3
            PATCH_OFFSET(&(entry->tune0_high), 0x0000FFFF);
            PATCH_OFFSET(&(entry->tune1_low), 0x021107FF);
            PATCH_OFFSET(&(entry->tune1_high), 0x00000000);
            break;
        case 4:
            PATCH_OFFSET(&(entry->tune0_low), 0x0000FFA2); // EOS Uv4
            PATCH_OFFSET(&(entry->tune0_high), 0x0000FFFF);
            PATCH_OFFSET(&(entry->tune1_low), 0x021107FF);
            PATCH_OFFSET(&(entry->tune1_high), 0x00000000);
            break;
        case 5:
            PATCH_OFFSET(&(entry->tune0_low), 0x0000FFFF); // EOS UV5
            PATCH_OFFSET(&(entry->tune0_high), 0x0000FFFF);
            PATCH_OFFSET(&(entry->tune1_low), 0x021107FF);
            PATCH_OFFSET(&(entry->tune1_high), 0x022217FF);
            break;
        case 6:
            PATCH_OFFSET(&(entry->tune0_low), 0x0000FFFF); // EOS UV6
            PATCH_OFFSET(&(entry->tune0_high), 0x0000FFFF);
            PATCH_OFFSET(&(entry->tune1_low), 0x021107FF);
            PATCH_OFFSET(&(entry->tune1_high), 0x024417FF);
            break;
        case 7:
            PATCH_OFFSET(&(entry->tune0_low), 0x0000FFFF); // EOS UV6
            PATCH_OFFSET(&(entry->tune0_high), 0x0000FFFF);
            PATCH_OFFSET(&(entry->tune1_low), 0x021107FF);
            PATCH_OFFSET(&(entry->tune1_high), 0x026617FF);
            break;
        case 8:
            PATCH_OFFSET(&(entry->tune0_low), 0x0000FFFF); // EOS UV6
            PATCH_OFFSET(&(entry->tune0_high), 0x0000FFFF);
            PATCH_OFFSET(&(entry->tune1_low), 0x021107FF);
            PATCH_OFFSET(&(entry->tune1_high), 0x028817FF);
            break;
        case 9:
            PATCH_OFFSET(&(entry->tune0_low), 0x0000FFFF); // EOS UV6
            PATCH_OFFSET(&(entry->tune0_high), 0x0000FFFF);
            PATCH_OFFSET(&(entry->tune1_low), 0x021107FF);
            PATCH_OFFSET(&(entry->tune1_high), 0x028817FF);
            break;
        case 10:
            PATCH_OFFSET(&(entry->tune0_low), 0x0000FFFF); // EOS UV6
            PATCH_OFFSET(&(entry->tune0_high), 0x0000FFFF);
            PATCH_OFFSET(&(entry->tune1_low), 0x021107FF);
            PATCH_OFFSET(&(entry->tune1_high), 0x02AA17FF);
            break;
        case 11:
            PATCH_OFFSET(&(entry->tune0_low), 0x0000FFFF); // EOS UV6
            PATCH_OFFSET(&(entry->tune0_high), 0x0000FFFF);
            PATCH_OFFSET(&(entry->tune1_low), 0x021107FF);
            PATCH_OFFSET(&(entry->tune1_high), 0x02CC17FF);
            break;
        case 12:
            PATCH_OFFSET(&(entry->tune0_low), 0x0000FFFF); // EOS UV6
            PATCH_OFFSET(&(entry->tune0_high), 0x0000FFFF);
            PATCH_OFFSET(&(entry->tune1_low), 0x021107FF);
            PATCH_OFFSET(&(entry->tune1_high), 0x02FF17FF);
            break;
        default:
            break;
        }
        R_SUCCEED();
    }

    Result GpuFreqMaxAsm(u32 *ptr32) {
        // Check if both two instructions match the pattern
        u32 ins1 = *ptr32, ins2 = *(ptr32 + 1);
        if (!(asm_compare_no_rd(ins1, asm_pattern[0]) && asm_compare_no_rd(ins2, asm_pattern[1])))
            R_THROW(ldr::ResultInvalidGpuFreqMaxPattern());

        // Both instructions should operate on the same register
        u8 rd = asm_get_rd(ins1);
        if (rd != asm_get_rd(ins2))
            R_THROW(ldr::ResultInvalidGpuFreqMaxPattern());

        u32 max_clock;
        switch (C.marikoGpuUV) {
        case 0:
            max_clock = GetDvfsTableLastEntry(C.marikoGpuDvfsTable)->freq;
            break;
        case 1:
            max_clock = GetDvfsTableLastEntry(C.marikoGpuDvfsTableSLT)->freq;
            break;
        case 2:
        case 3:
            max_clock = GetDvfsTableLastEntry(C.marikoGpuDvfsTableHiOPT)->freq;
            break;
        default:
            max_clock = GetDvfsTableLastEntry(C.marikoGpuDvfsTable)->freq;
            break;
        }
        u32 asm_patch[2] = {
            asm_set_rd(asm_set_imm16(asm_pattern[0], max_clock), rd),
            asm_set_rd(asm_set_imm16(asm_pattern[1], max_clock >> 16), rd)
        };

        PATCH_OFFSET(ptr32, asm_patch[0]);
        PATCH_OFFSET(ptr32 + 1, asm_patch[1]);

        R_SUCCEED();
    }

    Result GpuFreqPllMax(u32 *ptr) {
        clk_pll_param *entry = reinterpret_cast<clk_pll_param *>(ptr);

        // All zero except for freq
        for (size_t i = 1; i < sizeof(clk_pll_param) / sizeof(u32); i++) {
            R_UNLESS(*(ptr + i) == 0, ldr::ResultInvalidGpuPllEntry());
        }

        // Double the max clk simply
        u32 max_clk = entry->freq * 2;
        entry->freq = max_clk;
        R_SUCCEED();
    }

    Result GpuFreqPllLimit(u32 *ptr) {
        u32 prev_freq = *(ptr - 1);

        if (prev_freq != 128000 && prev_freq != 1300000 && prev_freq != 76800) {
            R_THROW(ldr::ResultInvalidGpuPllEntry());
        }

        if (C.marikoGpuFullUnlock) {
            /* Removes all limits - dangerous. */
            *ptr = 3600000;
        }

        R_SUCCEED();
    }

    void MemMtcTableAutoAdjustBaseLatency(MarikoMtcTable *table) {
        #define WRITE_PARAM_ALL_REG(TABLE, PARAM, VALUE) \
            TABLE->burst_regs.PARAM = VALUE;             \
            TABLE->shadow_regs_ca_train.PARAM   = VALUE; \
            TABLE->shadow_regs_rdwr_train.PARAM = VALUE;

        #define GET_CYCLE_CEIL(PARAM) u32(CEIL(double(PARAM) / tCK_avg))

        if (C.hpMode) {
            WRITE_PARAM_ALL_REG(table, emc_cfg, 0x13200000);
        } else {
            WRITE_PARAM_ALL_REG(table, emc_cfg, 0xF3200000);
        }

        u32 refresh_raw = 0xFFFF;
        if (C.t8_tREFI != 6) {
            refresh_raw = std::floor(tREFpb_values[C.t8_tREFI] / tCK_avg) - 0x40;
            refresh_raw = MIN(refresh_raw, static_cast<u32>(0xFFFF));
        }

        u32 trefbw = refresh_raw + 0x40;
        trefbw = MIN(trefbw, static_cast<u32>(0x3FFF));

        CalculateTimings();

        WRITE_PARAM_ALL_REG(table, emc_rd_rcd, GET_CYCLE_CEIL(tRCD));
        WRITE_PARAM_ALL_REG(table, emc_wr_rcd, GET_CYCLE_CEIL(tRCD));
        WRITE_PARAM_ALL_REG(table, emc_rc, MIN(GET_CYCLE_CEIL(tRC), static_cast<u32>(0xB8)));
        WRITE_PARAM_ALL_REG(table, emc_ras, MIN(GET_CYCLE_CEIL(tRAS), static_cast<u32>(0x7F)));
        WRITE_PARAM_ALL_REG(table, emc_rrd, GET_CYCLE_CEIL(tRRD));
        WRITE_PARAM_ALL_REG(table, emc_rfcpb, GET_CYCLE_CEIL(tRFCpb));
        WRITE_PARAM_ALL_REG(table, emc_rfc, GET_CYCLE_CEIL(tRFCab));
        WRITE_PARAM_ALL_REG(table, emc_rp, GET_CYCLE_CEIL(tRPpb));
        WRITE_PARAM_ALL_REG(table, emc_txsr, MIN(GET_CYCLE_CEIL(tXSR), static_cast<u32>(0x3fe)));
        WRITE_PARAM_ALL_REG(table, emc_txsrdll, MIN(GET_CYCLE_CEIL(tXSR), static_cast<u32>(0x3fe)));
        WRITE_PARAM_ALL_REG(table, emc_tfaw, GET_CYCLE_CEIL(tFAW));
        WRITE_PARAM_ALL_REG(table, emc_trpab, MIN(GET_CYCLE_CEIL(tRPab), static_cast<u32>(0x3F)));
        WRITE_PARAM_ALL_REG(table, emc_tckesr, GET_CYCLE_CEIL(tSR));
        WRITE_PARAM_ALL_REG(table, emc_tcke, GET_CYCLE_CEIL(tXP) + 1);
        WRITE_PARAM_ALL_REG(table, emc_tpd, GET_CYCLE_CEIL(tXP));
        WRITE_PARAM_ALL_REG(table, emc_tclkstop, GET_CYCLE_CEIL(tXP) + 8);
        WRITE_PARAM_ALL_REG(table, emc_r2p, tR2P);
        WRITE_PARAM_ALL_REG(table, emc_r2w, tR2W);
        WRITE_PARAM_ALL_REG(table, emc_trtm, tRTM);
        WRITE_PARAM_ALL_REG(table, emc_tratm, tRATM);
        WRITE_PARAM_ALL_REG(table, emc_w2p, tW2P);
        WRITE_PARAM_ALL_REG(table, emc_w2r, tW2R);
        WRITE_PARAM_ALL_REG(table, emc_twtm, tWTM);
        WRITE_PARAM_ALL_REG(table, emc_twatm, tWATM);
        WRITE_PARAM_ALL_REG(table, emc_rext, rext);
        WRITE_PARAM_ALL_REG(table, emc_wext, (C.marikoEmcMaxClock >= 2533000) ? 0x19 : 0x16);
        WRITE_PARAM_ALL_REG(table, emc_refresh, refresh_raw);
        WRITE_PARAM_ALL_REG(table, emc_pre_refresh_req_cnt, refresh_raw / 4);
        WRITE_PARAM_ALL_REG(table, emc_trefbw, trefbw);
        const u32 dyn_self_ref_control = (((u32)(7605.0 / tCK_avg)) + 260U) | (table->burst_regs.emc_dyn_self_ref_control & 0xffff0000U);
        WRITE_PARAM_ALL_REG(table, emc_dyn_self_ref_control, dyn_self_ref_control);
        WRITE_PARAM_ALL_REG(table, emc_pdex2wr, pdex2rw);
        WRITE_PARAM_ALL_REG(table, emc_pdex2rd, pdex2rw);
        WRITE_PARAM_ALL_REG(table, emc_pchg2pden, GET_CYCLE_CEIL(1.75));
        WRITE_PARAM_ALL_REG(table, emc_ar2pden, GET_CYCLE_CEIL(1.75));
        WRITE_PARAM_ALL_REG(table, emc_act2pden, GET_CYCLE_CEIL(14.0));
        WRITE_PARAM_ALL_REG(table, emc_cke2pden, cke2pden);
        WRITE_PARAM_ALL_REG(table, emc_pdex2mrr, GET_CYCLE_CEIL(pdex2mrr));
        WRITE_PARAM_ALL_REG(table, emc_rw2pden, tWTPDEN);
        WRITE_PARAM_ALL_REG(table, emc_einput, einput);
        WRITE_PARAM_ALL_REG(table, emc_einput_duration, einput_duration);
        WRITE_PARAM_ALL_REG(table, emc_obdly, obdly);
        WRITE_PARAM_ALL_REG(table, emc_ibdly, ibdly);
        WRITE_PARAM_ALL_REG(table, emc_wdv_mask, wdv);
        WRITE_PARAM_ALL_REG(table, emc_quse_width, quse_width);
        WRITE_PARAM_ALL_REG(table, emc_quse, quse);
        WRITE_PARAM_ALL_REG(table, emc_wdv, wdv);
        WRITE_PARAM_ALL_REG(table, emc_wsv, wsv);
        WRITE_PARAM_ALL_REG(table, emc_wev, wev);
        WRITE_PARAM_ALL_REG(table, emc_qrst, qrst);
        WRITE_PARAM_ALL_REG(table, emc_qsafe, qsafe);
        WRITE_PARAM_ALL_REG(table, emc_tr_qpop, qpop);
        WRITE_PARAM_ALL_REG(table, emc_rdv, rdv);
        WRITE_PARAM_ALL_REG(table, emc_qpop, qpop);
        WRITE_PARAM_ALL_REG(table, emc_tr_rdv_mask, rdv + 2);
        WRITE_PARAM_ALL_REG(table, emc_rdv_early, rdv - 2);
        WRITE_PARAM_ALL_REG(table, emc_rdv_early_mask, rdv);
        WRITE_PARAM_ALL_REG(table, emc_rdv_mask, rdv + 2);
        WRITE_PARAM_ALL_REG(table, emc_tr_rdv, rdv);

        constexpr u32 MC_ARB_DIV = 4;
        constexpr u32 MC_ARB_SFA = 2;

        table->burst_mc_regs.mc_emem_arb_cfg          = C.marikoEmcMaxClock           / (33.3 * 1000) / MC_ARB_DIV;
        table->burst_mc_regs.mc_emem_arb_timing_rcd   = (u32) (GET_CYCLE_CEIL(tRCD)   / MC_ARB_DIV) - 2;
        table->burst_mc_regs.mc_emem_arb_timing_rp    = (u32) (GET_CYCLE_CEIL(tRPpb)  / MC_ARB_DIV) - 1 + MC_ARB_SFA;
        table->burst_mc_regs.mc_emem_arb_timing_rc    = (u32) (GET_CYCLE_CEIL(tRC)    / MC_ARB_DIV) - 1;
        table->burst_mc_regs.mc_emem_arb_timing_ras   = (u32) (GET_CYCLE_CEIL(tRAS)   / MC_ARB_DIV) - 2;
        table->burst_mc_regs.mc_emem_arb_timing_faw   = (u32) (GET_CYCLE_CEIL(tFAW)   / MC_ARB_DIV) - 1;
        table->burst_mc_regs.mc_emem_arb_timing_rrd   = (u32) (GET_CYCLE_CEIL(tRRD)   / MC_ARB_DIV) - 1;
        table->burst_mc_regs.mc_emem_arb_timing_rfcpb = (u32) (GET_CYCLE_CEIL(tRFCpb) / MC_ARB_DIV);
        table->burst_mc_regs.mc_emem_arb_timing_rap2pre = (u32) (GET_CYCLE_CEIL(tR2P) / MC_ARB_DIV);
        table->burst_mc_regs.mc_emem_arb_timing_wap2pre = (u32) (tW2P / MC_ARB_DIV);
        table->burst_mc_regs.mc_emem_arb_timing_r2r = (u32) (table->burst_regs.emc_rext / 4) - 1 + MC_ARB_SFA;
        table->burst_mc_regs.mc_emem_arb_timing_r2w = (u32) (tR2W / MC_ARB_DIV) - 1 + MC_ARB_SFA;
        table->burst_mc_regs.mc_emem_arb_timing_w2r = (u32) (tW2R / MC_ARB_DIV) - 1 + MC_ARB_SFA;

        u32 da_turns = 0;
        da_turns |= u8(table->burst_mc_regs.mc_emem_arb_timing_r2w / 2) << 16;
        da_turns |= u8(table->burst_mc_regs.mc_emem_arb_timing_w2r / 2) << 24;
        table->burst_mc_regs.mc_emem_arb_da_turns = da_turns;

        u32 da_covers = 0;
        u8 r_cover = (table->burst_mc_regs.mc_emem_arb_timing_rap2pre + table->burst_mc_regs.mc_emem_arb_timing_rp + table->burst_mc_regs.mc_emem_arb_timing_rcd) / 2;
        u8 w_cover = (table->burst_mc_regs.mc_emem_arb_timing_wap2pre + table->burst_mc_regs.mc_emem_arb_timing_rp + table->burst_mc_regs.mc_emem_arb_timing_rcd) / 2;
        da_covers |= (u8) (table->burst_mc_regs.mc_emem_arb_timing_rc / 2);
        da_covers |= (r_cover << 8);
        da_covers |= (w_cover << 16);
        table->burst_mc_regs.mc_emem_arb_da_covers = da_covers;

        table->burst_mc_regs.mc_emem_arb_misc0 &= 0xFFE08000U;
        table->burst_mc_regs.mc_emem_arb_misc0 |= ((table->burst_mc_regs.mc_emem_arb_timing_rc + 1) & 0xFF); /* TODO, check this */

        table->la_scale_regs.mc_mll_mpcorer_ptsa_rate =         MIN((u32)((C.marikoEmcMaxClock / 1600000) * 0xd0U), (u32)0x115);
        table->la_scale_regs.mc_ftop_ptsa_rate =         MIN((u32)((C.marikoEmcMaxClock / 1600000) * 0x18U), (u32)0x1f);
        table->la_scale_regs.mc_ptsa_grant_decrement =         MIN((u32)((C.marikoEmcMaxClock / 1600000) * 0x1203U), (u32)0x17ff);

        u32 mc_latency_allowance = 0;
        if (C.marikoEmcMaxClock / 1000 != 0) {
            mc_latency_allowance = 204800 / (C.marikoEmcMaxClock / 1000);
        }

        const u32 mc_latency_allowance2 = mc_latency_allowance & 0xFF;
        const u32 mc_latency_allowance3 = (mc_latency_allowance & 0xFF) << 0x10;
        table->la_scale_regs.mc_latency_allowance_xusb_0 = (table->la_scale_regs.mc_latency_allowance_xusb_0 & 0xff00ffffU) | mc_latency_allowance3;
        table->la_scale_regs.mc_latency_allowance_sdmmc_0 = (table->la_scale_regs.mc_latency_allowance_sdmmc_0 & 0xff00ffffU) | mc_latency_allowance3;
        table->la_scale_regs.mc_latency_allowance_xusb_1 = (table->la_scale_regs.mc_latency_allowance_xusb_1 & 0xff00ffffU) | mc_latency_allowance3;
        table->la_scale_regs.mc_latency_allowance_tsec_0 = (table->la_scale_regs.mc_latency_allowance_tsec_0 & 0xff00ffffU) | mc_latency_allowance3;
        table->la_scale_regs.mc_latency_allowance_sdmmca_0 = (table->la_scale_regs.mc_latency_allowance_sdmmca_0 & 0xff00ffffU) | mc_latency_allowance3;
        table->la_scale_regs.mc_latency_allowance_sdmmcaa_0 = (table->la_scale_regs.mc_latency_allowance_sdmmcaa_0 & 0xff00ffffU) | mc_latency_allowance3;
        table->la_scale_regs.mc_latency_allowance_sdmmcab_0 = (table->la_scale_regs.mc_latency_allowance_sdmmcab_0 & 0xff00ffffU) | mc_latency_allowance3;
        table->la_scale_regs.mc_latency_allowance_ppcs_1 = (table->la_scale_regs.mc_latency_allowance_ppcs_1 & 0xff00ffffU) | mc_latency_allowance3;
        table->la_scale_regs.mc_latency_allowance_mpcore_0 = (table->la_scale_regs.mc_latency_allowance_mpcore_0 & 0xff00ffffU) | mc_latency_allowance3;
        table->la_scale_regs.mc_latency_allowance_avpc_0 = (table->la_scale_regs.mc_latency_allowance_avpc_0 & 0xff00ffffU) | mc_latency_allowance3;

        u32 mc_latency_allowance_hc_0 = 0;
        if (C.marikoEmcMaxClock / 1000 != 0) {
            mc_latency_allowance_hc_0 = 35200 / (C.marikoEmcMaxClock / 1000);
        }

        table->la_scale_regs.mc_latency_allowance_nvdec_0 = (table->la_scale_regs.mc_latency_allowance_nvdec_0 & 0xff00ffffU) | mc_latency_allowance3;
        table->la_scale_regs.mc_latency_allowance_hc_0 = (table->la_scale_regs.mc_latency_allowance_hc_0 & 0xffffff00U) | mc_latency_allowance_hc_0;

        table->la_scale_regs.mc_latency_allowance_isp2_1 = (table->la_scale_regs.mc_latency_allowance_isp2_1 & 0xff00ff00U) | mc_latency_allowance3 | mc_latency_allowance2;
        table->la_scale_regs.mc_latency_allowance_hc_1 = (table->la_scale_regs.mc_latency_allowance_hc_1 & 0xffffff00U) | mc_latency_allowance2;

        u32 mc_latency_allowance_gpu_0 = 0;
        if (C.marikoEmcMaxClock / 1000 != 0) {
            mc_latency_allowance_gpu_0 = 40000 / (C.marikoEmcMaxClock / 1000);
        }

        table->la_scale_regs.mc_latency_allowance_gpu_0 = ((mc_latency_allowance_gpu_0 | table->la_scale_regs.mc_latency_allowance_gpu_0) & 0xff00ff00U) | mc_latency_allowance3;

        u32 mc_latency_allowance_gpu2_0 = 0;
        if (C.marikoEmcMaxClock / 1000 != 0) {
        mc_latency_allowance_gpu2_0 = 40000 / (C.marikoEmcMaxClock / 1000);
        }

        table->la_scale_regs.mc_latency_allowance_gpu2_0 = ((mc_latency_allowance_gpu2_0 | table->la_scale_regs.mc_latency_allowance_gpu2_0) & 0xff00ff00U) | mc_latency_allowance3;

        u32 mc_latency_allowance_nvenc_0 = 0;
        if (C.marikoEmcMaxClock / 1000 != 0) {
            mc_latency_allowance_nvenc_0 = 38400 / (C.marikoEmcMaxClock / 1000);
        }

        table->la_scale_regs.mc_latency_allowance_nvenc_0 = ((mc_latency_allowance_nvenc_0 | table->la_scale_regs.mc_latency_allowance_nvenc_0) & 0xff00ff00U) | mc_latency_allowance3;

        u32 mc_latency_allowance_vic_0 = 0;
        if (C.marikoEmcMaxClock / 1000 != 0) {
            mc_latency_allowance_vic_0 = 0xb540 / (C.marikoEmcMaxClock / 1000);
        }

        table->la_scale_regs.mc_latency_allowance_vic_0 = ((mc_latency_allowance_vic_0 | table->la_scale_regs.mc_latency_allowance_vic_0) & 0xff00ff00U) | mc_latency_allowance3;
        table->la_scale_regs.mc_latency_allowance_vi2_0 = (table->la_scale_regs.mc_latency_allowance_vi2_0 & 0xffffff00U) | mc_latency_allowance2;

        // table->pllm_ss_ctrl1 = 0xb55fe01;
        // table->pllm_ss_ctrl2 = 0x10170b55;
        // table->pllmb_ss_ctrl1 = 0xb55fe01;
        // table->pllmb_ss_ctrl2 = 0x10170b55;

        table->dram_timings.t_rp = tRFCpb;
        table->dram_timings.t_rfc = tRFCab;
        table->dram_timings.rl = RL_DBI;
        table->emc_mrw2 = 0x8802003F;
        table->emc_cfg_2 = 0x11083D;
    }

    void MemMtcTableAutoAdjust(MarikoMtcTable *table) {
        /* Official Tegra X1 TRM, sign up for nvidia developer program (free) to download:
         *     https://developer.nvidia.com/embedded/dlc/tegra-x1-technical-reference-manual
         *     Section 18.11: MC Registers
         *
         * Retail Mariko: 200FBGA 16Gb DDP LPDDR4X SDRAM x 2
         * x16/Ch, 1Ch/die, Double-die, 2Ch, 1CS(rank), 8Gb density per die
         * 64Mb x 16DQ x 8banks x 2channels = 2048MB (x32DQ) per package
         *
         * Devkit Mariko: 200FBGA 32Gb DDP LPDDR4X SDRAM x 2
         * x16/Ch, 1Ch/die, Quad-die,   2Ch, 2CS(rank), 8Gb density per die
         * X1+ EMC can R/W to both ranks at the same time, resulting in doubled DQ
         * 64Mb x 32DQ x 8banks x 2channels = 4096MB (x64DQ) per package
         *
         * If you have access to LPDDR4(X) specs or datasheets (from manufacturers or Google),
         * you'd better calculate timings yourself rather than relying on following algorithm.
         */

                #define WRITE_PARAM_ALL_REG(TABLE, PARAM, VALUE) \
            TABLE->burst_regs.PARAM = VALUE;             \
            TABLE->shadow_regs_ca_train.PARAM   = VALUE; \
            TABLE->shadow_regs_rdwr_train.PARAM = VALUE;

        #define GET_CYCLE(PARAM) u32(CEIL(double(PARAM) / tCK_avg))

/* This condition is insane but it's done in eos. */
        /* Need to clean up at some point. */
      //  u32 rext;
      //  u32 wext;
      //  if (C.marikoEmcMaxClock < 3200001) {
      //      if (C.marikoEmcMaxClock < 2133001) {
      //          rext = 26;
      //          wext = 22;
      //      } else {
      //          rext = 28;
      //          wext = 22;
      //
      //          if (2400000 < C.marikoEmcMaxClock) {
      //              wext = 25;
      //          }
      //      }
      //  } else {
      //      rext = 30;
      //      wext = 25;
      //  }
      //
      //  u32 refresh_raw = 0xFFFF;
      //  u32 trefbw = 0;
      //
      //  if (C.t8_tREFI != 6) {
      //      refresh_raw = static_cast<u32>(std::floor(static_cast<double>(tREFpb_values[C.t8_tREFI]) / tCK_avg)) - 0x40;
      //      refresh_raw = MIN(refresh_raw, static_cast<u32>(0xFFFF));
      //  }
      //
      //  trefbw = refresh_raw + 0x40;
      //  trefbw = MIN(trefbw, static_cast<u32>(0x3FFF));
      //
      //  /* Primary timings. */
      //  WRITE_PARAM_ALL_REG(table, emc_rd_rcd, GET_CYCLE(tRCD));
      //  WRITE_PARAM_ALL_REG(table, emc_wr_rcd, GET_CYCLE(tRCD));
      //  WRITE_PARAM_ALL_REG(table, emc_ras,    GET_CYCLE(tRAS));
      //  WRITE_PARAM_ALL_REG(table, emc_rp,     GET_CYCLE(tRPpb));
      //
      //  /* Secondary timings. */
      //  WRITE_PARAM_ALL_REG(table, emc_rrd,     GET_CYCLE(tRRD));
      //  WRITE_PARAM_ALL_REG(table, emc_rfc,     GET_CYCLE(tRFCab));
      //  WRITE_PARAM_ALL_REG(table, emc_rfcpb,   GET_CYCLE(tRFCpb));
      //  WRITE_PARAM_ALL_REG(table, emc_r2w,     tR2W);
      //  WRITE_PARAM_ALL_REG(table, emc_w2r,     tW2R);
      WRITE_PARAM_ALL_REG(table, emc_r2p,   (u32)  0xC);
      //  WRITE_PARAM_ALL_REG(table, emc_w2p,     (u32) 0x2D);
      //
      //  WRITE_PARAM_ALL_REG(table, emc_rext, rext);
      //  WRITE_PARAM_ALL_REG(table, emc_wext, wext);
      //
      //  WRITE_PARAM_ALL_REG(table, emc_trpab,  GET_CYCLE(tRPab));
      //  WRITE_PARAM_ALL_REG(table, emc_tfaw,   GET_CYCLE(tFAW));
      //  WRITE_PARAM_ALL_REG(table, emc_rc,     GET_CYCLE(tRC));
      //
      //  WRITE_PARAM_ALL_REG(table, emc_tckesr,   GET_CYCLE(tSR));
      //  WRITE_PARAM_ALL_REG(table, emc_tcke,     GET_CYCLE(tXP) + 2);
      //  WRITE_PARAM_ALL_REG(table, emc_tpd,      GET_CYCLE(tXP));
      //  WRITE_PARAM_ALL_REG(table, emc_tclkstop, GET_CYCLE(tXP) + 8);
      //
      //  WRITE_PARAM_ALL_REG(table, emc_txsr,    MIN(GET_CYCLE(tXSR), (u32) 1022));
      //  WRITE_PARAM_ALL_REG(table, emc_txsrdll, MIN(GET_CYCLE(tXSR), (u32) 1022));
      //
      //  const u32 dyn_self_ref_control = (((u32)(7605.0 / tCK_avg)) + 260U) | (table->burst_regs.emc_dyn_self_ref_control & 0xffff0000U);
      //  WRITE_PARAM_ALL_REG(table, emc_dyn_self_ref_control, dyn_self_ref_control);
      //
      //  WRITE_PARAM_ALL_REG(table, emc_rw2pden, ams::ldr::oc::pcv::erista::tRW2PDEN);
      //  WRITE_PARAM_ALL_REG(table, emc_pdex2wr, GET_CYCLE(10.0));
      //  WRITE_PARAM_ALL_REG(table, emc_pdex2rd, GET_CYCLE(10.0));
      //
      //  WRITE_PARAM_ALL_REG(table, emc_pchg2pden, GET_CYCLE(1.75));
      //  WRITE_PARAM_ALL_REG(table, emc_ar2pden,   GET_CYCLE(1.75));
      //  WRITE_PARAM_ALL_REG(table, emc_pdex2cke,  GET_CYCLE(1.75));
      //  WRITE_PARAM_ALL_REG(table, emc_act2pden,  GET_CYCLE(14.0));
      //  WRITE_PARAM_ALL_REG(table, emc_cke2pden,  GET_CYCLE(5.0));
      //  WRITE_PARAM_ALL_REG(table, emc_pdex2mrr,  GET_CYCLE(ams::ldr::oc::pcv::erista::pdex2mrr));
      //
      //  WRITE_PARAM_ALL_REG(table, emc_refresh,                    refresh_raw);
      //  WRITE_PARAM_ALL_REG(table, emc_pre_refresh_req_cnt, (u32) (refresh_raw / 4));
      //  WRITE_PARAM_ALL_REG(table, emc_trefbw,                     trefbw);
      //
      //  const u32 mc_tRCD = (int)((double)(GET_CYCLE(tRCD) >> 2) - 2.0);
      //  const u32 mc_tRPpb = (int)(((double)(GET_CYCLE(tRPpb) >> 2) - 1.0) + 2.0);
      //  const u32 mc_tRC = (uint)((double)(GET_CYCLE(tRC) >> 2) - 1.0);
      //  const u32 mc_tR2W = (uint)(((double)((uint)tR2W >> 2) - 1.0) + 2.0);
      //  const u32 mc_tW2R = (uint)(((double)(tW2R >> 2) - 1.0) + 2.0);
      //  const u32 mc_tRAS = MIN(GET_CYCLE(tRAS), (u32) 0x7F);
      //  const u32 mc_tRRD = MIN(GET_CYCLE(tRRD), (u32) 31);
      //
      //  table->burst_mc_regs.mc_emem_arb_cfg = (int)(((double) C.marikoEmcMaxClock / 33300.0) * 0.25);
      //  table->burst_mc_regs.mc_emem_arb_timing_ras = (int) ((double) (mc_tRAS >> 2) - 2.0);
      //  table->burst_mc_regs.mc_emem_arb_timing_rcd = (int) ((double) (GET_CYCLE(tRCD) >> 2) - 2.0);
      //  table->burst_mc_regs.mc_emem_arb_timing_rp = (int) (((double) (GET_CYCLE(tRPpb) >> 2) - 1.0) + 2.0);
      //  table->burst_mc_regs.mc_emem_arb_timing_rc = (int) ((double) (GET_CYCLE(tRC) >> 2) - 1.0);
      //  table->burst_mc_regs.mc_emem_arb_timing_faw = (int) ((double)(GET_CYCLE(tFAW) >> 2) - 1.0);
      //  table->burst_mc_regs.mc_emem_arb_timing_rrd = (int)((double)(mc_tRRD >> 2) - 1.0);
      //  table->burst_mc_regs.mc_emem_arb_timing_rap2pre = 3;
      //  table->burst_mc_regs.mc_emem_arb_timing_wap2pre = 11;
      //  table->burst_mc_regs.mc_emem_arb_timing_r2w = (uint)(((double)((uint)tR2W >> 2) - 1.0) + 2.0);
      //  table->burst_mc_regs.mc_emem_arb_timing_w2r = (uint)(((double)(tW2R >> 2) - 1.0) + 2.0);
      //
      //  u32 mc_r2r = table->burst_mc_regs.mc_emem_arb_timing_r2r;
      //  if (mc_r2r > 1) {
      //      mc_r2r = (uint)(((double)(long)((double)rext * 0.25) - 1.0) + 2.0);
      //      table->burst_mc_regs.mc_emem_arb_timing_r2r = mc_r2r;
      //  }
      //
      //  u32 mc_w2w = table->burst_mc_regs.mc_emem_arb_timing_w2w;
      //  if (mc_w2w > 1) {
      //      mc_w2w = (uint)(((double)(long)((double)wext / 4.0) - 1.0) + 2.0);
      //      table->burst_mc_regs.mc_emem_arb_timing_w2w = mc_w2w;
      //  }
      //
      //  table->burst_mc_regs.mc_emem_arb_da_turns = ((mc_tW2R >> 1) << 0x18) | ((mc_tR2W >> 1) << 0x10) | ((mc_r2r >> 1) << 8) | ((mc_w2w >> 1));
      //  table->burst_mc_regs.mc_emem_arb_da_covers = (((uint)(mc_tRCD + 3 + mc_tRPpb) >> 1 & 0xff) << 8) | (((uint)(mc_tRCD + 11 + mc_tRPpb) >> 1 & 0xff) << 0x10) | ((mc_tRC >> 1) & 0xff);
      //  table->burst_mc_regs.mc_emem_arb_misc0 = (table->burst_mc_regs.mc_emem_arb_misc0 & 0xffe08000U) | ((mc_tRC + 1) & 0xff);
      //  table->la_scale_regs.mc_mll_mpcorer_ptsa_rate =         MIN((u32)((C.marikoEmcMaxClock / 1600000) * 0xd0U), (u32)0x115);
      //  table->la_scale_regs.mc_ftop_ptsa_rate =         MIN((u32)((C.marikoEmcMaxClock / 1600000) * 0x18U), (u32)0x1f);
      //  table->la_scale_regs.mc_ptsa_grant_decrement =         MIN((u32)((C.marikoEmcMaxClock / 1600000) * 0x1203U), (u32)0x17ff);
      //
      //  u32 mc_latency_allowance = 0;
      //  if (C.marikoEmcMaxClock / 1000 != 0) {
      //      mc_latency_allowance = 204800 / (C.marikoEmcMaxClock / 1000);
      //  }
      //
      //  const u32 mc_latency_allowance2 = mc_latency_allowance & 0xFF;
      //  const u32 mc_latency_allowance3 = (mc_latency_allowance & 0xFF) << 0x10;
      //  table->la_scale_regs.mc_latency_allowance_xusb_0 = (table->la_scale_regs.mc_latency_allowance_xusb_0 & 0xff00ffffU) | mc_latency_allowance3;
      //  table->la_scale_regs.mc_latency_allowance_sdmmc_0 = (table->la_scale_regs.mc_latency_allowance_sdmmc_0 & 0xff00ffffU) | mc_latency_allowance3;
      //  table->la_scale_regs.mc_latency_allowance_xusb_1 = (table->la_scale_regs.mc_latency_allowance_xusb_1 & 0xff00ffffU) | mc_latency_allowance3;
      //  table->la_scale_regs.mc_latency_allowance_tsec_0 = (table->la_scale_regs.mc_latency_allowance_tsec_0 & 0xff00ffffU) | mc_latency_allowance3;
      //  table->la_scale_regs.mc_latency_allowance_sdmmca_0 = (table->la_scale_regs.mc_latency_allowance_sdmmca_0 & 0xff00ffffU) | mc_latency_allowance3;
      //  table->la_scale_regs.mc_latency_allowance_sdmmcaa_0 = (table->la_scale_regs.mc_latency_allowance_sdmmcaa_0 & 0xff00ffffU) | mc_latency_allowance3;
      //  table->la_scale_regs.mc_latency_allowance_sdmmcab_0 = (table->la_scale_regs.mc_latency_allowance_sdmmcab_0 & 0xff00ffffU) | mc_latency_allowance3;
      //  table->la_scale_regs.mc_latency_allowance_ppcs_1 = (table->la_scale_regs.mc_latency_allowance_ppcs_1 & 0xff00ffffU) | mc_latency_allowance3;
      //  table->la_scale_regs.mc_latency_allowance_mpcore_0 = (table->la_scale_regs.mc_latency_allowance_mpcore_0 & 0xff00ffffU) | mc_latency_allowance3;
      //  table->la_scale_regs.mc_latency_allowance_avpc_0 = (table->la_scale_regs.mc_latency_allowance_avpc_0 & 0xff00ffffU) | mc_latency_allowance3;
      //
      //  u32 mc_latency_allowance_hc_0 = 0;
      //  if (C.marikoEmcMaxClock / 1000 != 0) {
      //      mc_latency_allowance_hc_0 = 35200 / (C.marikoEmcMaxClock / 1000);
      //  }
      //
      //  table->la_scale_regs.mc_latency_allowance_nvdec_0 = (table->la_scale_regs.mc_latency_allowance_nvdec_0 & 0xff00ffffU) | mc_latency_allowance3;
      //  table->la_scale_regs.mc_latency_allowance_hc_0 = (table->la_scale_regs.mc_latency_allowance_hc_0 & 0xffffff00U) | mc_latency_allowance_hc_0;
      //
      //  table->la_scale_regs.mc_latency_allowance_isp2_1 = (table->la_scale_regs.mc_latency_allowance_isp2_1 & 0xff00ff00U) | mc_latency_allowance3 | mc_latency_allowance2;
      //  table->la_scale_regs.mc_latency_allowance_hc_1 = (table->la_scale_regs.mc_latency_allowance_hc_1 & 0xffffff00U) | mc_latency_allowance2;
      //
      //  u32 mc_latency_allowance_gpu_0 = 0;
      //  if (C.marikoEmcMaxClock / 1000 != 0) {
      //      mc_latency_allowance_gpu_0 = 40000 / (C.marikoEmcMaxClock / 1000);
      //  }
      //
      //  table->la_scale_regs.mc_latency_allowance_gpu_0 = ((mc_latency_allowance_gpu_0 | table->la_scale_regs.mc_latency_allowance_gpu_0) & 0xff00ff00U) | mc_latency_allowance3;
      //
      //  u32 mc_latency_allowance_gpu2_0 = 0;
      //  if (C.marikoEmcMaxClock / 1000 != 0) {
      //  mc_latency_allowance_gpu2_0 = 40000 / (C.marikoEmcMaxClock / 1000);
      //  }
      //
      //  table->la_scale_regs.mc_latency_allowance_gpu2_0 = ((mc_latency_allowance_gpu2_0 | table->la_scale_regs.mc_latency_allowance_gpu2_0) & 0xff00ff00U) | mc_latency_allowance3;
      //
      //  u32 mc_latency_allowance_nvenc_0 = 0;
      //  if (C.marikoEmcMaxClock / 1000 != 0) {
      //      mc_latency_allowance_nvenc_0 = 38400 / (C.marikoEmcMaxClock / 1000);
      //  }
      //
      //  table->la_scale_regs.mc_latency_allowance_nvenc_0 = ((mc_latency_allowance_nvenc_0 | table->la_scale_regs.mc_latency_allowance_nvenc_0) & 0xff00ff00U) | mc_latency_allowance3;
      //
      //  u32 mc_latency_allowance_vic_0 = 0;
      //  if (C.marikoEmcMaxClock / 1000 != 0) {
      //      mc_latency_allowance_vic_0 = 0xb540 / (C.marikoEmcMaxClock / 1000);
      //  }
      //
      //  table->la_scale_regs.mc_latency_allowance_vic_0 = ((mc_latency_allowance_vic_0 | table->la_scale_regs.mc_latency_allowance_vic_0) & 0xff00ff00U) | mc_latency_allowance3;
      //  table->la_scale_regs.mc_latency_allowance_vi2_0 = (table->la_scale_regs.mc_latency_allowance_vi2_0 & 0xffffff00U) | mc_latency_allowance2;
      //
      //  table->burst_mc_regs.mc_emem_arb_timing_rfcpb = GET_CYCLE(tRFCpb) >> 2;
      //
      //  if (C.hpMode) {
      //      WRITE_PARAM_ALL_REG(table, emc_cfg, 0x13200000);
      //  }
      //
      //  table->dram_timings.t_rp = tRFCpb;
      //  table->dram_timings.t_rfc = tRFCab;
      //  table->emc_cfg_2 = 0x11083d;
    }

    void MemMtcPllmbDivisor(MarikoMtcTable *table) {
        constexpr u32 PllOscInKHz   = 38400;
        constexpr u32 PllOscHalfKHz = 19200;

        double target_freq_d = static_cast<double>(C.marikoEmcMaxClock);

        s32 divm_candidate_half = static_cast<u8>(C.marikoEmcMaxClock / PllOscHalfKHz);

        bool remainder_check = (C.marikoEmcMaxClock - PllOscInKHz * (C.marikoEmcMaxClock / PllOscInKHz)) > (C.marikoEmcMaxClock - PllOscHalfKHz * divm_candidate_half) && static_cast<int>(((target_freq_d / PllOscHalfKHz - divm_candidate_half - 0.5) * 8192.0)) != 0;

        u32 divm_final = remainder_check + 1;
        table->pllmb_divm = divm_final;

        double div_step_d = static_cast<double>(PllOscInKHz) / divm_final;
        s32 divn_integer = static_cast<u8>(C.marikoEmcMaxClock / div_step_d);
        table->pllmb_divn = divn_integer;

        u32 divn_fraction = static_cast<s32>((target_freq_d / div_step_d - divn_integer - 0.5) * 8192.0);

        u32 actual_freq_khz = static_cast<u32>((divn_integer + 0.5 + divn_fraction * 0.000122070312) * div_step_d);

        if (C.marikoEmcMaxClock - 2366001 <= 133999) {
            s32 divn_fraction_ssc = static_cast<s32>((actual_freq_khz * 0.997 / div_step_d - divn_integer - 0.5) * 8192.0);

            double delta_scaled = (0.3 / div_step_d + 0.3 / div_step_d) * (divn_fraction - divn_fraction_ssc);
            s32 delta_int = static_cast<s32>(delta_scaled);
            double delta_frac = delta_scaled - delta_int;

            u32 setup_value = 0;
            if (delta_frac <= 0.5) {
                double round_val = (delta_int + ROUND(delta_frac + delta_frac)) ? 0.5 : 0.0;
                setup_value = ROUND(delta_frac + delta_frac) ? static_cast<u32>(round_val + round_val) | 0x1000 : static_cast<u32>(round_val);
            } else {
                s32 frac_doubled = ROUND(delta_frac - 0.5 + delta_frac - 0.5);
                double round_val = 1.0;
                setup_value = frac_doubled ? static_cast<u32>(round_val) : static_cast<u32>(round_val + round_val) | 0x1000;
            }

            u32 ctrl1 = static_cast<u16>(divn_fraction_ssc) | (static_cast<u16>(divn_fraction) << 16);
            u32 ctrl2 = static_cast<u16>(divn_fraction) | (static_cast<u16>(setup_value) << 16);

            table->pllm_ss_ctrl1 = ctrl1;
            table->pllm_ss_ctrl2 = ctrl2;
            table->pllmb_ss_ctrl1 = ctrl1;
            table->pllmb_ss_ctrl2 = ctrl2;
        } else {
            table->pllm_ss_cfg &= 0xBFFFFFFF;
            table->pllmb_ss_cfg &= 0xBFFFFFFF;

            u64 pair = (static_cast<u64>(divn_fraction) << 32) | static_cast<u64>(C.marikoEmcMaxClock);
            u32 pll_misc = (table->pllm_ss_ctrl2 & 0xFFFF0000) | static_cast<u32>((pair - actual_freq_khz) >> 32);

            table->pllm_ss_ctrl2 = pll_misc;
            table->pllmb_ss_ctrl2 = pll_misc;
        }
    }

    Result MemFreqMtcTable(u32 *ptr) {
        u32 khz_list[] = {1600000, 1331200, 204000};
        u32 khz_list_size = sizeof(khz_list) / sizeof(u32);

        // Generate list for mtc table pointers
        MarikoMtcTable *table_list[khz_list_size];
        for (u32 i = 0; i < khz_list_size; i++) {
            u8 *table = reinterpret_cast<u8 *>(ptr) - offsetof(MarikoMtcTable, rate_khz) - i * sizeof(MarikoMtcTable);
            table_list[i] = reinterpret_cast<MarikoMtcTable *>(table);
            R_UNLESS(table_list[i]->rate_khz == khz_list[i], ldr::ResultInvalidMtcTable());
            R_UNLESS(table_list[i]->rev == MTC_TABLE_REV, ldr::ResultInvalidMtcTable());
        }

        if (C.marikoEmcMaxClock <= EmcClkOSLimit)
            R_SKIP();

        MarikoMtcTable *table_alt = table_list[1], *table_max = table_list[0];
        MarikoMtcTable *tmp = new MarikoMtcTable;

        // Copy unmodified 1600000 table to tmp
        std::memcpy(reinterpret_cast<void *>(tmp), reinterpret_cast<void *>(table_max), sizeof(MarikoMtcTable));
        // Adjust max freq mtc timing parameters with reference to 1331200 table
        /* TODO: Implement mariko */

        if (C.mtcConf == AUTO_ADJ) {
            MemMtcTableAutoAdjust(table_max);
        } else {
            MemMtcTableAutoAdjustBaseLatency(table_max);
        }

        MemMtcPllmbDivisor(table_max);
        // Overwrite 13312000 table with unmodified 1600000 table copied back
        std::memcpy(reinterpret_cast<void *>(table_alt), reinterpret_cast<void *>(tmp), sizeof(MarikoMtcTable));

        delete tmp;

        PATCH_OFFSET(ptr, C.marikoEmcMaxClock);
        R_SUCCEED();
    }

    Result MemFreqDvbTable(u32 *ptr) {
        emc_dvb_dvfs_table_t *default_end = reinterpret_cast<emc_dvb_dvfs_table_t *>(ptr);
        emc_dvb_dvfs_table_t *new_start = default_end + 1;

        // Validate existing table
        void *mem_dvb_table_head = reinterpret_cast<u8 *>(new_start) - sizeof(EmcDvbTableDefault);
        bool validated = std::memcmp(mem_dvb_table_head, EmcDvbTableDefault, sizeof(EmcDvbTableDefault)) == 0;
        R_UNLESS(validated, ldr::ResultInvalidDvbTable());

        if (C.marikoEmcMaxClock <= EmcClkOSLimit)
            R_SKIP();

        int32_t voltAdd = 25 * C.EmcDvbShift;

#define DVB_VOLT(zero, one, two) std::min(zero + voltAdd, 1050), std::min(one + voltAdd, 1025), std::min(two + voltAdd, 1000),

        if (C.marikoEmcMaxClock < 1862400) {
            std::memcpy(new_start, default_end, sizeof(emc_dvb_dvfs_table_t));
        } else if (C.marikoEmcMaxClock < 2131200) {
            emc_dvb_dvfs_table_t oc_table = {1862400, {700, 675, 650, }};
            std::memcpy(new_start, &oc_table, sizeof(emc_dvb_dvfs_table_t));
        } else if (C.marikoEmcMaxClock < 2400000) {
            emc_dvb_dvfs_table_t oc_table = {2131200, { 725, 700, 675} };
            std::memcpy(new_start, &oc_table, sizeof(emc_dvb_dvfs_table_t));
        } else if (C.marikoEmcMaxClock < 2665600) {
            emc_dvb_dvfs_table_t oc_table = {2400000, {DVB_VOLT(750, 725, 700)}};
            std::memcpy(new_start, &oc_table, sizeof(emc_dvb_dvfs_table_t));
        } else if (C.marikoEmcMaxClock < 2931200) {
            emc_dvb_dvfs_table_t oc_table = {2665600, {DVB_VOLT(775, 750, 725)}};
            std::memcpy(new_start, &oc_table, sizeof(emc_dvb_dvfs_table_t));
        }
        else if (C.marikoEmcMaxClock < 3200000) {
            emc_dvb_dvfs_table_t oc_table = {2931200, {DVB_VOLT(800, 775, 750)}};
            std::memcpy(new_start, &oc_table, sizeof(emc_dvb_dvfs_table_t));
        } else {
            emc_dvb_dvfs_table_t oc_table = {3200000, {DVB_VOLT(800, 800, 775)}};
            std::memcpy(new_start, &oc_table, sizeof(emc_dvb_dvfs_table_t));
        }
        new_start->freq = C.marikoEmcMaxClock;
        /* Max dvfs entry is 32, but HOS doesn't seem to boot if exact freq doesn't exist in dvb table,
           reason why it's like this
        */

        R_SUCCEED();
    }

    Result MemFreqMax(u32 *ptr) {
        if (C.marikoEmcMaxClock <= EmcClkOSLimit)
            R_SKIP();

        PATCH_OFFSET(ptr, C.marikoEmcMaxClock);
        R_SUCCEED();
    }

    Result I2cSet_U8(I2cDevice dev, u8 reg, u8 val) {
        struct {
            u8 reg;
            u8 val;
        } __attribute__((packed)) cmd;

        I2cSession _session;
        Result res = i2cOpenSession(&_session, dev);
        if (R_FAILED(res))
            return res;

        cmd.reg = reg;
        cmd.val = val;
        res = i2csessionSendAuto(&_session, &cmd, sizeof(cmd), I2cTransactionOption_All);
        i2csessionClose(&_session);
        return res;
    }

    Result EmcVddqVolt(u32 *ptr) {
        regulator *entry = reinterpret_cast<regulator *>(reinterpret_cast<u8 *>(ptr) - offsetof(regulator, type_2_3.default_uv));

        constexpr u32 uv_step = 5'000;
        constexpr u32 uv_min = 250'000;

        auto validator = [entry]() {
            R_UNLESS(entry->id == 2, ldr::ResultInvalidRegulatorEntry());
            R_UNLESS(entry->type == 3, ldr::ResultInvalidRegulatorEntry());
            R_UNLESS(entry->type_2_3.step_uv == uv_step, ldr::ResultInvalidRegulatorEntry());
            R_UNLESS(entry->type_2_3.min_uv == uv_min, ldr::ResultInvalidRegulatorEntry());
            R_SUCCEED();
        };

        R_TRY(validator());

        u32 emc_uv = C.marikoEmcVddqVolt;
        if (!emc_uv)
            R_SKIP();

        if (emc_uv % uv_step)
            emc_uv = (emc_uv + uv_step - 1) / uv_step * uv_step; // rounding

        PATCH_OFFSET(ptr, emc_uv);

        i2cInitialize();
        I2cSet_U8(I2cDevice_Max77812_2, 0x25, (emc_uv - uv_min) / uv_step);
        i2cExit();

        R_SUCCEED();
    }

    void Patch(uintptr_t mapped_nso, size_t nso_size) {
        u32 CpuCvbDefaultMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(CpuCvbTableDefault)->freq);
        u32 GpuCvbDefaultMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(GpuCvbTableDefault)->freq);

        PatcherEntry<u32> patches[] = {
            {"CPU Freq Vdd", &CpuFreqVdd, 1, nullptr, CpuClkOSLimit},
            {"CPU Freq Table", CpuFreqCvbTable<true>, 1, nullptr, CpuCvbDefaultMaxFreq},
            {"CPU Volt Limit", &CpuVoltRange, 13, nullptr, CpuVoltOfficial},
            {"CPU Volt Dfll", &CpuVoltDfll, 1, nullptr, 0x0000FFCF},
            {"GPU Freq Table", GpuFreqCvbTable<true>, 1, nullptr, GpuCvbDefaultMaxFreq},
            {"GPU Freq Asm", &GpuFreqMaxAsm, 2, &GpuMaxClockPatternFn},
            {"GPU PLL Max", &GpuFreqPllMax, 1, nullptr, GpuClkPllMax},
            {"GPU PLL Limit", &GpuFreqPllLimit, 4, nullptr, GpuClkPllLimit},
            {"MEM Freq Mtc", &MemFreqMtcTable, 0, nullptr, EmcClkOSLimit},
            {"MEM Freq Dvb", &MemFreqDvbTable, 1, nullptr, EmcClkOSLimit},
            {"MEM Freq Max", &MemFreqMax, 0, nullptr, EmcClkOSLimit},
            {"MEM Freq PLLM", &MemFreqPllmLimit, 2, nullptr, EmcClkPllmLimit},
            {"MEM Vddq", &EmcVddqVolt, 2, nullptr, EmcVddqDefault},
            {"MEM Vdd2", &MemVoltHandler, 2, nullptr, MemVdd2Default},
            {"GPU Vmin", &GpuVmin, 0, nullptr, gpuVmin},
            {"GPU Vmax", &GpuVmax, 0, nullptr, gpuVmax},
        };

        for (uintptr_t ptr = mapped_nso; ptr <= mapped_nso + nso_size - sizeof(MarikoMtcTable); ptr += sizeof(u32)) {
            u32 *ptr32 = reinterpret_cast<u32 *>(ptr);
            for (auto &entry : patches) {
                if (R_SUCCEEDED(entry.SearchAndApply(ptr32))) {
                    break;
                }
            }
        }

        for (auto &entry : patches) {
            LOGGING("%s Count: %zu", entry.description, entry.patched_count);
            if (R_FAILED(entry.CheckResult())) {
                CRASH(entry.description);
            }
        }
    }

}
