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
                    PATCH_OFFSET((ptr - 5), C.marikoCpuHighVmin); // hf vmin
                }
                if (*(ptr - 1) == 620) {
                    PATCH_OFFSET((ptr - 1), C.marikoCpuLowVmin); // lf vmin
                }
            }
            R_SUCCEED();
        }
        R_THROW(ldr::ResultInvalidCpuMinVolt());
    }
Result CpuVoltDfll(u32 *ptr)
    {
        cvb_cpu_dfll_data *entry = reinterpret_cast<cvb_cpu_dfll_data *>(ptr);

        R_UNLESS(entry->tune0_low == 0x0000FFCF, ldr::ResultInvalidCpuVoltDfllEntry());
        R_UNLESS(entry->tune0_high == 0x00000000, ldr::ResultInvalidCpuVoltDfllEntry());
        R_UNLESS(entry->tune1_low == 0x012207FF, ldr::ResultInvalidCpuVoltDfllEntry());
        R_UNLESS(entry->tune1_high == 0x03FFF7FF, ldr::ResultInvalidCpuVoltDfllEntry());
        switch (C.marikoCpuUV)
        {
        case 0:
            break;
        case 1:
            PATCH_OFFSET(&(entry->tune0_low), 0x0000FFA0); // process_id 0 // EOS UV1
            PATCH_OFFSET(&(entry->tune0_high), 0x0000FFFF);
            PATCH_OFFSET(&(entry->tune1_low), 0x021107FF);
            PATCH_OFFSET(&(entry->tune1_high), 0x00000000);
            break;
        case 2:
            // PATCH_OFFSET(&(entry->tune0_low), 0x0000FF92); /// EOS Uv2
            PATCH_OFFSET(&(entry->tune0_high), 0x0000FFDF);
            PATCH_OFFSET(&(entry->tune1_low), 0x021107FF);
            PATCH_OFFSET(&(entry->tune1_high), 0x027207FF);
            break;
        case 3:
            PATCH_OFFSET(&(entry->tune0_low), 0x0000FFDF); // EOS UV3
            PATCH_OFFSET(&(entry->tune0_high), 0x0000FFDF);
            PATCH_OFFSET(&(entry->tune1_low), 0x021107FF);
            PATCH_OFFSET(&(entry->tune1_high), 0x27307ff);
            break;
        case 4:
            PATCH_OFFSET(&(entry->tune0_low), 0x0000FFFF); // EOS Uv4
            PATCH_OFFSET(&(entry->tune0_high), 0x0000FFDF);
            PATCH_OFFSET(&(entry->tune1_low), 0x021107FF);
            PATCH_OFFSET(&(entry->tune1_high), 0x27407ff);
            break;
        case 5:
            // PATCH_OFFSET(&(entry->tune0_low), 0x0000FFFF); // EOS UV5
            PATCH_OFFSET(&(entry->tune0_high), 0x0000FFDF);
            PATCH_OFFSET(&(entry->tune1_low), 0x21607ff);
            PATCH_OFFSET(&(entry->tune1_high), 0x27707ff);
            break;
        case 6:
            // PATCH_OFFSET(&(entry->tune0_low), 0x0000FFFF); // EOS UV6
            PATCH_OFFSET(&(entry->tune0_high), 0x0000FFDF);
            PATCH_OFFSET(&(entry->tune1_low), 0x21607ff);
            PATCH_OFFSET(&(entry->tune1_high), 0x27807ff);
            break;
        case 7:
            /// PATCH_OFFSET(&(entry->tune0_low), 0x0000FFFF); // EOS UV6
            PATCH_OFFSET(&(entry->tune0_high), 0x0000FFFF);
            PATCH_OFFSET(&(entry->tune1_low), 0x21607ff);
            PATCH_OFFSET(&(entry->tune1_high), 0x27b07ff);
            break;
        // case 8:
        //     PATCH_OFFSET(&(entry->tune0_low), 0x0000FFFF); // EOS UV6
        //     PATCH_OFFSET(&(entry->tune0_high), 0x0000FFFF);
        //     PATCH_OFFSET(&(entry->tune1_low), 0x021107FF);
        //     PATCH_OFFSET(&(entry->tune1_high), 0x028817FF);
        //     break;
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

    Result GpuFreqPllLimit(u32 *ptr) {
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

    Result GpuFreqMax(u32 *ptr) {
        PATCH_OFFSET(ptr, 3600000);
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

        /* TODO: Make this less uggly and actually work by finding real clocks */
        if (C.marikoEmcMaxClock > 3'100'000) {
            obdly -= 2;
        }

        if (C.marikoEmcMaxClock > 2'500'000) {
            obdly -= 2;
        }

        WRITE_PARAM_ALL_REG(table, emc_rd_rcd, GET_CYCLE_CEIL(tRCD));
        WRITE_PARAM_ALL_REG(table, emc_wr_rcd, GET_CYCLE_CEIL(tRCD));
        WRITE_PARAM_ALL_REG(table, emc_rc, GET_CYCLE_CEIL(tRC));
        WRITE_PARAM_ALL_REG(table, emc_ras, GET_CYCLE_CEIL(tRAS));
        WRITE_PARAM_ALL_REG(table, emc_rrd, GET_CYCLE_CEIL(tRRD));
        WRITE_PARAM_ALL_REG(table, emc_rfcpb, GET_CYCLE_CEIL(tRFCpb));
        WRITE_PARAM_ALL_REG(table, emc_rfc, GET_CYCLE_CEIL(tRFCab));
        WRITE_PARAM_ALL_REG(table, emc_rp, GET_CYCLE_CEIL(tRPpb));
        WRITE_PARAM_ALL_REG(table, emc_txsr, MIN(GET_CYCLE_CEIL(tXSR), static_cast<u32>(0x3fe)));
        WRITE_PARAM_ALL_REG(table, emc_txsrdll, MIN(GET_CYCLE_CEIL(tXSR), static_cast<u32>(0x3fe)));
        WRITE_PARAM_ALL_REG(table, emc_tfaw, GET_CYCLE_CEIL(tFAW));
        WRITE_PARAM_ALL_REG(table, emc_trpab, GET_CYCLE_CEIL(tRPab));
        WRITE_PARAM_ALL_REG(table, emc_tckesr, GET_CYCLE_CEIL(tSR));
        WRITE_PARAM_ALL_REG(table, emc_tcke, GET_CYCLE_CEIL(tXP) + 1);
        WRITE_PARAM_ALL_REG(table, emc_tpd, GET_CYCLE_CEIL(tXP));
        WRITE_PARAM_ALL_REG(table, emc_tclkstop, GET_CYCLE_CEIL(tXP) + 8);
        WRITE_PARAM_ALL_REG(table, emc_r2p, GET_CYCLE_CEIL(tR2P));
        WRITE_PARAM_ALL_REG(table, emc_r2w, tR2W);
        WRITE_PARAM_ALL_REG(table, emc_trtm, tRTM);
        WRITE_PARAM_ALL_REG(table, emc_tratm, tRATM);
        WRITE_PARAM_ALL_REG(table, emc_w2p, tW2P);
        WRITE_PARAM_ALL_REG(table, emc_w2r, tW2R);
        WRITE_PARAM_ALL_REG(table, emc_twtm, tWTM);
        WRITE_PARAM_ALL_REG(table, emc_twatm, tWATM);
        WRITE_PARAM_ALL_REG(table, emc_rext, 26);
        WRITE_PARAM_ALL_REG(table, emc_refresh, refresh_raw);
        WRITE_PARAM_ALL_REG(table, emc_pre_refresh_req_cnt, refresh_raw / 4);
        WRITE_PARAM_ALL_REG(table, emc_trefbw, trefbw);
        const u32 dyn_self_ref_control = (((u32)(7605.0 / tCK_avg)) + 260U) | (table->burst_regs.emc_dyn_self_ref_control & 0xffff0000U);
        WRITE_PARAM_ALL_REG(table, emc_dyn_self_ref_control, dyn_self_ref_control);
        WRITE_PARAM_ALL_REG(table, emc_pdex2wr, GET_CYCLE_CEIL(10.0));
        WRITE_PARAM_ALL_REG(table, emc_pdex2rd, GET_CYCLE_CEIL(10.0));
        WRITE_PARAM_ALL_REG(table, emc_pchg2pden, GET_CYCLE_CEIL(1.75));
        WRITE_PARAM_ALL_REG(table, emc_ar2pden, GET_CYCLE_CEIL(1.75));
        WRITE_PARAM_ALL_REG(table, emc_act2pden, GET_CYCLE_CEIL(14.0));
        WRITE_PARAM_ALL_REG(table, emc_cke2pden, GET_CYCLE_CEIL(5.0));
        WRITE_PARAM_ALL_REG(table, emc_pdex2mrr, GET_CYCLE_CEIL(tPDEX2MRR));
        WRITE_PARAM_ALL_REG(table, emc_rw2pden, tWTPDEN);
        WRITE_PARAM_ALL_REG(table, emc_einput, 0xF);
        WRITE_PARAM_ALL_REG(table, emc_einput_duration, 0x31);
        WRITE_PARAM_ALL_REG(table, emc_obdly, obdly);
        WRITE_PARAM_ALL_REG(table, emc_ibdly, 0x1000001C);
        WRITE_PARAM_ALL_REG(table, emc_wdv_mask, wdv);
        WRITE_PARAM_ALL_REG(table, emc_quse_width, 0xD);
        WRITE_PARAM_ALL_REG(table, emc_quse, 0x2F);
        WRITE_PARAM_ALL_REG(table, emc_wdv, wdv);
        WRITE_PARAM_ALL_REG(table, emc_wsv, wsv);
        WRITE_PARAM_ALL_REG(table, emc_wev, wev);
        WRITE_PARAM_ALL_REG(table, emc_qrst, 0x00080005);
        WRITE_PARAM_ALL_REG(table, emc_qsafe, 0x44);
        WRITE_PARAM_ALL_REG(table, emc_tr_qpop, 0x3B);
        WRITE_PARAM_ALL_REG(table, emc_rdv, 0x49);
        WRITE_PARAM_ALL_REG(table, emc_qpop, 0x3B);
        WRITE_PARAM_ALL_REG(table, emc_tr_rdv_mask, 0x4B);
        WRITE_PARAM_ALL_REG(table, emc_rdv_early, 0x47);
        WRITE_PARAM_ALL_REG(table, emc_rdv_early_mask, 0x49);
        WRITE_PARAM_ALL_REG(table, emc_rdv_mask, 0x4B);
        WRITE_PARAM_ALL_REG(table, emc_tr_rdv, 0x49);

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

        table->pllm_ss_ctrl1 = 0xb55fe01;
        table->pllm_ss_ctrl2 = 0x10170b55;
        table->pllmb_ss_ctrl1 = 0xb55fe01;
        table->pllmb_ss_ctrl2 = 0x10170b55;

        table->dram_timings.t_rp = tRFCpb;
        table->dram_timings.t_rfc = tRFCab;
        table->dram_timings.rl = RL - 10;
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

        #define GET_CYCLE_CEIL(PARAM) u32(CEIL(double(PARAM) / tCK_avg))

        WRITE_PARAM_ALL_REG(table, emc_rc, 0x60);
//        WRITE_PARAM_ALL_REG(table, emc_rfc, GET_CYCLE_CEIL(tRFCab));
//        WRITE_PARAM_ALL_REG(table, emc_rfcpb, GET_CYCLE_CEIL(tRFCpb));
//        WRITE_PARAM_ALL_REG(table, emc_ras, GET_CYCLE_CEIL(tRAS));
//        WRITE_PARAM_ALL_REG(table, emc_rp, GET_CYCLE_CEIL(tRPpb));
//        WRITE_PARAM_ALL_REG(table, emc_r2p, GET_CYCLE_CEIL(tRTP));
//        WRITE_PARAM_ALL_REG(table, emc_rd_rcd, GET_CYCLE_CEIL(tRCD));
//        WRITE_PARAM_ALL_REG(table, emc_wr_rcd, GET_CYCLE_CEIL(tRCD));
//        WRITE_PARAM_ALL_REG(table, emc_rrd, GET_CYCLE_CEIL(tRRD));
//        WRITE_PARAM_ALL_REG(table, emc_refresh, REFRESH);
//        WRITE_PARAM_ALL_REG(table, emc_pre_refresh_req_cnt, REFRESH / 4);
//
//        WRITE_PARAM_ALL_REG(table, emc_r2w, R2W);
//        WRITE_PARAM_ALL_REG(table, emc_w2r, W2R);
//        WRITE_PARAM_ALL_REG(table, emc_w2p, WTP);
//
//        /* May or may not have to be patched in Micron; let's skip for now. */
//        if (!IsMicron())
//        {
//            WRITE_PARAM_ALL_REG(table, emc_pdex2wr, GET_CYCLE_CEIL(tXP));
//            WRITE_PARAM_ALL_REG(table, emc_pdex2rd, GET_CYCLE_CEIL(tXP));
//        }
//
//        WRITE_PARAM_ALL_REG(table, emc_txsr, MIN(GET_CYCLE_CEIL(tXSR), (u32)0x3fe));
//        WRITE_PARAM_ALL_REG(table, emc_txsrdll, MIN(GET_CYCLE_CEIL(tXSR), (u32)0x3fe));
//        WRITE_PARAM_ALL_REG(table, emc_tckesr, GET_CYCLE_CEIL(tSR));
//        WRITE_PARAM_ALL_REG(table, emc_tfaw, GET_CYCLE_CEIL(tFAW));
//        WRITE_PARAM_ALL_REG(table, emc_trpab, GET_CYCLE_CEIL(tRPab));
//        WRITE_PARAM_ALL_REG(table, emc_trefbw, REFBW);
//
///* Worth replacing with l4t dumps at some point. */
//// Burst MC Regs
//#define WRITE_PARAM_BURST_MC_REG(TABLE, PARAM, VALUE) TABLE->burst_mc_regs.PARAM = VALUE;
//
//        constexpr u32 MC_ARB_DIV = 4;
//        constexpr u32 MC_ARB_SFA = 2;
//
//        WRITE_PARAM_BURST_MC_REG(table, mc_emem_arb_cfg, C.marikoEmcMaxClock / (33.3 * 1000) / MC_ARB_DIV); // CYCLES_PER_UPDATE: The number of mcclk cycles per deadline timer update
//        WRITE_PARAM_BURST_MC_REG(table, mc_emem_arb_timing_rcd, CEIL(GET_CYCLE_CEIL(tRCD) / MC_ARB_DIV) - 2)
//        WRITE_PARAM_BURST_MC_REG(table, mc_emem_arb_timing_rp, CEIL(GET_CYCLE_CEIL(tRPpb) / MC_ARB_DIV) - 1 + MC_ARB_SFA)
//        WRITE_PARAM_BURST_MC_REG(table, mc_emem_arb_timing_rc, CEIL(GET_CYCLE_CEIL(tRC) / MC_ARB_DIV) - 1)
//        WRITE_PARAM_BURST_MC_REG(table, mc_emem_arb_timing_ras, CEIL(GET_CYCLE_CEIL(tRAS) / MC_ARB_DIV) - 2)
//        WRITE_PARAM_BURST_MC_REG(table, mc_emem_arb_timing_faw, CEIL(GET_CYCLE_CEIL(tFAW) / MC_ARB_DIV) - 1)
//        WRITE_PARAM_BURST_MC_REG(table, mc_emem_arb_timing_rrd, CEIL(GET_CYCLE_CEIL(tRRD) / MC_ARB_DIV) - 1)
//        WRITE_PARAM_BURST_MC_REG(table, mc_emem_arb_timing_rap2pre, CEIL(GET_CYCLE_CEIL(tRTP) / MC_ARB_DIV))
//        WRITE_PARAM_BURST_MC_REG(table, mc_emem_arb_timing_wap2pre, CEIL((WTP) / MC_ARB_DIV))
//        WRITE_PARAM_BURST_MC_REG(table, mc_emem_arb_timing_r2r, CEIL(table->burst_regs.emc_rext / MC_ARB_DIV) - 1 + MC_ARB_SFA)
//        WRITE_PARAM_BURST_MC_REG(table, mc_emem_arb_timing_r2w, CEIL((R2W) / MC_ARB_DIV) - 1 + MC_ARB_SFA)
//        WRITE_PARAM_BURST_MC_REG(table, mc_emem_arb_timing_w2r, CEIL((W2R) / MC_ARB_DIV) - 1 + MC_ARB_SFA)
//        WRITE_PARAM_BURST_MC_REG(table, mc_emem_arb_timing_rfcpb, CEIL(GET_CYCLE_CEIL(tRFCpb) / MC_ARB_DIV))
    }

    void MemMtcPllmbDivisor(MarikoMtcTable *table) {
        // Calculate DIVM and DIVN (clock divisors)
        // Common PLL oscillator is 38.4 MHz
        // PLLMB_OUT = 38.4 MHz / PLLLMB_DIVM * PLLMB_DIVN
        typedef struct {
            u8 numerator : 4;
            u8 denominator : 4;
        } pllmb_div;

        constexpr pllmb_div div[] = {
            {3, 4}, {2, 3}, {1, 2}, {1, 3}, {1, 4}, {0, 2}
        };

        constexpr u32 pll_osc_in = 38'400;
        u32 divm{}, divn{};
        const u32 remainder = C.marikoEmcMaxClock % pll_osc_in;
        for (const auto &index : div) {
            // Round down
            if (remainder >= pll_osc_in * index.numerator / index.denominator) {
                divm = index.denominator;
                divn = C.marikoEmcMaxClock / pll_osc_in * divm + index.numerator;
                break;
            }
        }

        table->pllmb_divm = divm;
        table->pllmb_divn = divn;
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
            {"GPU Freq PLL", &GpuFreqPllLimit, 1, nullptr, GpuClkPllLimit},
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
