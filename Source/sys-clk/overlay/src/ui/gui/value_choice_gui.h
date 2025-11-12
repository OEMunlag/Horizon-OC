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
 
/* --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */


 #pragma once

 #include <list>
 #include <functional>
 #include <string>
 #include "base_menu_gui.h"
 
 using ValueChoiceListener = std::function<bool(std::uint32_t value)>;
 
 #define VALUE_DEFAULT_TEXT "Default"
 
 struct ValueRange {
    std::uint32_t min;
    std::uint32_t max;
    std::uint32_t step;
    std::string suffix;
    std::uint32_t divisor;      // Divide input values by this for display
    int decimalPlaces;          // Number of decimal places to display (0-6)
    
    ValueRange() : min(0), max(0), step(1), suffix(""), divisor(1), decimalPlaces(0) {}
    
    ValueRange(std::uint32_t min, std::uint32_t max, std::uint32_t step, 
               const std::string& suffix = "", std::uint32_t divisor = 1, int decimalPlaces = 0)
        : min(min), max(max), step(step), suffix(suffix), divisor(divisor), decimalPlaces(decimalPlaces) {}
};

 struct ValueThresholds {
     std::uint32_t warning;  // Values >= this show orange
     std::uint32_t danger;   // Values >= this show red
     
     ValueThresholds(std::uint32_t warning = 0, std::uint32_t danger = 0)
         : warning(warning), danger(danger) {}
 };
 
 class ValueChoiceGui : public BaseMenuGui
 {
 protected:
     std::uint32_t selectedValue;
     ValueRange range;
     std::string categoryName;
     ValueChoiceListener listener;
     ValueThresholds thresholds;
     bool enableThresholds;
     
     tsl::elm::ListItem* createValueListItem(std::uint32_t value, bool selected, int safety);
     std::string formatValue(std::uint32_t value);
     int getSafetyLevel(std::uint32_t value);
 
 public:
     ValueChoiceGui(std::uint32_t selectedValue,
                    const ValueRange& range,
                    const std::string& categoryName,
                    ValueChoiceListener listener,
                    const ValueThresholds& thresholds = ValueThresholds(),
                    bool enableThresholds = false);
     ~ValueChoiceGui();
 
     void listUI() override;
 };