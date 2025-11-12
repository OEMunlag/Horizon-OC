/*
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
 *
 */

#include <switch.h>
#include "emc_patcher.h"
#include "file_utils.h"
#include "board.h"
#include "i2c.h"
#include "maxXXXXX.h"

#define MC_BASE 0x70019000
#define EMC_BASE 0x7001B000
#define MC_EMC_BASE_SIZE 0x1000
#define EMC_TIMING_CONTROL_0 0x28
#define EMC_RAS_0 0x34
#define EMC_RAS_BIT_END 5

#define HOSSVC_HAS_MM (hosversionAtLeast(10,0,0))


EMCpatcher* EMCpatcher::instance = nullptr;

EMCpatcher* EMCpatcher::GetInstance()
{
    return instance;
}

void EMCpatcher::Initialize()
{
    if (!instance)
    {
        instance = new EMCpatcher();
        FileUtils::LogLine("[emc] Initialized EMCpatcher");
    }
}

Config *EMCpatcher::GetConfig()
{
    return this->config;
}

void EMCpatcher::Exit()
{
    if (instance)
    {
        FileUtils::LogLine("[emc] Exiting EMCpatcher");
        delete instance;
        instance = nullptr;
    }
}

EMCpatcher::EMCpatcher()
{
    this->config = Config::CreateDefault();
}



EMCpatcher::~EMCpatcher()
{
    delete this->config;
}

void EMCpatcher::Run()
{
    std::scoped_lock lock{this->patcherMutex};
    this->config->Refresh();
    // this->ApplyEMCPatch();
    this->I2CApplyVoltage();
}


void EMCpatcher::ApplyEMCPatch()
{
    // if(HOSSVC_HAS_MM) { // only for 10.0.0+, older versions need rewrites
    //     u64 mc_virt_addr = 0;
    //     u64 mc_out_size = 0;
    //     u64 emc_virt_addr = 0;
    //     u64 emc_out_size = 0;
    //     Result rc;

    //     rc = svcQueryMemoryMapping(&mc_virt_addr, &mc_out_size, MC_BASE, MC_EMC_BASE_SIZE); // map mc
    //     ASSERT_RESULT_OK(rc, "svcQueryMemoryMapping");
        
    //     rc = svcQueryMemoryMapping(&emc_virt_addr, &emc_out_size, EMC_BASE, MC_EMC_BASE_SIZE); // map emc
    //     ASSERT_RESULT_OK(rc, "svcQueryMemoryMapping");

    //     write_reg64(EMC_BASE, EMC_RAS_0, 1);

    //     write_reg64(EMC_BASE, EMC_TIMING_CONTROL_0, 0x1); // apply shadow regs


    //     svcUnmapMemory((void *)mc_virt_addr, (void *)MC_BASE, MC_EMC_BASE_SIZE); // clean up
    //     svcUnmapMemory((void *)emc_virt_addr, (void *)EMC_BASE, MC_EMC_BASE_SIZE);

    // }
}

#define EMC_MAX_VOLTAGE_SAFETY_CHECK 1350000
#define SD3_UV_MIN   600000
#define SD3_UV_STEP  12500
#define I2C_DEVICE(bus, addr) ((I2cDevice)(((bus) << 8) | (addr)))

#define I2C_5      5
#define MAX77620_I2C_ADDR  0x3C

#define I2cDevice_MAX77620  I2C_DEVICE(I2C_5, MAX77620_I2C_ADDR)

void EMCpatcher::I2CApplyVoltage()
{
    if(this->config->GetConfigValue(HocClkConfigValue_EMCVdd2VoltageuV) < EMC_MAX_VOLTAGE_SAFETY_CHECK) {
         set_sd1_voltage((u32)this->config->GetConfigValue(HocClkConfigValue_EMCVdd2VoltageuV));
     }
}

void EMCpatcher::set_sd1_voltage(uint32_t voltage_uv)
{
	// SD1 parameters
	const u32 uv_step = 12500;
	const u32 uv_min = 600000;
	const u32 uv_max = 1237500;
	const u8 volt_addr = 0x17;      // MAX77620_REG_SD1
	const u8 volt_mask = 0x7F;      // MAX77620_SD1_VOLT_MASK

	// Validate input voltage
	if (voltage_uv < uv_min || voltage_uv > uv_max)
		return;

	// Calculate voltage multiplier
	u32 mult = (voltage_uv + uv_step - 1 - uv_min) / uv_step;
	mult = mult & volt_mask;

	// Open I2C session to MAX77620 PMIC
	I2cSession session;
	Result res = i2cOpenSession(&session, I2cDevice_Max77620Pmic);
	if (R_FAILED(res)) {
		return;
	}

	// Read current register value
	u8 current_val = 0;
	res = i2csessionSendAuto(&session, &volt_addr, 1, I2cTransactionOption_Start);
	if (R_FAILED(res)) {
		i2csessionClose(&session);
		return;
	}

	res = i2csessionReceiveAuto(&session, &current_val, 1, I2cTransactionOption_Stop);
	if (R_FAILED(res)) {
		i2csessionClose(&session);
		return;
	}

	// Mask in the new voltage bits, preserving other bits
	u8 new_val = (current_val & ~volt_mask) | mult;

	// Write back register with START and STOP conditions
	u8 write_buf[2] = {volt_addr, new_val};
	res = i2csessionSendAuto(&session, write_buf, sizeof(write_buf), I2cTransactionOption_All);

	i2csessionClose(&session);
}