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

InfoGui::InfoGui()
{
    if (!this->Currentcontext) [[unlikely]] {
        this->Currentcontext = new SysClkContext;
    }
    Result rc = sysclkIpcGetCurrentContext(this->Currentcontext);
    if (R_FAILED(rc)) [[unlikely]] {
        FatalGui::openWithResultCode("sysclkIpcGetCurrentContext", rc);
        return;
    }

}

InfoGui::~InfoGui()
{

}


void InfoGui::listUI()
{
    tsl::elm::ListItem* cpuSpeedoItem =
        new tsl::elm::ListItem("CPU Speedo");
    cpuSpeedoItem->setValue(std::to_string(this->Currentcontext->speedos[HorizonOCSpeedo_CPU]));
    this->listElement->addItem(cpuSpeedoItem);

    tsl::elm::ListItem* gpuSpeedoItem =
        new tsl::elm::ListItem("GPU Speedo");
    gpuSpeedoItem->setValue(std::to_string(this->Currentcontext->speedos[HorizonOCSpeedo_GPU]));
    this->listElement->addItem(gpuSpeedoItem);

    tsl::elm::ListItem* socSpeedoItem =
        new tsl::elm::ListItem("GPU Speedo");
    socSpeedoItem->setValue(std::to_string(this->Currentcontext->speedos[HorizonOCSpeedo_SOC]));
    this->listElement->addItem(socSpeedoItem);
}

void InfoGui::update()
{
    BaseMenuGui::update();
    
}

void InfoGui::refresh()
{
    BaseMenuGui::refresh();
    
    static int frameCounter = 0;
    
    if (this->context && ++frameCounter >= 60) {
        frameCounter = 0;
        
    }
}