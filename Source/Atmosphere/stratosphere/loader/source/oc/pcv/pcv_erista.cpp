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

namespace ams::ldr::oc::pcv::erista {

    Result CpuFreqVdd(u32* ptr) {
        dvfs_rail* entry = reinterpret_cast<dvfs_rail *>(reinterpret_cast<u8 *>(ptr) - offsetof(dvfs_rail, freq));

        R_UNLESS(entry->id == 1,            ldr::ResultInvalidCpuFreqVddEntry());
        R_UNLESS(entry->min_mv == 250'000,  ldr::ResultInvalidCpuFreqVddEntry());
        R_UNLESS(entry->step_mv == 5000,    ldr::ResultInvalidCpuFreqVddEntry());
        R_UNLESS(entry->max_mv == 1525'000, ldr::ResultInvalidCpuFreqVddEntry());

        if (C.eristaCpuUV) {
            if(!C.enableEristaCpuUnsafeFreqs) {
                PATCH_OFFSET(ptr, GetDvfsTableLastEntry(C.eristaCpuDvfsTable)->freq);
            } else {
                PATCH_OFFSET(ptr, GetDvfsTableLastEntry(C.eristaCpuDvfsTableUnsafeFreqs)->freq);
            }
        } else {
            PATCH_OFFSET(ptr, GetDvfsTableLastEntry(C.eristaCpuDvfsTable)->freq);
        }

        R_SUCCEED();
    }

    Result GpuVmin(u32 *ptr) {
        if (!C.eristaGpuVmin)
            R_SKIP();
            PATCH_OFFSET(ptr, (int)C.eristaGpuVmin);
            R_SUCCEED();
    }

    Result CpuVoltRange(u32 *ptr) {
        u32 min_volt_got = *(ptr - 1);
        for (const auto &mv : CpuMinVolts) {
            if (min_volt_got != mv)
                continue;

            if (!C.eristaCpuMaxVolt)
                R_SKIP();

            PATCH_OFFSET(ptr, C.eristaCpuMaxVolt);
            R_SUCCEED();
        }
        R_THROW(ldr::ResultInvalidCpuMinVolt());
    }

    Result CpuVoltDfll(u32* ptr) {
        cvb_cpu_dfll_data *entry = reinterpret_cast<cvb_cpu_dfll_data *>(ptr);

    // R_UNLESS(entry->tune0_low == 0x0000FFCF,   ldr::ResultInvalidCpuVoltDfllEntry());
    //        R_UNLESS(entry->tune0_high == 0x00000000,    ldr::ResultInvalidCpuVoltDfllEntry());
    //        R_UNLESS(entry->tune1_low == 0x012207FF,   ldr::ResultInvalidCpuVoltDfllEntry());
    //        R_UNLESS(entry->tune1_high == 0x03FFF7FF,    ldr::ResultInvalidCpuVoltDfllEntry());
        if(!C.eristaCpuUV) {
            R_SKIP();
        }
        PATCH_OFFSET(&(entry->dvco_calibration_max), 0x1C);
        PATCH_OFFSET(&(entry->tune1_high), 0x10);
        PATCH_OFFSET(&(entry->tune_high_margin_millivolts), 0xc);

        switch(C.eristaCpuUV) {
            case 1:
                PATCH_OFFSET(&(entry->tune0_low), 0x0000FFFF); //process_id 0 // EOS UV1
                PATCH_OFFSET(&(entry->tune1_low), 0x027007FF);
                break;
            case 2:
                PATCH_OFFSET(&(entry->tune0_low), 0x0000EFFF); //process_id 1 // EOS Uv2
                PATCH_OFFSET(&(entry->tune1_low), 0x027407FF);
                break;
            case 3:
                PATCH_OFFSET(&(entry->tune0_low), 0x0000DFFF); //process_id 0 // EOS UV3
                PATCH_OFFSET(&(entry->tune1_low), 0x027807FF);
                break;
            case 4:
                PATCH_OFFSET(&(entry->tune0_low), 0x0000DFDF); //process_id 1 // EOS Uv4
                PATCH_OFFSET(&(entry->tune1_low), 0x027A07FF);
                break;
            case 5:
                PATCH_OFFSET(&(entry->tune0_low), 0x0000CFDF); // EOS UV5
                PATCH_OFFSET(&(entry->tune1_low), 0x037007FF);
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
        switch (C.eristaGpuUV) {
        case 0:
            max_clock = GetDvfsTableLastEntry(C.eristaGpuDvfsTable)->freq;
            break;
        case 1:
            max_clock = GetDvfsTableLastEntry(C.eristaGpuDvfsTableSLT)->freq;
            break;
        case 2:
            max_clock = GetDvfsTableLastEntry(C.eristaGpuDvfsTableHigh)->freq;
            break;
        case 3:
            if(C.enableEristaGpuUnsafeFreqs) {
                max_clock = GetDvfsTableLastEntry(C.eristaGpuDvfsTableUv3UnsafeFreqs)->freq;
            } else {
                max_clock = GetDvfsTableLastEntry(C.eristaGpuDvfsTable)->freq;
            }
            break;
        default:
            max_clock = GetDvfsTableLastEntry(C.eristaGpuDvfsTable)->freq;
            break;
        }
        u32 asm_patch[2] = {
            asm_set_rd(asm_set_imm16(asm_pattern[0], max_clock), rd),
            asm_set_rd(asm_set_imm16(asm_pattern[1], max_clock >> 16), rd)};
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

    /* This is not done properly, this is not scaled correctly.                                                                                              */
    /* It is fixable of course, but I don't know if I hate myself enough to fix it, especially considering erista does not benefit much from proper timings. */
    /* This currently patches a lot of unwanted extra stuff that needs to be removed.                                                                        */
    void MemMtcTableAutoAdjustBaseLatency(EristaMtcTable *table) {
        using namespace pcv::erista;
        #define WRITE_PARAM_ALL_REG(TABLE, PARAM, VALUE) \
            TABLE->burst_regs.PARAM = VALUE;             \
            TABLE->shadow_regs_ca_train.PARAM   = VALUE; \
            TABLE->shadow_regs_quse_train.PARAM = VALUE; \
            TABLE->shadow_regs_rdwr_train.PARAM = VALUE;

        #define GET_CYCLE(PARAM) ((u32)((double)(PARAM) / tCK_avg))

        /* This condition is insane but it's done in eos. */
        /* Need to clean up at some point. */
        u32 rext;
        u32 wext;
        if (C.eristaEmcMaxClock < 3200001) {
            if (C.eristaEmcMaxClock < 2133001) {
                rext = 26;
                wext = 22;
            } else {
                rext = 28;
                wext = 22;

                if (2400000 < C.eristaEmcMaxClock) {
                    wext = 25;
                }
            }
        } else {
            rext = 30;
            wext = 25;
        }

        u32 refresh_raw = 0xFFFF;
        u32 trefbw = 0;

        if (C.t8_tREFI != 6) {
            refresh_raw = static_cast<u32>(std::floor(static_cast<double>(tREFpb_values[C.t8_tREFI]) / tCK_avg)) - 0x40;
            refresh_raw = MIN(refresh_raw, static_cast<u32>(0xFFFF));
        }

        trefbw = refresh_raw + 0x40;
        trefbw = MIN(trefbw, static_cast<u32>(0x3FFF));

        if (C.hpMode) {
            WRITE_PARAM_ALL_REG(table, emc_cfg, 0x13200000);
        } else {
            WRITE_PARAM_ALL_REG(table, emc_cfg, 0xF3200000);
        }

        WRITE_PARAM_ALL_REG(table, emc_rc,                           /*0x00000060*/ GET_CYCLE(tRC));
        WRITE_PARAM_ALL_REG(table, emc_rfc,                       /*0x00000120*/ GET_CYCLE(tRFCab));
        WRITE_PARAM_ALL_REG(table, emc_ras,                         /*0x00000044*/ GET_CYCLE(tRAS));
        WRITE_PARAM_ALL_REG(table, emc_rp,                         /*0x0000001D*/ GET_CYCLE(tRPpb));
        WRITE_PARAM_ALL_REG(table, emc_r2w,                                    /*0x0000002A*/ tR2W);
        WRITE_PARAM_ALL_REG(table, emc_w2r,                                    /*0x00000021*/ tW2R);
        WRITE_PARAM_ALL_REG(table, emc_r2p,                                             0x0000000C);
        WRITE_PARAM_ALL_REG(table, emc_w2p,                                             0x0000002D);
        WRITE_PARAM_ALL_REG(table, emc_rd_rcd,                      /*0x0000001D*/ GET_CYCLE(tRCD));
        WRITE_PARAM_ALL_REG(table, emc_wr_rcd,                      /*0x0000001D*/ GET_CYCLE(tRCD));
        WRITE_PARAM_ALL_REG(table, emc_rrd,                         /*0x00000010*/ GET_CYCLE(tRRD));
        WRITE_PARAM_ALL_REG(table, emc_rext,                                            0x00000017);
        WRITE_PARAM_ALL_REG(table, emc_wdv,                                             0x0000000E);
        WRITE_PARAM_ALL_REG(table, emc_quse,                                            0x00000024);
        WRITE_PARAM_ALL_REG(table, emc_qrst,                                            0x0006000C);
        WRITE_PARAM_ALL_REG(table, emc_qsafe,                                           0x00000034);
        WRITE_PARAM_ALL_REG(table, emc_rdv,                                             0x0000003C);
        WRITE_PARAM_ALL_REG(table, emc_refresh,                         /*0x00001820*/ refresh_raw);
        WRITE_PARAM_ALL_REG(table, emc_burst_refresh_num,                               0x00000000);
        WRITE_PARAM_ALL_REG(table, emc_pdex2wr,                                         0x00000010);
        WRITE_PARAM_ALL_REG(table, emc_pdex2rd,                                         0x00000010);
        WRITE_PARAM_ALL_REG(table, emc_pchg2pden,                                       0x00000003);
        WRITE_PARAM_ALL_REG(table, emc_act2pden,                                        0x00000003);
        WRITE_PARAM_ALL_REG(table, emc_ar2pden,                                         0x00000003);
        WRITE_PARAM_ALL_REG(table, emc_rw2pden,                 /*0x00000038*/ GET_CYCLE(tRW2PDEN));
        WRITE_PARAM_ALL_REG(table, emc_txsr,       /*0x0000012C*/ MIN(GET_CYCLE(tXSR), (u32) 1022));
        WRITE_PARAM_ALL_REG(table, emc_tcke,                                            0x0000000D);
        WRITE_PARAM_ALL_REG(table, emc_tfaw,                        /*0x00000040*/ GET_CYCLE(tFAW));
        WRITE_PARAM_ALL_REG(table, emc_trpab,                      /*0x00000022*/ GET_CYCLE(tRPab));
        WRITE_PARAM_ALL_REG(table, emc_tclkstable,                                      0x00000004);
        WRITE_PARAM_ALL_REG(table, emc_tclkstop,                                        0x00000014);
        WRITE_PARAM_ALL_REG(table, emc_trefbw,                              /* 0x00001860*/ trefbw);
        WRITE_PARAM_ALL_REG(table, emc_tppd,                                            0x00000004);
        WRITE_PARAM_ALL_REG(table, emc_odt_write,                                       0x00000000);
        WRITE_PARAM_ALL_REG(table, emc_pdex2mrr,                /*0x0000002E*/ GET_CYCLE(pdex2mrr));
        WRITE_PARAM_ALL_REG(table, emc_wext,                                            0x00000016);
        WRITE_PARAM_ALL_REG(table, emc_rfc_slr,                                         0x00000000);
        WRITE_PARAM_ALL_REG(table, emc_mrs_wait_cnt2,                                   0x01900017);
        WRITE_PARAM_ALL_REG(table, emc_mrs_wait_cnt,                                    0x0640002F);
        // table->emc_mrs  =                                                            0x00000000;
        // table->emc_emrs =                                                            0x00000000;
        // table->emc_mrw  =                                                            0x00170040;
        WRITE_PARAM_ALL_REG(table, emc_fbio_spare,                                      0x00000012);
        WRITE_PARAM_ALL_REG(table, emc_fbio_cfg5,                                       0x9960A00D);
        WRITE_PARAM_ALL_REG(table, emc_pdex2cke,                                        0x00000002);
        WRITE_PARAM_ALL_REG(table, emc_cke2pden,                                        0x0000000E);
        // table->emc_emrs2 =                                                           0x00000000;
        // table->emc_mrw2  =                                                           0x0802002D;
        // table->emc_mrw3  =                                                           0x0C0D00C0;
        // table->emc_mrw4  =                                                           0xC0000000;
        WRITE_PARAM_ALL_REG(table, emc_r2r,                                             0x00000000);
        WRITE_PARAM_ALL_REG(table, emc_einput,                                          0x00000014);
        WRITE_PARAM_ALL_REG(table, emc_einput_duration,                                 0x0000001D);
        WRITE_PARAM_ALL_REG(table, emc_puterm_extra,                                    0x0000001F);
        WRITE_PARAM_ALL_REG(table, emc_tckesr,                                          0x00000018);
        WRITE_PARAM_ALL_REG(table, emc_tpd,                                             0x0000000C);
        table->emc_auto_cal_config =                                                    0x201A51D8;
        table->emc_cfg_2           =                                                    0x00110835;
        WRITE_PARAM_ALL_REG(table, emc_cfg_dig_dll,                                     0x002C03A9);
        WRITE_PARAM_ALL_REG(table, emc_cfg_dig_dll_period,                              0x00008000);
        WRITE_PARAM_ALL_REG(table, emc_rdv_mask,                                        0x0000003E);
        WRITE_PARAM_ALL_REG(table, emc_wdv_mask,                                        0x0000000E);
        WRITE_PARAM_ALL_REG(table, emc_rdv_early_mask,                                  0x0000003C);
        WRITE_PARAM_ALL_REG(table, emc_rdv_early,                                       0x0000003A);
        table->emc_auto_cal_config8 =                                                   0x00770000;
        WRITE_PARAM_ALL_REG(table, emc_zcal_interval,                                   0x00064000);
        WRITE_PARAM_ALL_REG(table, emc_zcal_wait_cnt,                                   0x00310640);
        WRITE_PARAM_ALL_REG(table, emc_fdpd_ctrl_dq,                                    0x8020221F);
        WRITE_PARAM_ALL_REG(table, emc_fdpd_ctrl_cmd,                                   0x0220F40F);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_cmd_brick_ctrl_fdpd,                   0x00000000);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_data_brick_ctrl_fdpd,                  0x00000000);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_brick_ctrl_rfu1,                       0x1FFF1FFF);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_brick_ctrl_rfu2,                       0x00000000);
        WRITE_PARAM_ALL_REG(table, emc_tr_timing_0,                                     0x01186190);
        // WRITE_PARAM_ALL_REG(table, emc_tr_ctrl_1,                                    0x00000000);
        WRITE_PARAM_ALL_REG(table, emc_tr_rdv,                                          0x0000003C);
        table->emc_sel_dpd_ctrl =                                                       0x00040000;
        WRITE_PARAM_ALL_REG(table, emc_pre_refresh_req_cnt, /*0x00000608*/ (u32) (refresh_raw / 4));
        WRITE_PARAM_ALL_REG(table, emc_dyn_self_ref_control,                            0x8000308C);
        WRITE_PARAM_ALL_REG(table, emc_txsrdll,    /*0x0000012C*/ MIN(GET_CYCLE(tXSR), (u32) 1022));
        WRITE_PARAM_ALL_REG(table, emc_tr_qpop,                                         0x0000002C);
        WRITE_PARAM_ALL_REG(table, emc_tr_rdv_mask,                                     0x0000003E);
        WRITE_PARAM_ALL_REG(table, emc_tr_qsafe,                                        0x00000034);
        WRITE_PARAM_ALL_REG(table, emc_tr_qrst,                                         0x0006000C);
        table->emc_auto_cal_config2 =                                                   0x05500000;
        table->emc_auto_cal_config3 =                                                   0x00770000;
        // WRITE_PARAM_ALL_REG(table, emc_tr_dvfs,                                      0x00000000);
        WRITE_PARAM_ALL_REG(table, emc_auto_cal_channel,                                0xC1E0030A);
        WRITE_PARAM_ALL_REG(table, emc_ibdly,                                           0x1000001C);
        WRITE_PARAM_ALL_REG(table, emc_obdly,                                           0x10000002);
        WRITE_PARAM_ALL_REG(table, emc_txdsrvttgen,                                     0x00000000);
        WRITE_PARAM_ALL_REG(table, emc_we_duration,                                     0x0000000D);
        WRITE_PARAM_ALL_REG(table, emc_ws_duration,                                     0x00000008);
        WRITE_PARAM_ALL_REG(table, emc_wev,                                             0x0000000A);
        WRITE_PARAM_ALL_REG(table, emc_wsv,                                             0x0000000C);
        WRITE_PARAM_ALL_REG(table, emc_cfg_3,                                           0x00000040);
        // WRITE_PARAM_ALL_REG(table, emc_mrw6,                                         0x08037171);
        // WRITE_PARAM_ALL_REG(table, emc_mrw7,                                         0x48037171);
        // WRITE_PARAM_ALL_REG(table, emc_mrw8,                                         0x080B6666);
        // table->emc_mrw9  =                                                           0x0C0E7272;
        // table->emc_mrw10 =                                                           0x880C4848;
        // table->emc_mrw11 =                                                           0x480C4848; /* Check them maybe */
        // table->emc_mrw12 =                                                           0x880E1718;
        // table->emc_mrw13 =                                                           0x480E1814;
        // WRITE_PARAM_ALL_REG(table, emc_mrw14,                                        0x08161414);
        // WRITE_PARAM_ALL_REG(table, emc_mrw15,                                        0x48161414);
        // table->emc_fdpd_ctrl_cmd_no_ramp =                                           0x00000001;
        WRITE_PARAM_ALL_REG(table, emc_wdv_chk,                                         0x00000006);
        // WRITE_PARAM_ALL_REG(table, emc_cfg_pipe_2,                                   0x00000000);
        // WRITE_PARAM_ALL_REG(table, emc_cfg_pipe_1,                                   0x00000000);
        // WRITE_PARAM_ALL_REG(table, emc_cfg_pipe,                                     0x00000000);
        WRITE_PARAM_ALL_REG(table, emc_qpop,                                            0x0000002C);
        WRITE_PARAM_ALL_REG(table, emc_quse_width,                                      0x00000009);
        WRITE_PARAM_ALL_REG(table, emc_puterm_width,                                    0x0000000E);
        table->emc_auto_cal_config7 =                                                   0x00770000;
        // WRITE_PARAM_ALL_REG(table, emc_refctrl2,                                     0x00000000);
        WRITE_PARAM_ALL_REG(table, emc_fbio_cfg7,                                       0x00003BFF);
        WRITE_PARAM_ALL_REG(table, emc_rfcpb,                     /*0x00000090*/ GET_CYCLE(tRFCpb));
        // WRITE_PARAM_ALL_REG(table, emc_dqs_brlshft_0,                                0x00000000); /* brlshft may or may not be important, I don't think it matters but who knows. */
        // WRITE_PARAM_ALL_REG(table, emc_dqs_brlshft_1,                                0x00000000);
        table->emc_auto_cal_config4 =                                                   0x00770000;
        table->emc_auto_cal_config5 =                                                   0x00770000;
        WRITE_PARAM_ALL_REG(table, emc_ccdmw,                                           0x00000020);
        table->emc_auto_cal_config6 =                                                   0x00770000;
        WRITE_PARAM_ALL_REG(table, emc_dll_cfg_0,                                       0x1F13612F);
        WRITE_PARAM_ALL_REG(table, emc_dll_cfg_1,                                       0x00000014);
        WRITE_PARAM_ALL_REG(table, emc_config_sample_delay,                             0x00000020);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_tx_pwrd_0,                             0x10000000);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_tx_pwrd_1,                             0x08000000);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_tx_pwrd_2,                             0x08000000);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_tx_pwrd_3,                             0x00000000);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_tx_pwrd_4,                             0x00000000);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_tx_pwrd_5,                             0x00001000);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_ddll_bypass,                           0xEFFF2210);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_ddll_pwrd_0,                           0x00000000);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_ddll_pwrd_1,                           0x00000000);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_ddll_pwrd_2,                           0xDCDCDCDC);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_cmd_ctrl_0,                            0x0A0A0A0A);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_cmd_ctrl_1,                            0x0A0A0A0A);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_cmd_ctrl_2,                            0x000A0A0A);
        // table->trim_regs.emc_pmacro_ib_vref_dq_0  =                                  0x15171414;
        // table->trim_regs.emc_pmacro_ib_vref_dq_1  =                                  0x15131513;
        // table->trim_regs.emc_pmacro_ib_vref_dqs_0 =                                  0x11111111;
        // table->trim_regs.emc_pmacro_ib_vref_dqs_1 =                                  0x11111111;
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_ddll_long_cmd_0,                       0x000C000C);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_ddll_long_cmd_1,                       0x000B000B);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_ddll_long_cmd_2,                       0x000A000A);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_ddll_long_cmd_3,                       0x000C000C);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_ddll_long_cmd_4,                       0x0000000C);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_ddll_short_cmd_0,                      0x00000000);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_ddll_short_cmd_1,                      0x00000000);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_ddll_short_cmd_2,                      0x00000000);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_vttgen_ctrl_0,                         0x00030808);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_vttgen_ctrl_1,                         0x00015C00);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_bg_bias_ctrl_0,                        0x00000034);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_pad_cfg_ctrl,                          0x00020000);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_zctrl,                                 0x00000550);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_cmd_pad_rx_ctrl,                       0x00000000);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_data_pad_rx_ctrl,                      0x00000033);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_cmd_rx_term_mode,                      0x00003000);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_data_rx_term_mode,                     0x00000011);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_cmd_pad_tx_ctrl,                       0x02000000);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_data_pad_tx_ctrl,                      0x02000101);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_common_pad_tx_ctrl,                    0x00000007);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_autocal_cfg_common,                    0x0000080D);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_vttgen_ctrl_2,                         0x00102020);
        // WRITE_PARAM_ALL_REG(table, emc_pmacro_ib_rxrt,                               0x00000055);
        WRITE_PARAM_ALL_REG(table, emc_training_ctrl,                                   0x00009080);
        WRITE_PARAM_ALL_REG(table, emc_training_quse_cors_ctrl,                         0x01124000);
        WRITE_PARAM_ALL_REG(table, emc_training_quse_fine_ctrl,                         0x01125B6A);
        WRITE_PARAM_ALL_REG(table, emc_training_quse_ctrl_misc,                         0x0F081000);
        WRITE_PARAM_ALL_REG(table, emc_training_write_fine_ctrl,                        0x1114FC00);
        WRITE_PARAM_ALL_REG(table, emc_training_write_ctrl_misc,                        0x07004300);
        WRITE_PARAM_ALL_REG(table, emc_training_write_vref_ctrl,                        0x00103200);
        WRITE_PARAM_ALL_REG(table, emc_training_read_fine_ctrl,                         0x1110FC00);
        WRITE_PARAM_ALL_REG(table, emc_training_read_ctrl_misc,                         0x0F085300);
        WRITE_PARAM_ALL_REG(table, emc_training_read_vref_ctrl,                         0x00105800);
        WRITE_PARAM_ALL_REG(table, emc_training_ca_fine_ctrl,                           0x0513801F);
        WRITE_PARAM_ALL_REG(table, emc_training_ca_ctrl_misc,                           0x1F101100);
        WRITE_PARAM_ALL_REG(table, emc_training_ca_ctrl_misc1,                          0x00000014);
        WRITE_PARAM_ALL_REG(table, emc_training_ca_vref_ctrl,                           0x00103200);
        WRITE_PARAM_ALL_REG(table, emc_training_settle,                                 0x07070404);
        // WRITE_PARAM_ALL_REG(table, emc_training_mpc,                                 0x00000000);

        const u32 mc_tRCD  = (int)  ((double)     (GET_CYCLE(tRCD)  >> 2) - 2.0);
        const u32 mc_tRPpb = (int)  (((double)    (GET_CYCLE(tRPpb) >> 2) - 1.0) + 2.0);
        const u32 mc_tRC   = (uint) ((double)     (GET_CYCLE(tRC)   >> 2) - 1.0);
        const u32 mc_tR2W  = (uint) (((double)    ((uint)tR2W       >> 2) - 1.0) + 2.0);
        const u32 mc_tW2R  = (uint) (((double)    (tW2R >> 2)             - 1.0) + 2.0);
        const u32 mc_tRAS  = MIN(GET_CYCLE(tRAS), (u32) 0x7F);
        const u32 mc_tRRD  = MIN(GET_CYCLE(tRRD), (u32)   31);

        table->burst_mc_regs.mc_emem_arb_timing_ras = (int)  ((double)  (mc_tRAS          >> 2) - 2.0);
        table->burst_mc_regs.mc_emem_arb_timing_rcd = (int)  ((double)  (GET_CYCLE(tRCD)  >> 2) - 2.0);
        table->burst_mc_regs.mc_emem_arb_timing_rp  = (int)  (((double) (GET_CYCLE(tRPpb) >> 2) - 1.0) + 2.0);
        table->burst_mc_regs.mc_emem_arb_timing_rc  = (int)  ((double)  (GET_CYCLE(tRC)   >> 2) - 1.0);
        table->burst_mc_regs.mc_emem_arb_timing_faw = (int)  ((double)  (GET_CYCLE(tFAW)  >> 2) - 1.0);
        table->burst_mc_regs.mc_emem_arb_timing_rrd = (int)  ((double)  (mc_tRRD          >> 2) - 1.0);
        table->burst_mc_regs.mc_emem_arb_timing_r2w = (uint) (((double) ((uint) tR2W      >> 2) - 1.0) + 2.0);
        table->burst_mc_regs.mc_emem_arb_timing_w2r = (uint) (((double) (tW2R             >> 2) - 1.0) + 2.0);

        table->burst_mc_regs.mc_emem_arb_da_turns     = (val & 0x0000FFFF) | (tW2R << 24) | (tR2W << 16);
        table->burst_mc_regs.mc_emem_arb_da_covers    = (((uint)                    (mc_tRCD + 3 + mc_tRPpb) >> 1 & 0xff)    << 8) | (((uint) (mc_tRCD + 11 + mc_tRPpb) >> 1 & 0xff) << 0x10) | ((mc_tRC >> 1) & 0xff);
        table->burst_mc_regs.mc_emem_arb_misc0        = (table->burst_mc_regs.mc_emem_arb_misc0 & 0xffe08000U) | ((mc_tRC + 1) & 0xff); /* Missing in l4t dump? TODO */
        table->burst_mc_regs.mc_emem_arb_timing_rfcpb = GET_CYCLE(tRFCpb) >> 2;

        table->burst_mc_regs.mc_emem_arb_cfg                  = 0x0000000c;
        // table->burst_mc_regs.mc_emem_arb_timing_rcd        = 0x00000006;
        // table->burst_mc_regs.mc_emem_arb_timing_rp         = 0x00000007;
        // table->burst_mc_regs.mc_emem_arb_timing_rc         = 0x00000018;
        // table->burst_mc_regs.mc_emem_arb_timing_ras        = 0x0000000f;
        // table->burst_mc_regs.mc_emem_arb_timing_faw        = 0x0000000f;
        // table->burst_mc_regs.mc_emem_arb_timing_rrd        = 0x00000003;
        table->burst_mc_regs.mc_emem_arb_timing_rap2pre       = 0x00000003;
        table->burst_mc_regs.mc_emem_arb_timing_wap2pre       = 0x0000000d;
        table->burst_mc_regs.mc_emem_arb_timing_r2r           = 0x00000007;
        table->burst_mc_regs.mc_emem_arb_timing_w2w           = 0x00000007;
        // table->burst_mc_regs.mc_emem_arb_timing_r2w        = 0x0000000c;
        // table->burst_mc_regs.mc_emem_arb_timing_w2r        = 0x0000000a;
        // table->burst_mc_regs.mc_emem_arb_da_turns          = 0x05060303;
        // table->burst_mc_regs.mc_emem_arb_da_covers         = 0x000d080c;
        table->burst_mc_regs.mc_emem_arb_ring1_throttle       = 0x001f0000;
        // table->burst_mc_regs.mc_emem_arb_timing_rfcpb      = 0x00000023;
        table->burst_mc_regs.mc_emem_arb_timing_ccdmw         = 0x00000008;
        table->burst_mc_regs.mc_emem_arb_refpb_hp_ctrl        = 0x000a1020;
        table->burst_mc_regs.mc_emem_arb_refpb_bank_ctrl      = 0x80001028;
        // table->burst_mc_regs.mc_emem_arb_dhyst_ctrl        = 0x00000002;
        table->burst_mc_regs.mc_emem_arb_dhyst_timeout_util_0 = 0x0000001a;
        table->burst_mc_regs.mc_emem_arb_dhyst_timeout_util_1 = 0x0000001a;
        table->burst_mc_regs.mc_emem_arb_dhyst_timeout_util_2 = 0x0000001a;
        table->burst_mc_regs.mc_emem_arb_dhyst_timeout_util_3 = 0x0000001a;
        table->burst_mc_regs.mc_emem_arb_dhyst_timeout_util_4 = 0x0000001a;
        table->burst_mc_regs.mc_emem_arb_dhyst_timeout_util_5 = 0x0000001a;
        table->burst_mc_regs.mc_emem_arb_dhyst_timeout_util_6 = 0x0000001a;
        table->burst_mc_regs.mc_emem_arb_dhyst_timeout_util_7 = 0x0000001a;
        table->la_scale_regs.mc_mll_mpcorer_ptsa_rate         = 0x000000d0;
        table->la_scale_regs.mc_ftop_ptsa_rate                = 0x00000018;
        table->la_scale_regs.mc_ptsa_grant_decrement          = 0x00001203;
        table->la_scale_regs.mc_latency_allowance_avpc_0      = 0x00800004;
        table->la_scale_regs.mc_latency_allowance_xusb_1      = 0x00800038;
        table->la_scale_regs.mc_latency_allowance_sdmmcaa_0   = 0x00800005;
        table->la_scale_regs.mc_latency_allowance_sdmmca_0    = 0x00800014;
        table->la_scale_regs.mc_latency_allowance_isp2_0      = 0x0000002c;
        table->la_scale_regs.mc_latency_allowance_isp2_1      = 0x00800080;
        table->la_scale_regs.mc_latency_allowance_vic_0       = 0x0080001d;
        table->la_scale_regs.mc_latency_allowance_nvdec_0     = 0x00800095;
        table->la_scale_regs.mc_latency_allowance_tsec_0      = 0x00800041;
        table->la_scale_regs.mc_latency_allowance_ppcs_1      = 0x00800080;
        table->la_scale_regs.mc_latency_allowance_xusb_0      = 0x0080003d;
        table->la_scale_regs.mc_latency_allowance_ppcs_0      = 0x00340049;
        table->la_scale_regs.mc_latency_allowance_gpu2_0      = 0x00800019;
        table->la_scale_regs.mc_latency_allowance_hc_1        = 0x00000080;
        table->la_scale_regs.mc_latency_allowance_sdmmc_0     = 0x00800090;
        table->la_scale_regs.mc_latency_allowance_mpcore_0    = 0x00800004;
        table->la_scale_regs.mc_latency_allowance_vi2_0       = 0x00000080;
        table->la_scale_regs.mc_latency_allowance_hc_0        = 0x00080016;
        table->la_scale_regs.mc_latency_allowance_gpu_0       = 0x00800019;
        table->la_scale_regs.mc_latency_allowance_sdmmcab_0   = 0x00800005;
        table->la_scale_regs.mc_latency_allowance_nvenc_0     = 0x00800018;
        table->dram_timings.t_rp                              =     tRFCpb;
        table->dram_timings.t_rfc                             =     tRFCab;
    }

    /* These timings are slightly off from eos, I am not sure why but I am going to figure it out at some point. */
    void MemMtcTableAutoAdjust(EristaMtcTable *table) {
        if (C.mtcConf != AUTO_ADJ) /* Return even needed? */
            return;

        using namespace pcv::erista;

        #define WRITE_PARAM_ALL_REG(TABLE, PARAM, VALUE) \
            TABLE->burst_regs.PARAM = VALUE;             \
            TABLE->shadow_regs_ca_train.PARAM = VALUE;   \
            TABLE->shadow_regs_quse_train.PARAM = VALUE; \
            TABLE->shadow_regs_rdwr_train.PARAM = VALUE;

        #define GET_CYCLE(PARAM) ((u32)((double)(PARAM) / tCK_avg))

        /* This condition is insane but it's done in eos. */
        /* Need to clean up at some point. */
        u32 rext;
        u32 wext;
        if (C.eristaEmcMaxClock < 3200001) {
            if (C.eristaEmcMaxClock < 2133001) {
                rext = 26;
                wext = 22;
            } else {
                rext = 28;
                wext = 22;

                if (2400000 < C.eristaEmcMaxClock) {
                    wext = 25;
                }
            }
        } else {
            rext = 30;
            wext = 25;
        }

        u32 refresh_raw = 0xFFFF;
        u32 trefbw = 0;

        if (C.t8_tREFI != 6) {
            refresh_raw = static_cast<u32>(std::floor(static_cast<double>(tREFpb_values[C.t8_tREFI]) / tCK_avg)) - 0x40;
            refresh_raw = MIN(refresh_raw, static_cast<u32>(0xFFFF));
        }

        trefbw = refresh_raw + 0x40;
        trefbw = MIN(trefbw, static_cast<u32>(0x3FFF));

        /* Primary timings. */
        WRITE_PARAM_ALL_REG(table, emc_rd_rcd, GET_CYCLE(tRCD));
        WRITE_PARAM_ALL_REG(table, emc_wr_rcd, GET_CYCLE(tRCD));
        WRITE_PARAM_ALL_REG(table, emc_ras,    GET_CYCLE(tRAS));
        WRITE_PARAM_ALL_REG(table, emc_rp,     GET_CYCLE(tRPpb));

        /* Secondary timings. */
        WRITE_PARAM_ALL_REG(table, emc_rrd,     GET_CYCLE(tRRD));
        WRITE_PARAM_ALL_REG(table, emc_rfc,     GET_CYCLE(tRFCab));
        WRITE_PARAM_ALL_REG(table, emc_rfcpb,   GET_CYCLE(tRFCpb));
        WRITE_PARAM_ALL_REG(table, emc_r2w,     tR2W);
        WRITE_PARAM_ALL_REG(table, emc_w2r,     tW2R);
        WRITE_PARAM_ALL_REG(table, emc_r2p,   (u32)  0xC);
        WRITE_PARAM_ALL_REG(table, emc_w2p,     (u32) 0x2D);

        WRITE_PARAM_ALL_REG(table, emc_rext, rext);
        WRITE_PARAM_ALL_REG(table, emc_wext, wext);

        WRITE_PARAM_ALL_REG(table, emc_trpab,  GET_CYCLE(tRPab));
        WRITE_PARAM_ALL_REG(table, emc_tfaw,   GET_CYCLE(tFAW));
        WRITE_PARAM_ALL_REG(table, emc_rc,     GET_CYCLE(tRC));

        WRITE_PARAM_ALL_REG(table, emc_tckesr,   GET_CYCLE(tSR));
        WRITE_PARAM_ALL_REG(table, emc_tcke,     GET_CYCLE(tXP) + 1);
        WRITE_PARAM_ALL_REG(table, emc_tpd,      GET_CYCLE(tXP));
        WRITE_PARAM_ALL_REG(table, emc_tclkstop, GET_CYCLE(tXP) + 8);

        WRITE_PARAM_ALL_REG(table, emc_txsr,    MIN(GET_CYCLE(tXSR), (u32) 1022));
        WRITE_PARAM_ALL_REG(table, emc_txsrdll, MIN(GET_CYCLE(tXSR), (u32) 1022));

        const u32 dyn_self_ref_control = (((u32)(7605.0 / tCK_avg)) + 260U) | (table->burst_regs.emc_dyn_self_ref_control & 0xffff0000U);
        WRITE_PARAM_ALL_REG(table, emc_dyn_self_ref_control, dyn_self_ref_control);

        WRITE_PARAM_ALL_REG(table, emc_rw2pden, tRW2PDEN);
        WRITE_PARAM_ALL_REG(table, emc_pdex2wr, GET_CYCLE(10.0));
        WRITE_PARAM_ALL_REG(table, emc_pdex2rd, GET_CYCLE(10.0));

        WRITE_PARAM_ALL_REG(table, emc_pchg2pden, GET_CYCLE(1.75));
        WRITE_PARAM_ALL_REG(table, emc_ar2pden,   GET_CYCLE(1.75));
        WRITE_PARAM_ALL_REG(table, emc_pdex2cke,  GET_CYCLE(1.75));
        WRITE_PARAM_ALL_REG(table, emc_act2pden,  GET_CYCLE(14.0));
        WRITE_PARAM_ALL_REG(table, emc_cke2pden,  GET_CYCLE(5.0));
        WRITE_PARAM_ALL_REG(table, emc_pdex2mrr,  GET_CYCLE(pdex2mrr));

        WRITE_PARAM_ALL_REG(table, emc_refresh,                    refresh_raw);
        WRITE_PARAM_ALL_REG(table, emc_pre_refresh_req_cnt, (u32) (refresh_raw / 4));
        WRITE_PARAM_ALL_REG(table, emc_trefbw,                     trefbw);

        const u32 mc_tRCD = (int)((double)(GET_CYCLE(tRCD) >> 2) - 2.0);
        const u32 mc_tRPpb = (int)(((double)(GET_CYCLE(tRPpb) >> 2) - 1.0) + 2.0);
        const u32 mc_tRC = (uint)((double)(GET_CYCLE(tRC) >> 2) - 1.0);
        const u32 mc_tR2W = (uint)(((double)((uint)tR2W >> 2) - 1.0) + 2.0);
        const u32 mc_tW2R = (uint)(((double)(tW2R >> 2) - 1.0) + 2.0);
        const u32 mc_tRAS = MIN(GET_CYCLE(tRAS), (u32) 0x7F);
        const u32 mc_tRRD = MIN(GET_CYCLE(tRRD), (u32) 31);

        table->burst_mc_regs.mc_emem_arb_cfg = (int)(((double) C.eristaEmcMaxClock / 33300.0) * 0.25);
        table->burst_mc_regs.mc_emem_arb_timing_ras = (int) ((double) (mc_tRAS >> 2) - 2.0);
        table->burst_mc_regs.mc_emem_arb_timing_rcd = (int) ((double) (GET_CYCLE(tRCD) >> 2) - 2.0);
        table->burst_mc_regs.mc_emem_arb_timing_rp = (int) (((double) (GET_CYCLE(tRPpb) >> 2) - 1.0) + 2.0);
        table->burst_mc_regs.mc_emem_arb_timing_rc = (int) ((double) (GET_CYCLE(tRC) >> 2) - 1.0);
        table->burst_mc_regs.mc_emem_arb_timing_faw = (int) ((double)(GET_CYCLE(tFAW) >> 2) - 1.0);
        table->burst_mc_regs.mc_emem_arb_timing_rrd = (int)((double)(mc_tRRD >> 2) - 1.0);
        table->burst_mc_regs.mc_emem_arb_timing_rap2pre = 3;
        table->burst_mc_regs.mc_emem_arb_timing_wap2pre = 11;
        table->burst_mc_regs.mc_emem_arb_timing_r2w = (uint)(((double)((uint)tR2W >> 2) - 1.0) + 2.0);
        table->burst_mc_regs.mc_emem_arb_timing_w2r = (uint)(((double)(tW2R >> 2) - 1.0) + 2.0);

        u32 mc_r2r = table->burst_mc_regs.mc_emem_arb_timing_r2r;
        if (mc_r2r > 1) {
            mc_r2r = (uint)(((double)(long)((double)rext * 0.25) - 1.0) + 2.0);
            table->burst_mc_regs.mc_emem_arb_timing_r2r = mc_r2r;
        }

        u32 mc_w2w = table->burst_mc_regs.mc_emem_arb_timing_w2w;
        if (mc_w2w > 1) {
            mc_w2w = (uint)(((double)(long)((double)wext / 4.0) - 1.0) + 2.0);
            table->burst_mc_regs.mc_emem_arb_timing_w2w = mc_w2w;
        }

        table->burst_mc_regs.mc_emem_arb_da_turns = ((mc_tW2R >> 1) << 0x18) | ((mc_tR2W >> 1) << 0x10) | ((mc_r2r >> 1) << 8) | ((mc_w2w >> 1));
        table->burst_mc_regs.mc_emem_arb_da_covers = (((uint)(mc_tRCD + 3 + mc_tRPpb) >> 1 & 0xff) << 8) | (((uint)(mc_tRCD + 11 + mc_tRPpb) >> 1 & 0xff) << 0x10) | ((mc_tRC >> 1) & 0xff);
        table->burst_mc_regs.mc_emem_arb_misc0 = (table->burst_mc_regs.mc_emem_arb_misc0 & 0xffe08000U) | ((mc_tRC + 1) & 0xff);
        table->la_scale_regs.mc_mll_mpcorer_ptsa_rate =         MIN((u32)((C.eristaEmcMaxClock / 1600000) * 0xd0U), (u32)0x115);
        table->la_scale_regs.mc_ftop_ptsa_rate =         MIN((u32)((C.eristaEmcMaxClock / 1600000) * 0x18U), (u32)0x1f);
        table->la_scale_regs.mc_ptsa_grant_decrement =         MIN((u32)((C.eristaEmcMaxClock / 1600000) * 0x1203U), (u32)0x17ff);

        u32 mc_latency_allowance = 0;
        if (C.eristaEmcMaxClock / 1000 != 0) {
            mc_latency_allowance = 204800 / (C.eristaEmcMaxClock / 1000);
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
        if (C.eristaEmcMaxClock / 1000 != 0) {
            mc_latency_allowance_hc_0 = 35200 / (C.eristaEmcMaxClock / 1000);
        }

        table->la_scale_regs.mc_latency_allowance_nvdec_0 = (table->la_scale_regs.mc_latency_allowance_nvdec_0 & 0xff00ffffU) | mc_latency_allowance3;
        table->la_scale_regs.mc_latency_allowance_hc_0 = (table->la_scale_regs.mc_latency_allowance_hc_0 & 0xffffff00U) | mc_latency_allowance_hc_0;

        table->la_scale_regs.mc_latency_allowance_isp2_1 = (table->la_scale_regs.mc_latency_allowance_isp2_1 & 0xff00ff00U) | mc_latency_allowance3 | mc_latency_allowance2;
        table->la_scale_regs.mc_latency_allowance_hc_1 = (table->la_scale_regs.mc_latency_allowance_hc_1 & 0xffffff00U) | mc_latency_allowance2;

        u32 mc_latency_allowance_gpu_0 = 0;
        if (C.eristaEmcMaxClock / 1000 != 0) {
            mc_latency_allowance_gpu_0 = 40000 / (C.eristaEmcMaxClock / 1000);
        }

        table->la_scale_regs.mc_latency_allowance_gpu_0 = ((mc_latency_allowance_gpu_0 | table->la_scale_regs.mc_latency_allowance_gpu_0) & 0xff00ff00U) | mc_latency_allowance3;

        u32 mc_latency_allowance_gpu2_0 = 0;
        if (C.eristaEmcMaxClock / 1000 != 0) {
        mc_latency_allowance_gpu2_0 = 40000 / (C.eristaEmcMaxClock / 1000);
        }

        table->la_scale_regs.mc_latency_allowance_gpu2_0 = ((mc_latency_allowance_gpu2_0 | table->la_scale_regs.mc_latency_allowance_gpu2_0) & 0xff00ff00U) | mc_latency_allowance3;

        u32 mc_latency_allowance_nvenc_0 = 0;
        if (C.eristaEmcMaxClock / 1000 != 0) {
            mc_latency_allowance_nvenc_0 = 38400 / (C.eristaEmcMaxClock / 1000);
        }

        table->la_scale_regs.mc_latency_allowance_nvenc_0 = ((mc_latency_allowance_nvenc_0 | table->la_scale_regs.mc_latency_allowance_nvenc_0) & 0xff00ff00U) | mc_latency_allowance3;

        u32 mc_latency_allowance_vic_0 = 0;
        if (C.eristaEmcMaxClock / 1000 != 0) {
            mc_latency_allowance_vic_0 = 0xb540 / (C.eristaEmcMaxClock / 1000);
        }

        table->la_scale_regs.mc_latency_allowance_vic_0 = ((mc_latency_allowance_vic_0 | table->la_scale_regs.mc_latency_allowance_vic_0) & 0xff00ff00U) | mc_latency_allowance3;
        table->la_scale_regs.mc_latency_allowance_vi2_0 = (table->la_scale_regs.mc_latency_allowance_vi2_0 & 0xffffff00U) | mc_latency_allowance2;

        table->burst_mc_regs.mc_emem_arb_timing_rfcpb = GET_CYCLE(tRFCpb) >> 2;

        if (C.hpMode) {
            WRITE_PARAM_ALL_REG(table, emc_cfg, 0x13200000);
        }

        table->dram_timings.t_rp = tRFCpb;
        table->dram_timings.t_rfc = tRFCab;
        table->emc_cfg_2 = 0x11083d;
        #undef GET_CYCLE
    }

    Result MemFreqMtcTable(u32 *ptr) {
        u32 khz_list[] = {1600000, 1331200, 1065600, 800000, 665600, 408000, 204000, 102000, 68000, 40800};
        u32 khz_list_size = sizeof(khz_list) / sizeof(u32);

        // Generate list for mtc table pointers
        EristaMtcTable *table_list[khz_list_size];
        for (u32 i = 0; i < khz_list_size; i++) {
            u8 *table = reinterpret_cast<u8 *>(ptr) - offsetof(EristaMtcTable, rate_khz) - i * sizeof(EristaMtcTable);
            table_list[i] = reinterpret_cast<EristaMtcTable *>(table);
            R_UNLESS(table_list[i]->rate_khz == khz_list[i], ldr::ResultInvalidMtcTable());
            R_UNLESS(table_list[i]->rev == MTC_TABLE_REV, ldr::ResultInvalidMtcTable());
        }

        if (C.eristaEmcMaxClock <= EmcClkOSLimit)
            R_SKIP();

        // Make room for new mtc table, discarding useless 40.8 MHz table
        // 40800 overwritten by 68000, ..., 1331200 overwritten by 1600000, leaving table_list[0] not overwritten
        for (u32 i = khz_list_size - 1; i > 0; i--)
            std::memcpy(static_cast<void *>(table_list[i]), static_cast<void *>(table_list[i - 1]), sizeof(EristaMtcTable));

        if (C.mtcConf == AUTO_ADJ) {
            MemMtcTableAutoAdjust(table_list[0]);
        } else {
            MemMtcTableAutoAdjustBaseLatency(table_list[0]);
        }

        PATCH_OFFSET(ptr, C.eristaEmcMaxClock);
        R_SUCCEED();
    }

    Result MemFreqMax(u32 *ptr) {
        if (C.eristaEmcMaxClock <= EmcClkOSLimit)
            R_SKIP();

        PATCH_OFFSET(ptr, C.eristaEmcMaxClock);

        R_SUCCEED();
    }

    void Patch(uintptr_t mapped_nso, size_t nso_size) {
        u32 CpuCvbDefaultMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(CpuCvbTableDefault)->freq);
        u32 GpuCvbDefaultMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(GpuCvbTableDefault)->freq);

        PatcherEntry<u32> patches[] = {
            {"CPU Freq Vdd",   &CpuFreqVdd,            1, nullptr, CpuClkOSLimit },
            {"CPU Freq Table", CpuFreqCvbTable<false>, 1, nullptr, CpuCvbDefaultMaxFreq},
            {"CPU Volt Limit", &CpuVoltRange,         13, nullptr, CpuVoltOfficial },
            {"CPU Volt Dfll",  &CpuVoltDfll,           1, nullptr, 0xFFEAD0FF },
            {"GPU Freq Table", GpuFreqCvbTable<false>, 1, nullptr, GpuCvbDefaultMaxFreq},
            {"GPU Freq Asm", &GpuFreqMaxAsm, 2, &GpuMaxClockPatternFn},
            {"GPU Freq PLL", &GpuFreqPllLimit, 1, nullptr, GpuClkPllLimit},
            {"MEM Freq Mtc", &MemFreqMtcTable, 0, nullptr, EmcClkOSLimit},
            {"MEM Freq Max", &MemFreqMax, 0, nullptr, EmcClkOSLimit},
            {"MEM Freq PLLM", &MemFreqPllmLimit, 2, nullptr, EmcClkPllmLimit},
            {"MEM Volt", &MemVoltHandler, 2, nullptr, MemVoltHOS},
            {"GPU Vmin", &GpuVmin, 0, nullptr, gpuVmin},
        };

        for (uintptr_t ptr = mapped_nso;
            ptr <= mapped_nso + nso_size - sizeof(EristaMtcTable);
            ptr += sizeof(u32)) {
            u32 *ptr32 = reinterpret_cast<u32 *>(ptr);
            for (auto &entry : patches) {
                if (R_SUCCEEDED(entry.SearchAndApply(ptr32)))
                    break;
            }
        }

        for (auto &entry : patches) {
            LOGGING("%s Count: %zu", entry.description, entry.patched_count);
            if (R_FAILED(entry.CheckResult()))
                CRASH(entry.description);
        }
    }
}
