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

    PATCH_OFFSET(ptr, GetDvfsTableLastEntry(C.eristaCpuDvfsTable)->freq);
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

    R_UNLESS(entry->tune0_low == 0x152f01,   ldr::ResultInvalidCpuVoltDfllEntry());
    R_UNLESS(entry->tune0_high == 0x00000000,    ldr::ResultInvalidCpuVoltDfllEntry());
    R_UNLESS(entry->tune1_low == 0x00000000,   ldr::ResultInvalidCpuVoltDfllEntry());
    R_UNLESS(entry->tune1_high == 0x00000000,    ldr::ResultInvalidCpuVoltDfllEntry());
    if(!C.eristaCpuUV) {
        R_SKIP();
    }
    PATCH_OFFSET(&(entry->dvco_calibration_max), 0x1C);
    PATCH_OFFSET(&(entry->tune1_high), 0x10);
    PATCH_OFFSET(&(entry->tune_high_margin_millivolts), 0xc);

    switch(C.eristaCpuUV) {
        case 1:
            PATCH_OFFSET(&(entry->tune0_high), 0xFFFF); //process_id 0 // EOS UV1
            PATCH_OFFSET(&(entry->tune1_high), 0x027007FF);
            break;
        case 2:
            PATCH_OFFSET(&(entry->tune0_high), 0x0000EFFF); //process_id 1 // EOS Uv2
            PATCH_OFFSET(&(entry->tune1_high), 0x027407FF);
            break;
        case 3:
            PATCH_OFFSET(&(entry->tune0_high), 0x0000DFFF); //process_id 0 // EOS UV3
            PATCH_OFFSET(&(entry->tune1_high), 0x027807FF);
            break;
        case 4:
            PATCH_OFFSET(&(entry->tune0_high), 0x0000DFDF); //process_id 1 // EOS Uv4
            PATCH_OFFSET(&(entry->tune1_high), 0x027A07FF);
            break;
        case 5:
            PATCH_OFFSET(&(entry->tune0_high), 0x0000CFDF); // EOS UV5
            PATCH_OFFSET(&(entry->tune1_high), 0x037007FF);
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
        case 3:
            max_clock = GetDvfsTableLastEntry(C.eristaGpuDvfsTableHigh)->freq;
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

void MemMtcTableAutoAdjust(EristaMtcTable *table) {
    if (C.mtcConf != AUTO_ADJ)
        return;

    using namespace pcv::erista;

    #define WRITE_PARAM_ALL_REG(TABLE, PARAM, VALUE) \
        TABLE->burst_regs.PARAM = VALUE;             \
        TABLE->shadow_regs_ca_train.PARAM = VALUE;   \
        TABLE->shadow_regs_quse_train.PARAM = VALUE; \
        TABLE->shadow_regs_rdwr_train.PARAM = VALUE;

    #define GET_CYCLE_CEIL(PARAM) u32(CEIL(double(PARAM) / tCK_avg))

    /* Primary timings. */
//    WRITE_PARAM_ALL_REG(table, emc_tckesr, GET_CYCLE_CEIL(tCK_avg));
    WRITE_PARAM_ALL_REG(table, emc_rd_rcd, GET_CYCLE_CEIL(tRCD));
    WRITE_PARAM_ALL_REG(table, emc_wr_rcd, GET_CYCLE_CEIL(tRCD));
    WRITE_PARAM_ALL_REG(table, emc_ras,    GET_CYCLE_CEIL(tRAS));
    WRITE_PARAM_ALL_REG(table, emc_rp,     GET_CYCLE_CEIL(tRPpb));

    /* Secondary timings. */
    WRITE_PARAM_ALL_REG(table, emc_rrd,    GET_CYCLE_CEIL(tRRD));
    WRITE_PARAM_ALL_REG(table, emc_rfcpb,  GET_CYCLE_CEIL(tRFCpb));
    WRITE_PARAM_ALL_REG(table, emc_r2w,    R2W);
    WRITE_PARAM_ALL_REG(table, emc_w2r,    W2R);
    WRITE_PARAM_ALL_REG(table, emc_trefbw, REFBW);

    WRITE_PARAM_ALL_REG(table, emc_rfc,    GET_CYCLE_CEIL(tRFCab));
    WRITE_PARAM_ALL_REG(table, emc_tppd,   tPPD);
    WRITE_PARAM_ALL_REG(table, emc_tfaw,   GET_CYCLE_CEIL(tFAW));
    WRITE_PARAM_ALL_REG(table, emc_rc,     GET_CYCLE_CEIL(tRC));
    WRITE_PARAM_ALL_REG(table, emc_tckesr, GET_CYCLE_CEIL(tSR));

    WRITE_PARAM_ALL_REG(table, emc_tcke,    MAX(4u, GET_CYCLE_CEIL(7.5)));
    WRITE_PARAM_ALL_REG(table, emc_txsr,    GET_CYCLE_CEIL(tXSR));
    WRITE_PARAM_ALL_REG(table, emc_r2p,     GET_CYCLE_CEIL(tRTP));
    WRITE_PARAM_ALL_REG(table, emc_w2p,     WTP);
    WRITE_PARAM_ALL_REG(table, emc_pdex2wr, GET_CYCLE_CEIL(tXP));
    WRITE_PARAM_ALL_REG(table, emc_pdex2rd, GET_CYCLE_CEIL(tXP));

    constexpr u32 MC_ARB_DIV = 4;
    constexpr u32 MC_ARB_SFA = 2;

    table->burst_mc_regs.mc_emem_arb_timing_rcd     = u32(CEIL(GET_CYCLE_CEIL(tRCD)  / double(MC_ARB_DIV))) - 2;
    table->burst_mc_regs.mc_emem_arb_timing_rp      = u32(CEIL(GET_CYCLE_CEIL(tRPpb) / double(MC_ARB_DIV))) - 1 + MC_ARB_SFA;
    table->burst_mc_regs.mc_emem_arb_timing_rc      = u32(CEIL(GET_CYCLE_CEIL(tRC)   / double(MC_ARB_DIV))) - 1;
    table->burst_mc_regs.mc_emem_arb_timing_ras     = u32(CEIL(GET_CYCLE_CEIL(tRAS)  / double(MC_ARB_DIV))) - 2;
    table->burst_mc_regs.mc_emem_arb_timing_faw     = u32(CEIL(GET_CYCLE_CEIL(tFAW)  / double(MC_ARB_DIV))) - 1;
    table->burst_mc_regs.mc_emem_arb_timing_rrd     = u32(CEIL(GET_CYCLE_CEIL(tRRD)  / double(MC_ARB_DIV))) - 1;
    table->burst_mc_regs.mc_emem_arb_timing_rap2pre = u32(CEIL(GET_CYCLE_CEIL(tRTP)  / double(MC_ARB_DIV)));
    table->burst_mc_regs.mc_emem_arb_timing_r2w     = u32(CEIL(R2W                   / double(MC_ARB_DIV)));
    table->burst_mc_regs.mc_emem_arb_timing_w2r     = u32(CEIL(W2R                   / double(MC_ARB_DIV)));
    #undef GET_CYCLE_CEIL
}

    Result MemFreqMtcTable(u32 *ptr) {
        if(C.eristaEmcMaxClock != EmcClkOSLimit) {
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
        
            MemMtcTableAutoAdjust(table_list[0]);
            PATCH_OFFSET(ptr, C.eristaEmcMaxClock);

            // Handle customize table replacement
            // if (C.mtcConf == CUSTOMIZED_ALL) {
            //    MemMtcCustomizeTable(table_list[0], const_cast<EristaMtcTable *>(C.eristaMtcTable));
            //}

            R_SUCCEED();
        } else {
            R_SUCCEED(); // Skip changing table on default freq
        }
    }

    Result MemFreqMax(u32 *ptr) {
        if (C.eristaEmcMaxClock <= EmcClkOSLimit)
            R_SKIP();

        PATCH_OFFSET(ptr, C.eristaEmcMaxClock);

        R_SUCCEED();
    }

    // Result MemFreqDvbTable(u32* ptr) {
    //     emc_dvb_dvfs_table_t* default_end  = reinterpret_cast<emc_dvb_dvfs_table_t *>(ptr);
    //     emc_dvb_dvfs_table_t* new_start = default_end + 1;
    
    //     // Validate existing table
    //     void* mem_dvb_table_head = reinterpret_cast<u8 *>(new_start) - sizeof(EmcDvbTableDefault);
    //     bool validated = std::memcmp(mem_dvb_table_head, EmcDvbTableDefault, sizeof(EmcDvbTableDefault)) == 0;
    //     R_UNLESS(validated, ldr::ResultInvalidDvbTable());
    
    //     if (C.eristaEmcMaxClock <= EmcClkOSLimit)
    //         R_SKIP();
    
    //     int32_t voltAdd = 25*C.EmcDvbShift;
    
    //     #define DVB_VOLT(zero, one, two)    std::min(zero+voltAdd, 1050), std::min(one+voltAdd, 1025), std::min(two+voltAdd, 1000),
    
    //     if (C.marikoEmcMaxClock < 1862400) {
    //         std::memcpy(new_start, default_end, sizeof(emc_dvb_dvfs_table_t));
    //     } else if (C.marikoEmcMaxClock < 2131200){
    //         emc_dvb_dvfs_table_t oc_table = { 1862400, { 950, 925, 900, } };
    //         std::memcpy(new_start, &oc_table, sizeof(emc_dvb_dvfs_table_t));
    //     } else if (C.marikoEmcMaxClock < 2227000){
    //         emc_dvb_dvfs_table_t oc_table = { 2131200, { 975, 950, 925, } };
    //         std::memcpy(new_start, &oc_table, sizeof(emc_dvb_dvfs_table_t));
    //     } else {
    //         emc_dvb_dvfs_table_t oc_table = { 2227000, { DVB_VOLT(1000, 975, 950) } };
    //         std::memcpy(new_start, &oc_table, sizeof(emc_dvb_dvfs_table_t));
    //     }
    //     new_start->freq = C.eristaEmcMaxClock;
    //     /* Max dvfs entry is 32, but HOS doesn't seem to boot if exact freq doesn't exist in dvb table,
    //        reason why it's like this 
    //     */ 
    
    //     R_SUCCEED();
    // }

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
            // {"MEM Freq Dvb", &MemFreqDvbTable, 1, nullptr, EmcClkOSLimit},
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
