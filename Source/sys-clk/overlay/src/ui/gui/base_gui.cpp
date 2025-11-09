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


#include "base_gui.h"

#include "../elements/base_frame.h"
#include "logo_rgba_bin.h"




#define LOGO_X 20
#define LOGO_Y 45
#define LOGO_LABEL_FONT_SIZE 35

#define VERSION_X (LOGO_X + 250)
#define VERSION_Y LOGO_Y-40
#define VERSION_FONT_SIZE 15

std::string getVersionString() {
    char buf[0x100] = "";  // 256 bytes — safe for any expected version string
    Result rc = sysclkIpcGetVersionString(buf, sizeof(buf));
    if (R_FAILED(rc) || buf[0] == '\0') {
        return "unknown";
    }
    return std::string(buf);
}

void BaseGui::preDraw(tsl::gfx::Renderer* renderer)
{
//    renderer->drawBitmap(LOGO_X, LOGO_Y, LOGO_WIDTH, LOGO_HEIGHT, logo_rgba_bin);
    renderer->drawString("Horizon OC overlay", false, LOGO_X, LOGO_Y, LOGO_LABEL_FONT_SIZE, renderer->a(TEXT_COLOR));
//    renderer->drawString(TARGET_VERSION, false, VERSION_X, VERSION_Y, VERSION_FONT_SIZE, tsl::bannerVersionTextColor);
}

tsl::elm::Element* BaseGui::createUI()
{
    BaseFrame* rootFrame = new BaseFrame(this);
    rootFrame->setContent(this->baseUI());
    return rootFrame;
}

void BaseGui::update()
{
    this->refresh();
}
