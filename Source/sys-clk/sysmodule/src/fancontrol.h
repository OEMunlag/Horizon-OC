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
