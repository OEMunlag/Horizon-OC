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
    const std::map<uint32_t, std::string>& labels,
    const std::vector<NamedValue>& namedValues,
    bool showDefaultValue)
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
        [this, configVal, range, categoryName, thresholdsCopy, labels, namedValues, showDefaultValue](u64 keys)
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
                    labels,
                    namedValues,
                    showDefaultValue
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
                    labels,
                    namedValues,
                    showDefaultValue
                );
            }

            return true;
        });

    this->listElement->addItem(listItem);
    this->configButtons[configVal] = listItem;
    this->configRanges[configVal] = range;
    this->configNamedValues[configVal] = namedValues;
}

void MiscGui::updateConfigToggles() {
    for (const auto& [value, toggle] : this->configToggles) {
        if (toggle != nullptr)
            toggle->setState(this->configList->values[value]);
    }
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

void MiscGui::listUI()
{
    ValueThresholds thresholdsDisabled(0, 0);
    std::vector<NamedValue> noNamedValues = {};

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

    this->listElement->addItem(new tsl::elm::CategoryHeader("KIP Editing"));

    addConfigToggle(HocClkConfigValue_KipEditing, nullptr);

    this->listElement->addItem(new tsl::elm::CategoryHeader("KIP Settings"));

    std::vector<NamedValue> autoAdjOptions = {
        NamedValue("AUTO_ADJ", 0),
        NamedValue("AUTO_ADJ_BL", 1)
    };

    addConfigButton(
        KipConfigValue_mtcConf,
        "MTC Configuration",
        ValueRange(0, 1, 1, "", 1),
        "MTC Configuration",
        &thresholdsDisabled,
        {},
        autoAdjOptions,
        false
    );

    addConfigToggle(KipConfigValue_hpMode, "HP Mode");

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
        &vdd2Thresholds,
        emc_voltage_label,
        noNamedValues,
        false
    );

    std::vector<NamedValue> marikoMaxEmcClock = {
        NamedValue("1600MHz", 1600000),
        NamedValue("1633MHz", 1633000),
        NamedValue("1666MHz", 1666000),
        NamedValue("1700MHz", 1700000),
        NamedValue("1733MHz", 1733000),
        NamedValue("1766MHz", 1766000),
        NamedValue("1800MHz", 1800000),
        NamedValue("1833MHz", 1833000),
        NamedValue("1866MHz", 1866000),
        NamedValue("1900MHz", 1900000),
        NamedValue("1933MHz", 1933000),
        NamedValue("1966MHz", 1966000),
        NamedValue("2000MHz", 2000000),
        NamedValue("2033MHz", 2033000),
        NamedValue("2066MHz", 2066000),
        NamedValue("2100MHz", 2100000),
        NamedValue("2133MHz", 2133000),
        NamedValue("2166MHz", 2166000),
        NamedValue("2200MHz", 2200000),
        NamedValue("2233MHz", 2233000),
        NamedValue("2266MHz", 2266000),
        NamedValue("2300MHz", 2300000),
        NamedValue("2333MHz", 2333000),
        NamedValue("2366MHz", 2366000),
        NamedValue("2400MHz", 2400000),
        NamedValue("2433MHz", 2433000),
        NamedValue("2466MHz", 2466000),
        NamedValue("2500MHz", 2500000),
        NamedValue("2533MHz", 2533000),
        NamedValue("2566MHz", 2566000),
        NamedValue("2600MHz", 2600000),
        NamedValue("2633MHz", 2633000),
        NamedValue("2666MHz", 2666000),
        NamedValue("2700MHz", 2700000),
        NamedValue("2733MHz", 2733000),
        NamedValue("2766MHz", 2766000),
        NamedValue("2800MHz", 2800000),
        NamedValue("2833MHz", 2833000),
        NamedValue("2866MHz", 2866000),
        NamedValue("2900MHz", 2900000),
        NamedValue("2933MHz", 2933000),
        NamedValue("2966MHz", 2966000),
        NamedValue("3000MHz", 3000000),
        NamedValue("3033MHz", 3033000),
        NamedValue("3066MHz", 3066000),
        NamedValue("3100MHz", 3100000),
        NamedValue("3133MHz", 3133000),
        NamedValue("3166MHz", 3166000),
        NamedValue("3200MHz", 3200000),
    };

    std::vector<NamedValue> eristaMaxEmcClock = {
        NamedValue("1600MHz", 1600000),
        NamedValue("1633MHz", 1633000),
        NamedValue("1666MHz", 1666000),
        NamedValue("1700MHz", 1700000),
        NamedValue("1733MHz", 1733000),
        NamedValue("1766MHz", 1766000),
        NamedValue("1800MHz", 1800000),
        NamedValue("1833MHz", 1833000),
        NamedValue("1866MHz", 1866000),
        NamedValue("1900MHz", 1900000),
        NamedValue("1933MHz", 1933000),
        NamedValue("1966MHz", 1966000),
        NamedValue("2000MHz", 2000000),
        NamedValue("2033MHz", 2033000),
        NamedValue("2066MHz", 2066000),
        NamedValue("2100MHz", 2100000),
        NamedValue("2133MHz", 2133000),
        NamedValue("2166MHz", 2166000),
        NamedValue("2200MHz", 2200000),
        NamedValue("2233MHz", 2233000),
        NamedValue("2266MHz", 2266000),
        NamedValue("2300MHz", 2300000),
        NamedValue("2333MHz", 2333000),
        NamedValue("2366MHz", 2366000),
        NamedValue("2400MHz", 2400000),
    };
    if(IsErista()) {
        addConfigButton(
            KipConfigValue_eristaEmcMaxClock,
            "EMC Max Clock",
            ValueRange(0, 1, 1, "", 1),
            "EMC Max Clock",
            &thresholdsDisabled,
            {},
            eristaMaxEmcClock,
            false
        );
    } else {
        addConfigButton(
            KipConfigValue_marikoEmcMaxClock,
            "EMC Max Clock",
            ValueRange(0, 1, 1, "", 1),
            "EMC Max Clock",
            &thresholdsDisabled,
            {},
            marikoMaxEmcClock,
            false
        );
        addConfigButton(
            KipConfigValue_marikoEmcVddqVolt,
            "EMC VDDQ Voltage",
            ValueRange(550000, 700000, 5000, "mV", 1000),
            "EMC VDDQ Voltage",
            &thresholdsDisabled,
            {},
            {},
            false
        );
    }

    addConfigButton(
        KipConfigValue_t1_tRCD,
        "t1 tRCD",
        ValueRange(0, 8, 1, "", 1),
        "tRCD",
        &thresholdsDisabled,
        {},
        {},
        false
    );
    addConfigButton(
        KipConfigValue_t2_tRP,
        "t2 tRP",
        ValueRange(0, 8, 1, "", 1),
        "tRP",
        &thresholdsDisabled,
        {},
        {},
        false
    );
    addConfigButton(
        KipConfigValue_t3_tRAS,
        "t3 tRAS",
        ValueRange(0, 10, 1, "", 1),
        "tRAS",
        &thresholdsDisabled,
        {},
        {},
        false
    );
    addConfigButton(
        KipConfigValue_t4_tRRD,
        "t4 tRRD",
        ValueRange(0, 7, 1, "", 1),
        "tRRD",
        &thresholdsDisabled,
        {},
        {},
        false
    );
    addConfigButton(
        KipConfigValue_t5_tRFC,
        "t5 tRFC",
        ValueRange(0, 11, 1, "", 1),
        "tRFC",
        &thresholdsDisabled,
        {},
        {},
        false
    );

    addConfigButton(
        KipConfigValue_t6_tRTW,
        "t6 tRTW",
        ValueRange(0, 10, 1, "", 1),
        "tRTW",
        &thresholdsDisabled,
        {},
        {},
        false
    );

    addConfigButton(
        KipConfigValue_t7_tWTR,
        "t7 tWTR",
        ValueRange(0, 10, 1, "", 1),
        "tWTR",
        &thresholdsDisabled,
        {},
        {},
        false
    );

    addConfigButton(
        KipConfigValue_t8_tREFI,
        "t8 tREFI",
        ValueRange(0, 6, 1, "", 1),
        "tREFI",
        &thresholdsDisabled,
        {},
        {},
        false
    );

    std::vector<NamedValue> rlLabels = {
        NamedValue("1600BL", 0),
        NamedValue("1866BL", 4),
        NamedValue("2133BL", 8)
    };

    std::vector<NamedValue> wlLabels = {
        NamedValue("1600BL", 0),
        NamedValue("1866BL", 2),
        NamedValue("2133BL", 4)
    };

    addConfigButton(
        KipConfigValue_mem_burst_read_latency,
        "Read Latency",
        ValueRange(0, 6, 1, "", 0),
        "Read Latency",
        &thresholdsDisabled,
        {},
        rlLabels,
        false
    );

    addConfigButton(
        KipConfigValue_mem_burst_write_latency,
        "Write Latency",
        ValueRange(0, 6, 1, "", 0),
        "Write Latency",
        &thresholdsDisabled,
        {},
        wlLabels,
        false
    );

    std::vector<NamedValue> marikoTableConf = {
        NamedValue("Auto", 0),
        NamedValue("Default", 1),
        NamedValue("1581MHz Tbreak", 2),
        NamedValue("1683MHz Tbreak", 3),
        NamedValue("HELIOS Table", 4)
    };

    ValueThresholds mCpuVoltThresholds(1160, 1180);

    if(IsErista()) {
        addConfigButton(
            KipConfigValue_eristaCpuUV,
            "CPU UV",
            ValueRange(0, 5, 1, "", 1),
            "CPU UV",
            &thresholdsDisabled,
            {},
            {},
            true
        );
        addConfigButton(
            KipConfigValue_eristaCpuMaxVolt,
            "CPU Max Voltage",
            ValueRange(1120, 1235, 5, "mV", 1),
            "CPU Max Voltage",
            &thresholdsDisabled,
            {},
            {},
            false
        );
    } else {
        addConfigButton(
            KipConfigValue_tableConf,
            "CPU UV Table",
            ValueRange(0, 12, 1, "", 0),
            "CPU UV Table",
            &thresholdsDisabled,
            {},
            marikoTableConf,
            false
        );
        addConfigButton(
            KipConfigValue_marikoCpuUVLow,
            "CPU Low UV",
            ValueRange(0, 8, 1, "", 1),
            "CPU Low UV",
            &thresholdsDisabled,
            {},
            {},
            false
        );
        addConfigButton(
            KipConfigValue_marikoCpuUVHigh,
            "CPU High UV",
            ValueRange(0, 12, 1, "", 1),
            "CPU High UV",
            &thresholdsDisabled,
            {},
            {},
            false
        );
        addConfigButton(
            KipConfigValue_marikoCpuMaxVolt,
            "CPU Max Voltage",
            ValueRange(1000, 1235, 5, "mV", 1),
            "CPU Max Voltage",
            &mCpuVoltThresholds,
            {},
            {},
            false
        );
    }

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

static std::string getValueDisplayText(uint64_t currentValue, 
                                       const ValueRange& range,
                                       const std::vector<NamedValue>& namedValues)
{
    char valueText[32];
    
    for (const auto& namedValue : namedValues) {
        if (currentValue == namedValue.value) {
            return namedValue.name;
        }
    }
    
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
    return std::string(valueText);
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
            
            auto namedValuesIt = this->configNamedValues.find(configVal);
            const std::vector<NamedValue>& namedValues = (namedValuesIt != this->configNamedValues.end()) 
                ? namedValuesIt->second 
                : std::vector<NamedValue>();
            
            char valueText[32];
            
            bool foundNamedValue = false;
            for (const auto& namedValue : namedValues) {
                if (currentValue == namedValue.value) {
                    snprintf(valueText, sizeof(valueText), "%s", namedValue.name.c_str());
                    foundNamedValue = true;
                    break;
                }
            }
            
            if (!foundNamedValue) {
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