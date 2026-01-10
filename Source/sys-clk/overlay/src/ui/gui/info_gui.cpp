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

tsl::elm::ListItem* SpeedoItem;
tsl::elm::ListItem* IddqItem;

InfoGui::InfoGui()
{
    // Initialize display strings
    memset(strings, 0, sizeof(strings));
}

InfoGui::~InfoGui()
{
}

void InfoGui::listUI()
{
    SpeedoItem =
        new tsl::elm::ListItem("Speedos:");
    this->listElement->addItem(SpeedoItem);

    IddqItem =
        new tsl::elm::ListItem("IDDQ:");
    this->listElement->addItem(IddqItem);

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
    
    // Format strings once per refresh
    sprintf(strings[0], "%u/%u/%u", this->context->speedos[HorizonOCSpeedo_CPU], this->context->speedos[HorizonOCSpeedo_GPU], this->context->speedos[HorizonOCSpeedo_SOC]);
    sprintf(strings[1], "%u/%u/%u", this->context->iddq[HorizonOCSpeedo_CPU], this->context->iddq[HorizonOCSpeedo_GPU], this->context->iddq[HorizonOCSpeedo_SOC]);
    SpeedoItem->setValue(strings[0]);
    IddqItem->setValue(strings[1]);

}