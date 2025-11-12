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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/syslimits.h>
#include <switch.h>

#define LOG_DIR "./config/horizon-oc/"
#define LOG_FILE "./config/horizon-oc/fan_log.txt"
#define CONFIG_DIR "./config/horizon-oc/"
#define CONFIG_FILE "./config/horizon-oc/config.dat"
#define TABLE_SIZE sizeof(TemperaturePoint) * 10


typedef struct
{
    int     temperature_c;
    float   fanLevel_f;
} TemperaturePoint;

void WriteConfigFile(TemperaturePoint *table);
void ReadConfigFile(TemperaturePoint **table_out);

void InitFanController(TemperaturePoint *table);
void FanControllerThreadFunction(void*);
void StartFanControllerThread();
void CloseFanControllerThread();
void WaitFanController();
void WriteLog(char *buffer);

#ifdef __cplusplus
}
#endif
