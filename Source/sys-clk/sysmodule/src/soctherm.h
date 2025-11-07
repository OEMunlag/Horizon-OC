/* * HOS soctherm driver by Souldbminer & Dominatorul * Licensed under the LGPLv3 */
#ifndef _SOCTHERM_H_
#define _SOCTHERM_H_

#include <switch.h>

#define SOCTHERM_BASE 0x700E2000ULL

#define SENSOR_TEMP1 0x1c8
#define SENSOR_TEMP2 0x1cc

#define SOC_THERM_THERMCTL_LEVEL0_GROUP_CPU_0 0x0
#define SOC_THERM_THERMCTL_LEVEL0_GROUP_GPU_0 0x4
#define SOC_THERM_THERMCTL_LEVEL0_GROUP_MEM_0 0x8
#define SOC_THERM_THERMCTL_LEVEL0_GROUP_TSENSE_0 0xC

#define SENSOR_CPU0_CONFIG0  0xC0
#define SENSOR_CPU1_CONFIG0  0xE0
#define SENSOR_CPU2_CONFIG0  0x100
#define SENSOR_CPU3_CONFIG0  0x120
#define SENSOR_GPU_CONFIG0   0x180
#define SENSOR_PLLX_CONFIG0  0x1A0

#define SENSOR_CPU0_CONFIG1  0xC4
#define SENSOR_CPU1_CONFIG1  0xE4
#define SENSOR_CPU2_CONFIG1  0x104
#define SENSOR_CPU3_CONFIG1  0x124
#define SENSOR_GPU_CONFIG1   0x184
#define SENSOR_PLLX_CONFIG1  0x1A4

#define SENSOR_CPU0_STATUS   0xC8
#define SENSOR_GPU_STATUS    0x188
#define SENSOR_PLLX_STATUS   0x1A8

#define SENSOR_CONFIG0_STOP_MASK      (1U << 0)
#define SENSOR_CONFIG0_TALL_SHIFT     8
#define SENSOR_CONFIG0_TALL_MASK      (0xFFFFF << 8)

#define SENSOR_CONFIG1_TEMP_ENABLE_MASK   (1U << 31)
#define SENSOR_CONFIG1_TEN_COUNT_SHIFT    24
#define SENSOR_CONFIG1_TEN_COUNT_MASK     (0x3F << 24)
#define SENSOR_CONFIG1_TIDDQ_EN_SHIFT     15
#define SENSOR_CONFIG1_TIDDQ_EN_MASK      (0x3F << 15)
#define SENSOR_CONFIG1_TSAMPLE_SHIFT      0
#define SENSOR_CONFIG1_TSAMPLE_MASK       0x3FF

#define SENSOR_STATUS_VALID_MASK          (1U << 31)

#define TSENSOR_TALL_DEFAULT      16300
#define TSENSOR_TIDDQ_EN_DEFAULT  1
#define TSENSOR_TEN_COUNT_DEFAULT 1
#define TSENSOR_TSAMPLE_DEFAULT   120

#define SENSOR_TEMP1_CPU_TEMP_MASK  (0xFFFF << 16)
#define SENSOR_TEMP1_GPU_TEMP_MASK  0xFFFF
#define SENSOR_TEMP2_PLLX_TEMP_MASK 0xFFFF

#define READBACK_VALUE_MASK   0xFF00
#define READBACK_VALUE_SHIFT  8
#define READBACK_ADD_HALF     (1 << 7)
#define READBACK_NEGATE       (1 << 0)

#define REG_GET_MASK(r, m) (((r) & (m)) >> (__builtin_ffs(m) - 1))

// Timing constants (in microseconds)
#define SENSOR_STABILIZATION_DELAY_US  2000  // 2ms for sensor to stabilize
#define SENSOR_READ_DELAY_US           100   // 100us between config operations

// Makes my life easier
#define BITMASK(bits) (0UL | (bits))
#define BIT(n)        (1UL << (n))

#define WRITE_REG_BIT(addr, bit, val) do {              \
    u32 _tmp;                                           \
    svcReadWriteRegister(&_tmp, (addr), 0, false);      \
    if (val)                                            \
        _tmp |=  (1U << (bit)); /* set bit */           \
    else                                                \
        _tmp &= ~(1U << (bit)); /* clear bit */         \
    svcReadWriteRegister(&_tmp, (addr), 0, true);       \
} while (0)

typedef enum {
    SENSOR_CPU  = 0,
    SENSOR_GPU  = 1,
    SENSOR_PLLX = 2,
} SocthermSensor;

void socthermInit(void) {
    WRITE_REG_BIT(SOCTHERM_BASE + SENSOR_CPU0_CONFIG0, 0, 0); // start cpu0
    WRITE_REG_BIT(SOCTHERM_BASE + SENSOR_CPU0_CONFIG1, 31, 1); // start cpu0

    WRITE_REG_BIT(SOCTHERM_BASE + SENSOR_CPU1_CONFIG0, 0, 0); // start cpu1
    WRITE_REG_BIT(SOCTHERM_BASE + SENSOR_CPU1_CONFIG1, 31, 1); // start cpu1

    WRITE_REG_BIT(SOCTHERM_BASE + SENSOR_CPU2_CONFIG0, 0, 0); // start cpu2
    WRITE_REG_BIT(SOCTHERM_BASE + SENSOR_CPU2_CONFIG1, 31, 1); // start cpu2

    WRITE_REG_BIT(SOCTHERM_BASE + SENSOR_CPU3_CONFIG0, 0, 0); // start cpu3
    WRITE_REG_BIT(SOCTHERM_BASE + SENSOR_CPU3_CONFIG1, 31, 1); // start cpu3

    WRITE_REG_BIT(SOCTHERM_BASE + SENSOR_GPU_CONFIG0, 0, 0); // start gpu
    WRITE_REG_BIT(SOCTHERM_BASE + SENSOR_GPU_CONFIG1, 31, 1); // start gpu

    WRITE_REG_BIT(SOCTHERM_BASE + SENSOR_PLLX_CONFIG0, 0, 0); // start pllx
    WRITE_REG_BIT(SOCTHERM_BASE + SENSOR_PLLX_CONFIG1, 31, 1); // start pllx
}

int socthermRead(SocthermSensor sensor) {
    switch(sensor) {
        case SENSOR_CPU: {
            u32 temp_reg;
            svcReadWriteRegister(&temp_reg, SOCTHERM_BASE + SENSOR_TEMP1, 0, false);
            return REG_GET_MASK(temp_reg, SENSOR_TEMP1_CPU_TEMP_MASK);
        }
        case SENSOR_GPU: {
            u32 temp_reg;
            svcReadWriteRegister(&temp_reg, SOCTHERM_BASE + SENSOR_TEMP1, 0, false);
            return REG_GET_MASK(temp_reg, SENSOR_TEMP1_GPU_TEMP_MASK);
        }
        case SENSOR_PLLX: {
            u32 temp_reg;
            svcReadWriteRegister(&temp_reg, SOCTHERM_BASE + SENSOR_TEMP2, 0, false);
            return REG_GET_MASK(temp_reg, SENSOR_TEMP2_PLLX_TEMP_MASK);
        }
        default:
            return -1; // Invalid sensor
    }
}

#endif // _SOCTHERM_H_