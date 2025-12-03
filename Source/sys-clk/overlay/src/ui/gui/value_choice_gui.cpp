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


 #include "value_choice_gui.h"
 #include "../format.h"
 #include "fatal_gui.h"
 #include <sstream>
 #include <iomanip>

 ValueChoiceGui::ValueChoiceGui(std::uint32_t selectedValue,
                                const ValueRange& range,
                                const std::string& categoryName,
                                ValueChoiceListener listener,
                                const ValueThresholds& thresholds,
                                bool enableThresholds)
     : selectedValue(selectedValue),
       range(range),
       categoryName(categoryName),
       listener(listener),
       thresholds(thresholds),
       enableThresholds(enableThresholds)
 {
 }

 ValueChoiceGui::~ValueChoiceGui()
 {
 }

 std::string ValueChoiceGui::formatValue(std::uint32_t value)
 {
     std::ostringstream oss;

     if (value == 0) {

         return VALUE_DEFAULT_TEXT;
     }

     std::uint32_t displayValue = value / range.divisor;

     oss << displayValue;
     if (!range.suffix.empty()) {
         oss << " " << range.suffix;
     }
     return oss.str();
 }

 int ValueChoiceGui::getSafetyLevel(std::uint32_t value)
 {
     if (!enableThresholds) {
         return 0; 
     }

     std::uint32_t scaledValue = value / range.divisor;

     if (scaledValue > thresholds.danger) {
         return 2; 
     }
     if (scaledValue > thresholds.warning) {
         return 1; 
     }
     return 0; 
 }

 tsl::elm::ListItem* ValueChoiceGui::createValueListItem(std::uint32_t value, bool selected, int safety)
 {
     std::string text = formatValue(value);
     if (selected) {
         text += " \uE14B"; 
     }

     tsl::elm::ListItem* listItem = new tsl::elm::ListItem(text, "", false);

     switch (safety)
     {
     case 0:
         listItem->setTextColor(tsl::Color(255, 255, 255, 255)); 
         listItem->setValueColor(tsl::Color(255, 255, 255, 255));
         break;
     case 1:
         listItem->setTextColor(tsl::Color(255, 165, 0, 255)); 
         listItem->setValueColor(tsl::Color(255, 165, 0, 255));
         break;
     case 2:
         listItem->setTextColor(tsl::Color(255, 0, 0, 255)); 
         listItem->setValueColor(tsl::Color(255, 0, 0, 255));
         break;
     }

     listItem->setClickListener([this, value](u64 keys)
     {
         if ((keys & HidNpadButton_A) == HidNpadButton_A && this->listener) {

             if (this->listener(value)) {
                 tsl::goBack();
             }
             return true;
         }
         return false;
     });

     return listItem;
 }

 void ValueChoiceGui::listUI()
 {

     if (!categoryName.empty()) {
         this->listElement->addItem(new tsl::elm::CategoryHeader(categoryName));
     }

     this->listElement->addItem(this->createValueListItem(0, this->selectedValue == 0, 0));

     for (std::uint32_t value = range.min; value <= range.max; value += range.step)
     {
         int safety = getSafetyLevel(value);
         bool selected = (value == this->selectedValue);
         this->listElement->addItem(this->createValueListItem(value, selected, safety));
     }

     this->listElement->jumpToItem("", "\uE14B");
 }