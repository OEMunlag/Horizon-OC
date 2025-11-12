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

#include <cstdint>
#include <mutex>
#include <switch.h>

#include "config.h"
#include "errors.h"
#include "file_utils.h"
#include "pwm_dimming.h"
#define PWM_DIMMING_HZ_RATE 5e3 // 5KHz PWM dimming, should be decent for most people
#define DUTY_CYCLE_LEGNTH_NS (1e9 / (double)(PWM_DIMMING_HZ_RATE))

float initial_brightness = 1.0f;
float current_brightness = 1.0f;
Thread pwmThread;

PWMDimmer *PWMDimmer::instance = nullptr;

PWMDimmer *PWMDimmer::GetInstance()
{
    return instance;
}

void PWMDimmer::Initialize()
{
    if (!instance) {
        instance = new PWMDimmer();
        FileUtils::LogLine("[pwmDim] Initialized PWMDimmer");
    }
}

Config *PWMDimmer::GetConfig()
{
    return this->config;
}

void PWMDimmer::Exit()
{
    if (instance) {
        FileUtils::LogLine("[pwmDim] Exiting PWMDimmer");
        delete instance;
        instance = nullptr;
    }
}

PWMDimmer::PWMDimmer()
{
    this->config = Config::CreateDefault();
}

PWMDimmer::~PWMDimmer()
{
    delete this->config;
}

void PWMDimmer::Start()
{
    std::scoped_lock lock{ this->patcherMutex };
    this->config->Refresh();
    this->StartPWMDimming();

    u64 sku = 0;
    Result rc = splInitialize();
    ASSERT_RESULT_OK(rc, "splInitialize");

    rc = splGetConfig(SplConfigItem_HardwareType, &sku);
    ASSERT_RESULT_OK(rc, "splGetConfig");

    splExit();
    if (sku == HocClkConsoleType_OLED && this->config->GetConfigValue(HocClkConfigValue_PWMDimming)) {
        lblGetCurrentBrightnessSetting(&initial_brightness);
        Result rc =
        threadCreate(&pwmThread, ThreadEntry, this, NULL, 0x4000, 0x2B, -2);
        if (R_FAILED(rc)) {
            return;
        }

        rc = threadStart(&pwmThread);
        if (R_FAILED(rc)) {
            threadClose(&pwmThread);
            return;
        }
    }
}

void PWMDimmer::ThreadEntry(void *arg)
{
    auto *self = static_cast<PWMDimmer *>(arg);
    self->StartPWMDimming();
}

void PWMDimmer::StartPWMDimming()
{
    Result rc = lblInitialize();
    if (R_FAILED(rc)) {
        return;
    }
    for (;;) {
        lblGetCurrentBrightnessSetting(&current_brightness);
        lblSetCurrentBrightnessSetting(current_brightness);
        svcSleepThread(DUTY_CYCLE_LEGNTH_NS / 2);
        lblSetCurrentBrightnessSetting(0.0f);
        svcSleepThread(DUTY_CYCLE_LEGNTH_NS / 2);
    }
}
