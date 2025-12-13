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
 * 
 */
 

#include "misc_gui.h"
#include "fatal_gui.h"
#include "../format.h"
#include <cstdio>
#include <cstring>
#include <vector>

MiscGui::MiscGui()
{
    this->configList = new SysClkConfigValueList {};
}

MiscGui::~MiscGui()
{
    delete this->configList;
    this->configToggles.clear();
    this->configTrackbars.clear();
    this->configButtons.clear();
    this->configRanges.clear();
}

void MiscGui::addConfigToggle(SysClkConfigValue configVal, const char* altName) {
    const char* configName = altName ? altName : sysclkFormatConfigValue(configVal, true);
    tsl::elm::ToggleListItem* toggle = new tsl::elm::ToggleListItem(configName, this->configList->values[configVal]);
    toggle->setStateChangedListener([this, configVal](bool state) {
        this->configList->values[configVal] = uint64_t(state);
        Result rc = sysclkIpcSetConfigValues(this->configList);
        if (R_FAILED(rc))
            FatalGui::openWithResultCode("sysclkIpcSetConfigValues", rc);
        this->lastContextUpdate = armGetSystemTick();
    });
    this->listElement->addItem(toggle);
    this->configToggles[configVal] = toggle;
}

void MiscGui::addConfigButton(SysClkConfigValue configVal, 
    const char* altName, 
    const ValueRange& range,
    const std::string& categoryName,
    const ValueThresholds* thresholds,
    const std::map<uint32_t, std::string>& labels)
{
    const char* configName = altName ? altName : sysclkFormatConfigValue(configVal, true);

    tsl::elm::ListItem* listItem = new tsl::elm::ListItem(configName);

    uint64_t currentValue = this->configList->values[configVal];
    char valueText[32];
    if (currentValue == 0) {
        snprintf(valueText, sizeof(valueText), "%s", VALUE_DEFAULT_TEXT);
    } else {
        uint64_t displayValue = currentValue / range.divisor;
        if (!range.suffix.empty()) {
            snprintf(valueText, sizeof(valueText), "%lu %s", displayValue, range.suffix.c_str());
        } else {
            snprintf(valueText, sizeof(valueText), "%lu", displayValue);
        }
    }
    listItem->setValue(valueText);

    ValueThresholds thresholdsCopy = (thresholds ? *thresholds : ValueThresholds{});

    listItem->setClickListener(
        [this, configVal, range, categoryName, thresholdsCopy, labels](u64 keys)
        {
            if ((keys & HidNpadButton_A) == 0)
                return false;

            std::uint32_t currentValue = this->configList->values[configVal];

            if (thresholdsCopy.warning != 0 || thresholdsCopy.danger != 0) {

                tsl::changeTo<ValueChoiceGui>(
                    currentValue,
                    range,
                    categoryName,
                    [this, configVal](std::uint32_t value) {
                        this->configList->values[configVal] = value;
                        Result rc = sysclkIpcSetConfigValues(this->configList);
                        if (R_FAILED(rc)) {
                            FatalGui::openWithResultCode("sysclkIpcSetConfigValues", rc);
                            return false;
                        }
                        this->lastContextUpdate = armGetSystemTick();
                        return true;
                    },
                    thresholdsCopy,
                    true,
                    labels     // <── NEW
                );
            } else {

                tsl::changeTo<ValueChoiceGui>(
                    currentValue,
                    range,
                    categoryName,
                    [this, configVal](std::uint32_t value) {
                        this->configList->values[configVal] = value;
                        Result rc = sysclkIpcSetConfigValues(this->configList);
                        if (R_FAILED(rc)) {
                            FatalGui::openWithResultCode("sysclkIpcSetConfigValues", rc);
                            return false;
                        }
                        this->lastContextUpdate = armGetSystemTick();
                        return true;
                    },
                    ValueThresholds(),
                    false,
                    labels       // <── NEW
                );
            }

            return true;
        });

    this->listElement->addItem(listItem);
    this->configButtons[configVal] = listItem;
    this->configRanges[configVal] = range;  
}

void MiscGui::addFreqButton(SysClkConfigValue configVal,
                            const char* altName,
                            SysClkModule module,
                            const std::map<uint32_t, std::string>& labels)
{
    const char* configName = altName ? altName : sysclkFormatConfigValue(configVal, true);

    tsl::elm::ListItem* listItem = new tsl::elm::ListItem(configName);

    uint64_t currentMHz = this->configList->values[configVal];
    char valueText[32];
    snprintf(valueText, sizeof(valueText), "%lu MHz", currentMHz);
    listItem->setValue(valueText);

    listItem->setClickListener(
        [this, configVal, module, labels](u64 keys)
        {
            if ((keys & HidNpadButton_A) == 0)
                return false;

            std::uint32_t hzList[SYSCLK_FREQ_LIST_MAX];
            std::uint32_t hzCount;

            Result rc = sysclkIpcGetFreqList(module, hzList, SYSCLK_FREQ_LIST_MAX, &hzCount);
            if (R_FAILED(rc)) {
                FatalGui::openWithResultCode("sysclkIpcGetFreqList", rc);
                return false;
            }

            std::uint32_t currentHz = this->configList->values[configVal] * 1'000'000;

            tsl::changeTo<FreqChoiceGui>(
                currentHz,
                hzList,
                hzCount,
                module,
                [this, configVal](std::uint32_t hz)
                {
                    uint64_t mhz = hz / 1'000'000;
                    this->configList->values[configVal] = mhz;

                    Result rc = sysclkIpcSetConfigValues(this->configList);
                    if (R_FAILED(rc)) {
                        FatalGui::openWithResultCode("sysclkIpcSetConfigValues", rc);
                        return false;
                    }

                    this->lastContextUpdate = armGetSystemTick();
                    return true;
                },
                false,
                labels
            );

            return true;
        });

    this->listElement->addItem(listItem);
    this->configButtons[configVal] = listItem;

    this->configRanges[configVal] = ValueRange(0, 0, 0, "MHz", 1);
}

void MiscGui::updateConfigToggles() {
    for (const auto& [value, toggle] : this->configToggles) {
        if (toggle != nullptr)
            toggle->setState(this->configList->values[value]);
    }
}

void MiscGui::listUI()
{
    this->listElement->addItem(new tsl::elm::CategoryHeader("Settings"));
    addConfigToggle(HocClkConfigValue_UncappedClocks, nullptr);
    addConfigToggle(HocClkConfigValue_OverwriteBoostMode, nullptr);

    addConfigToggle(HocClkConfigValue_HandheldGovernor, nullptr);

    this->listElement->addItem(new tsl::elm::CategoryHeader("Experimental"));
    addConfigToggle(HocClkConfigValue_ThermalThrottle, nullptr);
    addConfigToggle(HocClkConfigValue_HandheldTDP, nullptr);
    std::map<uint32_t, std::string> labels_pwr_r = {
        {8600, "Official Rating"}
    };
    std::map<uint32_t, std::string> labels_pwr_l = {
        {6400, "Official Rating"}
    };
    ValueThresholds tdpThresholds(8600, 9500);
    addConfigButton(
        HocClkConfigValue_HandheldTDPLimit,
        "TDP Threshold",
        ValueRange(5000, 10000, 100, "mW", 1),
        "Power",
        &tdpThresholds,
        labels_pwr_r
    );

    ValueThresholds tdpThresholdsLite(6400, 7500);
    addConfigButton(
        HocClkConfigValue_LiteTDPLimit,
        "Lite TDP Threshold",
        ValueRange(4000, 8000, 100, "mW", 1),
        "Power",
        &tdpThresholdsLite,
        labels_pwr_r
    );

    ValueThresholds throttleThresholds(70, 80);
    addConfigButton(
        HocClkConfigValue_ThermalThrottleThreshold,
        "Thermal Throttle Limit",
        ValueRange(50, 85, 1, "°C", 1),
        "Temp",
        &throttleThresholds
    );


        std::map<uint32_t, std::string> cpu_freq_label_m = {
        {612000000, "Sleep Mode"},
        {1020000000, "Stock"},
        {1224000000, "Dev OC"},
        {1785000000, "Boost Mode"},
        {1963000000, "Safe Max"},
        {2397000000, "Unsafe Max"},
        {2805000000, "Aboslute Max"},
    };

    std::map<uint32_t, std::string> cpu_freq_label_e = {
        {612000000, "Sleep Mode"},
        {1020000000, "Stock"},
        {1224000000, "Dev OC"},
        {1785000000, "Boost Mode & Safe Max"},
        {2091000000, "Unsafe Max"},
        {2295000000, "Aboslute Max"},
    };

    std::map<uint32_t, std::string> gpu_freq_label_e = {
        {76800000, "Boost Mode"},
        {307200000, "Handheld"},
        {384000000, "Handheld"},
        {460800000, "Handheld Safe Max"},
        {768000000, "Docked"},
        {844000000, "Safe Max"},
        {998400000, "Unsafe Max"},
        {1075200000, "Aboslute Max"},
    };

    std::map<uint32_t, std::string> gpu_freq_label_m = {
        {76800000, "Boost Mode"},
        {307200000, "Handheld"},
        {384000000, "Handheld"},
        {460800000, "Handheld"},
        {614400000, "Handheld Safe Max"},
        {768000000, "Docked"},
        {1152200000, "Safe Max"},
        {1305600000, "Unsafe Max"},
        {1536000000, "Aboslute Max"},
    };

    std::map<uint32_t, std::string> emc_freq_label_e = {
        {133120000, "Handheld"},
        {160000000, "Docked & Safe Max"},
        {213100000, "JEDEC Max"},
        {236000000, "Absolute Max"},
    };


    std::map<uint32_t, std::string> emc_freq_label_m = {
        {133120000, "Handheld"},
        {160000000, "Docked"},
        {186600000, "Safe Max (3733MT/s)"},
        {213300000, "Safe Max (4266MT/s)"},
        {320000000, "Absolute Max"},
    };





    if(IsMariko()) {
        addFreqButton(HocClkConfigValue_MarikoMaxCpuClock, nullptr, SysClkModule_CPU, cpu_freq_label_m);
        addFreqButton(HocClkConfigValue_MarikoMaxGpuClock, nullptr, SysClkModule_GPU, gpu_freq_label_m);
        addFreqButton(HocClkConfigValue_MarikoMaxMemClock, nullptr, SysClkModule_MEM, emc_freq_label_m);
    } else {
        addFreqButton(HocClkConfigValue_EristaMaxCpuClock, nullptr, SysClkModule_CPU, cpu_freq_label_e);
        addFreqButton(HocClkConfigValue_EristaMaxGpuClock, nullptr, SysClkModule_GPU, gpu_freq_label_e);
        addFreqButton(HocClkConfigValue_EristaMaxMemClock, nullptr, SysClkModule_MEM, emc_freq_label_e);
    }

    this->listElement->addItem(new tsl::elm::CategoryHeader("KIP"));

    std::map<uint32_t, std::string> emc_voltage_label = {
        {1100000, "Default (Mariko)"},
        {1125000, "Default (Erista)"},
        {1175000, "Rating"},
        {1212500, "Safe Max"},
    };



    ValueThresholds vdd2Thresholds(1212500, 1250000);
    addConfigButton(
        KipConfigValue_commonEmcMemVolt,
        "EMC VDD2 Voltage",
        ValueRange(912500, 1350000, 12500, "mV", 1000),
        "Voltage",
        &vdd2Thresholds
    );

    tsl::elm::ListItem* saveBtn = new tsl::elm::ListItem("Save KIP Settings");
    saveBtn->setClickListener([](u64 keys) {
        if (keys & HidNpadButton_A) {
            Result rc = hocClkIpcSetKipData();
            if (R_FAILED(rc)) {
                FatalGui::openWithResultCode("hocClkIpcSetKipData", rc);
                return false;
            }
            return true;
        }
        return false;
    });
    this->listElement->addItem(saveBtn);
}

void MiscGui::refresh() {
    BaseMenuGui::refresh();

    if (this->context && ++frameCounter >= 60) {
        frameCounter = 0;

        sysclkIpcGetConfigValues(this->configList);

        updateConfigToggles();

        for (const auto& [configVal, button] : this->configButtons) {
            uint64_t currentValue = this->configList->values[configVal];
            const ValueRange& range = this->configRanges[configVal];

            char valueText[32];
            if (currentValue == 0) {
                snprintf(valueText, sizeof(valueText), "%s", VALUE_DEFAULT_TEXT);
            } else {
                uint64_t displayValue = currentValue / range.divisor;
                if (!range.suffix.empty()) {
                    snprintf(valueText, sizeof(valueText), "%lu %s", displayValue, range.suffix.c_str());
                } else {
                    snprintf(valueText, sizeof(valueText), "%lu", displayValue);
                }
            }
            button->setValue(valueText);
        }
    }
}