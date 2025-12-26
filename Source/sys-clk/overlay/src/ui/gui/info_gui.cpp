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
 */
#include "info_gui.h"
#include "../format.h"
#include <tesla.hpp>
#include <string>
tsl::elm::ListItem* cpuSpeedoItem;
tsl::elm::ListItem* gpuSpeedoItem;
tsl::elm::ListItem* socSpeedoItem;

InfoGui::InfoGui()
{
    // Initialize display strings
    memset(speedoStrings, 0, sizeof(speedoStrings));
}

InfoGui::~InfoGui()
{
}

void InfoGui::listUI()
{
    cpuSpeedoItem =
        new tsl::elm::ListItem("CPU Speedo");
    this->listElement->addItem(cpuSpeedoItem);
    
    gpuSpeedoItem =
        new tsl::elm::ListItem("GPU Speedo");
    this->listElement->addItem(gpuSpeedoItem);
    
    socSpeedoItem =
        new tsl::elm::ListItem("SOC Speedo");
    this->listElement->addItem(socSpeedoItem);
}

void InfoGui::update()
{
    BaseMenuGui::update();
}

void InfoGui::refresh()
{
    BaseMenuGui::refresh();
    
    if (!this->context)
        return;
    
    // Format speedo strings once per refresh
    sprintf(speedoStrings[0], "%u", this->context->speedos[HorizonOCSpeedo_CPU]);
    sprintf(speedoStrings[1], "%u", this->context->speedos[HorizonOCSpeedo_GPU]);
    sprintf(speedoStrings[2], "%u", this->context->speedos[HorizonOCSpeedo_SOC]);
    cpuSpeedoItem->setValue(speedoStrings[HorizonOCSpeedo_CPU]); // this is SO hacky but it works i guess
    gpuSpeedoItem->setValue(speedoStrings[HorizonOCSpeedo_GPU]);
    socSpeedoItem->setValue(speedoStrings[HorizonOCSpeedo_SOC]);

}