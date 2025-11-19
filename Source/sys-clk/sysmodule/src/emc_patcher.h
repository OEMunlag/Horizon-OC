#pragma once

#include <mutex>
#include <cstdint>
#include "config.h"
#include <array>
#include "errors.h"

// Read a 32-bit register via libnx SVC
static inline uint32_t read_reg64(uint64_t phys_addr, uint32_t offset) {
    uint32_t value = 0;
    Result rc = svcReadWriteRegister(&value, phys_addr + offset, 0, false);
    if (R_FAILED(rc)) {
        // Handle failure if needed
        return 0;
    }
    return value;
}

// Write a 32-bit register via libnx SVC
static inline void write_reg64(uint64_t phys_addr, uint32_t offset, uint32_t value) {
    Result rc = svcReadWriteRegister(NULL, phys_addr + offset, value, true);
    if (R_FAILED(rc)) {
        // Handle failure if needed
    }
}

// Bitfield helper remains the same
static inline uint32_t set_bits(uint32_t reg_value, uint8_t start_bit, uint8_t end_bit, uint32_t value)
{
    if (end_bit < start_bit || end_bit > 31)
        return reg_value;

    uint32_t mask = ((1u << (end_bit - start_bit + 1)) - 1u) << start_bit;
    reg_value = (reg_value & ~mask) | ((value << start_bit) & mask);
    return reg_value;
}

/* Primary timings. */
const std::array<double,  8> tRCD_values  =  {18, 17, 16, 15, 14, 13, 12, 11};
const std::array<double,  8> tRP_values   =  {18, 17, 16, 15, 14, 13, 12, 11};
const std::array<double, 10> tRAS_values  =  {42, 36, 34, 32, 30, 28, 26, 24, 22, 20};

/* Secondary timings. */
const std::array<double, 8>  tRRD_values   = {10.0, 7.5, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0};
const std::array<double, 6>  tRFC_values   = {140, 120, 100, 80, 60, 40};
const std::array<u32,    10>  tRTW_values   = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; /* Is this even correct? */
const std::array<double, 10>  tWTR_values   = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
const std::array<u32,    7>  tREFpb_values = {488, 732, 488 * 2, 488 * 3, 488 * 4, 488 * 6, 488 * 8}; /* TODO: Figure out if it's actually 8 and if this is even right. */

struct SOC_THERM_THERMCTL_LEVEL0_GROUP_CPU_0 {
    
};

class EMCpatcher
{
private:
    static EMCpatcher* instance;
    Config* config;
    std::mutex patcherMutex;

public:
    static EMCpatcher* GetInstance();
    static void Initialize();
    Config *GetConfig();
    static void Exit();

    EMCpatcher();
    ~EMCpatcher();

    void Run();
    void ApplyEMCPatch();
};
