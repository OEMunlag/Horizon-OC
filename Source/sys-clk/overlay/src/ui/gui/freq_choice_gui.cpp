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
 
/* --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */


 #include "freq_choice_gui.h"

 #include "../format.h"
 #include "fatal_gui.h"
 
 FreqChoiceGui::FreqChoiceGui(std::uint32_t selectedHz, std::uint32_t *hzList, std::uint32_t hzCount, SysClkModule module, FreqChoiceListener listener, bool checkMax)
 {
     this->selectedHz = selectedHz;
     this->hzList = hzList;
     this->hzCount = hzCount;
     this->module = module;
     this->listener = listener;
     this->checkMax = checkMax;
     this->configList = new SysClkConfigValueList {};
 }
FreqChoiceGui::~FreqChoiceGui()
{
    delete this->configList;
}

 tsl::elm::ListItem* FreqChoiceGui::createFreqListItem(std::uint32_t hz, bool selected, int safety)
 {
     std::string text = formatListFreqHz(hz);
     if (selected) text += " \uE14B";
 
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
 
     listItem->setClickListener([this, hz](u64 keys)
     {
         if ((keys & HidNpadButton_A) == HidNpadButton_A && this->listener) {
             if (this->listener(hz)) {
                 tsl::goBack();
             }
             return true;
         }
         return false;
     });
 
     return listItem;
 }
 
 void FreqChoiceGui::listUI()
 {
     sysclkIpcGetConfigValues(this->configList);
     // Add CategoryHeader based on module
     std::string moduleName = sysclkFormatModule(this->module, false);
     this->listElement->addItem(new tsl::elm::CategoryHeader(moduleName));
 
     this->listElement->addItem(this->createFreqListItem(0, this->selectedHz == 0, false));
     std::uint32_t hz;
     for (std::uint32_t i = 0; i < this->hzCount; i++)
     {
         hz = this->hzList[i];
         uint32_t mhz = hz / 1000000;
         // Skip 204 MHz exactly
         if(checkMax && IsMariko()) {
            if (this->configList->values[HocClkConfigValue_MarikoMaxCpuClock] < mhz && moduleName == "cpu") {
                continue;
            }
            
            if (this->configList->values[HocClkConfigValue_MarikoMaxGpuClock] < mhz && moduleName == "gpu") {
                continue;
            }
            if (this->configList->values[HocClkConfigValue_MarikoMaxMemClock] < mhz && moduleName == "mem") {
                continue;
            }
        } else if (checkMax && IsErista()) {
            if (this->configList->values[HocClkConfigValue_EristaMaxCpuClock] < mhz && moduleName == "cpu") {
                continue;
            }
            
            if (this->configList->values[HocClkConfigValue_EristaMaxGpuClock] < mhz && moduleName == "gpu") {
                continue;
            }
            if (this->configList->values[HocClkConfigValue_EristaMaxMemClock] < mhz && moduleName == "mem") {
                continue;
            }
         }
         if (moduleName == "mem" && mhz <= 600)
         {
             continue;
         }
         uint32_t unsafe_cpu;
         uint32_t unsafe_gpu;
         uint32_t danger_cpu;
         uint32_t danger_gpu;
         if (IsMariko())
         {
             unsafe_cpu = 1964;
             unsafe_gpu = 1076;
             danger_cpu = 2398;
             danger_gpu = 1306;
         }
         else
         {
             unsafe_cpu = 1786;
             unsafe_gpu = 922;
             danger_cpu = 2092;
             danger_gpu = 999;
         }

         if (moduleName == "cpu") {
             if (mhz >= danger_cpu) {
                 this->listElement->addItem(this->createFreqListItem(hz, mhz == this->selectedHz / 1000000, 2));
                 continue;
             }
             if (mhz >= unsafe_cpu) {
                 this->listElement->addItem(this->createFreqListItem(hz, mhz == this->selectedHz / 1000000, 1));
                 continue;
             }
             if (mhz <= unsafe_cpu) {
                this->listElement->addItem(this->createFreqListItem(hz, mhz == this->selectedHz / 1000000, 0));
                continue;
             }
         } else if (moduleName == "gpu") {
             if (mhz >= danger_gpu) {
                 this->listElement->addItem(this->createFreqListItem(hz, mhz == this->selectedHz / 1000000, 2));
                 continue;
             }
             if (mhz >= unsafe_gpu) {
                 this->listElement->addItem(this->createFreqListItem(hz, mhz == this->selectedHz / 1000000, 1));
                 continue;
             }
             if (mhz <= unsafe_gpu) {
                this->listElement->addItem(this->createFreqListItem(hz, mhz == this->selectedHz / 1000000, 0));
                continue;
             }
         } else if (moduleName == "mem") {
            this->listElement->addItem(this->createFreqListItem(hz, mhz == this->selectedHz / 1000000, 0));
            continue;
         }
         
     }
     this->listElement->jumpToItem("", "");
 }