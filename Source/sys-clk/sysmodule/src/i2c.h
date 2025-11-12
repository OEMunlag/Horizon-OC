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

#ifndef I2C_H
#define I2C_H

#include <switch.h>

Result I2cReadRegHandler16(u8 reg, I2cDevice dev, u16 *out)
{
	struct readReg {
        u8 send;
        u8 sendLength;
        u8 sendData;
        u8 receive;
        u8 receiveLength;
    };

	I2cSession _session;

	Result res = i2cOpenSession(&_session, dev);
	if (res)
		return res;

	u16 val;

    struct readReg readRegister = {
        .send = 0 | (I2cTransactionOption_Start << 6),
        .sendLength = sizeof(reg),
        .sendData = reg,
        .receive = 1 | (I2cTransactionOption_All << 6),
        .receiveLength = sizeof(val),
    };

	res = i2csessionExecuteCommandList(&_session, &val, sizeof(val), &readRegister, sizeof(readRegister));
	if (res)
	{
		i2csessionClose(&_session);
		return res;
	}

	*out = val;
	i2csessionClose(&_session);
	return 0;
}

Result I2cReadRegHandler8(u8 reg, I2cDevice dev, u8 *out)
{
	struct readReg {
        u8 send;
        u8 sendLength;
        u8 sendData;
        u8 receive;
        u8 receiveLength;
    };

	I2cSession _session;

	Result res = i2cOpenSession(&_session, dev);
	if (res)
		return res;

	u8 val;

    struct readReg readRegister = {
        .send = 0 | (I2cTransactionOption_Start << 6),
        .sendLength = sizeof(reg),
        .sendData = reg,
        .receive = 1 | (I2cTransactionOption_All << 6),
        .receiveLength = sizeof(val),
    };

	res = i2csessionExecuteCommandList(&_session, &val, sizeof(val), &readRegister, sizeof(readRegister));
	if (res)
	{
		i2csessionClose(&_session);
		return res;
	}

	*out = val;
	i2csessionClose(&_session);
	return 0;
}

#endif