/*
 * Copyright (c) Souldbminer and Horizon OC Contributors
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
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

#include <tesla.hpp>
#include <math.h>

// -------------------------------------------------------------
// Layout constants
// -------------------------------------------------------------

#define LOGO_X 20
#define LOGO_Y 50
#define LOGO_LABEL_FONT_SIZE 45

#define VERSION_X (LOGO_X + 250)
#define VERSION_Y (LOGO_Y - 40)
#define VERSION_FONT_SIZE 15

// -------------------------------------------------------------
// Version string getter
// -------------------------------------------------------------

std::string getVersionString() {
    char buf[0x100] = "";
    Result rc = sysclkIpcGetVersionString(buf, sizeof(buf));
    if (R_FAILED(rc) || buf[0] == '\0') {
        return "HorizonOC-Misc";
    }
    return std::string(buf);
}

// -------------------------------------------------------------
// Animated Ultra Text
// -------------------------------------------------------------

// Your animated wave colors (example placeholders)
static constexpr tsl::Color dynamicLogoRGB1 = tsl::Color(40, 255, 80, 255);
static constexpr tsl::Color dynamicLogoRGB2 = tsl::Color(120, 255, 160, 255);

// Your project name rendered letter-by-letter
static constexpr const char* PROJECT_NAME = "Horizon OC Gaea";

// Fully corrected function signature
static s32 drawDynamicUltraText(
    tsl::gfx::Renderer* renderer,
    s32 startX,
    s32 y,
    u32 fontSize,
    const tsl::Color& staticColor,
    bool useNotificationMethod = false)
{
    static constexpr double cycleDuration = 1.6;

    const std::string name = "Horizon OC Gaea";
    s32 currentX = startX;

    const u64 currentTime_ns = armTicksToNs(armGetSystemTick());
    const double timeNow = static_cast<double>(currentTime_ns) / 1e9;
    const double timeBase = fmod(timeNow, cycleDuration);

    // Controls wave spacing
    const double waveScale = 2.0 * M_PI / cycleDuration;

    // Every character has its own index offset
    for (size_t i = 0; i < name.size(); i++)
    {
        char letter = name[i];
        if (letter == '\0') break;

        // phase shift per character → THIS CREATES THE WAVE
        double phase = waveScale * (timeBase + i * 0.12);

        double raw = cos(phase);
        double n = (raw + 1.0) * 0.5;

        // Smoothstep ×2 (ultra smooth)
        double s1 = n * n * (3.0 - 2.0 * n);
        double s2 = s1 * s1 * (3.0 - 2.0 * s1);

        double blend = std::clamp(s2, 0.0, 1.0);

        tsl::Color color = {
            static_cast<u8>(staticColor.r + (dynamicLogoRGB2.r - staticColor.r) * blend),
            static_cast<u8>(staticColor.g + (dynamicLogoRGB2.g - staticColor.g) * blend),
            static_cast<u8>(staticColor.b + (dynamicLogoRGB2.b - staticColor.b) * blend),
            255
        };

        std::string ls(1, letter);

        if (useNotificationMethod)
            currentX += renderer->drawNotificationString(ls, false, currentX, y, fontSize, color).first;
        else
            currentX += renderer->drawString(ls, false, currentX, y, fontSize, color).first;
    }

    return currentX;
}


// -------------------------------------------------------------
// Rendering functions
// -------------------------------------------------------------

void BaseGui::preDraw(tsl::gfx::Renderer* renderer)
{
    static constexpr tsl::Color STATIC_GREEN = tsl::Color(80, 255, 120, 255);

    drawDynamicUltraText(
        renderer,
        LOGO_X,
        LOGO_Y,
        LOGO_LABEL_FONT_SIZE,
        STATIC_GREEN,
        false
    );
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
