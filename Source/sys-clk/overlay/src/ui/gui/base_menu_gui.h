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

#include "../../rgltr_services.h"
#include "../../ipc.h"
#include "base_gui.h"

class BaseMenuGui : public BaseGui
{
    protected:
        SysClkContext* context;
        std::uint64_t lastContextUpdate;

    public:
        bool g_hardwareModelCached = false;
        bool g_isMariko = false;

        bool IsMariko() {
            if (!g_hardwareModelCached) {
                SetSysProductModel model = SetSysProductModel_Invalid;
                setsysGetProductModel(&model);
                g_isMariko = (model == SetSysProductModel_Iowa || 
                            model == SetSysProductModel_Hoag || 
                            model == SetSysProductModel_Calcio || 
                            model == SetSysProductModel_Aula);
                g_hardwareModelCached = true;
            }
            return g_isMariko;
        }

        bool IsErista() {
            return !IsMariko();
        }
        BaseMenuGui();
        ~BaseMenuGui();
        void preDraw(tsl::gfx::Renderer* renderer) override;
        tsl::elm::List* listElement;
        tsl::elm::Element* baseUI() override;
        void refresh() override;
        virtual void listUI() = 0;

    private:
        char displayStrings[32][32];  // Pre-formatted display strings
        tsl::Color tempColors[3];     // Pre-computed temperature colors
};
