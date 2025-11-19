/*
    sys-clk manager, a sys-clk frontend homebrew
    Copyright (C) 2019-2020  natinusala
    Copyright (C) 2019  p-sam
    Copyright (C) 2019  m4xw

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "cheat_sheet_tab.h"

#include <borealis.hpp>

CheatSheetTab::CheatSheetTab()
{
    // CPU
    this->addView(new brls::Header("CPU Clocks"));
    brls::Table *cpuTable = new brls::Table();
    
    cpuTable->addRow(brls::TableRowType::BODY, "Mariko Absolute Max", "2601 MHz");
    cpuTable->addRow(brls::TableRowType::BODY, "Mariko Unsafe Max", "2397 MHz");

    cpuTable->addRow(brls::TableRowType::BODY, "Erista Absolute Max", "2295 MHz");

    cpuTable->addRow(brls::TableRowType::BODY, "Erista Unsafe Max", "2091 MHz");
    cpuTable->addRow(brls::TableRowType::BODY, "Mariko Safe Max", "1963 MHz");
    cpuTable->addRow(brls::TableRowType::BODY, "Erista Safe Max", "1785 MHz");
    cpuTable->addRow(brls::TableRowType::BODY, "Official Docked and Handheld", "1020 MHz");
    cpuTable->addRow(brls::TableRowType::BODY, "Sleep mode", "612 MHz");

    this->addView(cpuTable);

    // GPU
    this->addView(new brls::Header("GPU Clocks"));
    brls::Table *gpuTable = new brls::Table();

    gpuTable->addRow(brls::TableRowType::BODY, "Mariko Absolute Max", "1536 MHz");

    gpuTable->addRow(brls::TableRowType::BODY, "Mariko Unsafe Max", "1267 MHz");

    gpuTable->addRow(brls::TableRowType::BODY, "Mariko Safe Max", "1152 MHz");
    gpuTable->addRow(brls::TableRowType::BODY, "Erista Absolute Max", "1075 MHz");

    gpuTable->addRow(brls::TableRowType::BODY, "Erista Safe Max", "860 MHz");

    gpuTable->addRow(brls::TableRowType::BODY, "Official Docked", "768 MHz");

    gpuTable->addRow(brls::TableRowType::BODY, "Maximum Mariko Handheld", "614 MHz");
    gpuTable->addRow(brls::TableRowType::BODY, "Maximum Erista Handheld", "460 MHz");

    gpuTable->addRow(brls::TableRowType::BODY, "Official Handheld", "307-460 MHz");
    gpuTable->addRow(brls::TableRowType::BODY, "Boost Mode", "76 MHz");

    this->addView(gpuTable);

    // MEM
    this->addView(new brls::Header("MEM Clocks"));
    brls::Table *memTable = new brls::Table();

    memTable->addRow(brls::TableRowType::BODY, "Mariko 2133BL Max", "3500 MHz");

    memTable->addRow(brls::TableRowType::BODY, "Mariko 1866BL Max", "3200 MHz");

    memTable->addRow(brls::TableRowType::BODY, "Mariko 1600BL Max", "2900 MHz");

    memTable->addRow(brls::TableRowType::BODY, "Mariko 1331BL Max", "2500 MHz");

    memTable->addRow(brls::TableRowType::BODY, "Erista 2133BL Max", "2360 MHz");

    memTable->addRow(brls::TableRowType::BODY, "Erista 1600BL Max", "2227 MHz");

    memTable->addRow(brls::TableRowType::BODY, "Mariko 4266MT/s Safe Max", "2133 MHz");

    memTable->addRow(brls::TableRowType::BODY, "Mariko 3766MT/s Safe Max", "1866 MHz");

    memTable->addRow(brls::TableRowType::BODY, "Erista Unsafe Max", "1862 MHz");

    memTable->addRow(brls::TableRowType::BODY, "Erista Safe Max", "1600 MHz");

    memTable->addRow(brls::TableRowType::BODY, "Official Handheld", "1331 MHz");

    memTable->addRow(brls::TableRowType::BODY, "Sleep Mode", "204 MHz");

    this->addView(memTable);

    this->addView(new brls::Header("Display Freqs"));
    brls::Table *lcdTable = new brls::Table();

    lcdTable->addRow(brls::TableRowType::BODY, "External Display (720P) Max", "240 HZ");

    lcdTable->addRow(brls::TableRowType::BODY, "External Display (1080P) Max", "120 HZ");

    lcdTable->addRow(brls::TableRowType::BODY, "OLED Max", "90 HZ");

    lcdTable->addRow(brls::TableRowType::BODY, "LCD Max", "72 HZ");

    lcdTable->addRow(brls::TableRowType::BODY, "Default", "60 HZ");

    lcdTable->addRow(brls::TableRowType::BODY, "Minimum", "40 HZ");

    this->addView(lcdTable);
}

void CheatSheetTab::customSpacing(brls::View* current, brls::View* next, int* spacing)
{
    if (dynamic_cast<brls::Table*>(current))
        *spacing = 0;
    else
        List::customSpacing(current, next, spacing);
}
