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
#include "about_gui.h"
#include "../format.h"
#include <tesla.hpp>
#include <string>


AboutGui::AboutGui()
{
}

AboutGui::~AboutGui()
{
}

void AboutGui::listUI()
{
    this->listElement->addItem(
        new tsl::elm::CategoryHeader("Developers")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("Souldbminer")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("Lightos_")
    );

    // ---- Contributors ----
    this->listElement->addItem(
        new tsl::elm::CategoryHeader("Contributors")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("Dom")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("Blaise25")
    );

    // ---- Testers ----
    this->listElement->addItem(
        new tsl::elm::CategoryHeader("Testers")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("Dom")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("Samybigio2011")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("Delta")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("Miki1305")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("Happy")
    );
    
    this->listElement->addItem(
        new tsl::elm::ListItem("Flopsider")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("Winnerboi77")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("Blaise25")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("WE1ZARD")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("Alvise")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("TDRR")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("agjeococh")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("Xenshen")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("Frost")
    );

    // ---- Special Thanks ----
    this->listElement->addItem(
        new tsl::elm::CategoryHeader("Special Thanks")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("ScriesM - Atmosphere CFW")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("KazushiMe - Switch OC Suite")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("hanai3bi - Switch OC Suite & EOS")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("NaGaa95 - L4T-OC-Kernel")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("B3711 - EOS")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("RetroNX - sys-clk")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("b0rd2death - Ultrahand")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("MasaGratoR - Status Monitor")
    );

    this->listElement->addItem(
        new tsl::elm::ListItem("The Switch Homebrew Community")
    );

}

void AboutGui::update()
{
    BaseMenuGui::update();
}

void AboutGui::refresh()
{
    BaseMenuGui::refresh();
    
    if (!this->context)
        return;

}
/*
## Credits
* **Lightos's Cat** - Cat

* **Souldbminer** – hoc-clk and loader development
* **Lightos** – loader patches development
* **SciresM** - Atmosphere CFW
* **KazushiMe** – Switch OC Suite
* **hanai3bi (meha)** – Switch OC Suite, EOS, sys-clk-eos
* **NaGaa95** – L4T-OC-kernel
* **B3711 (halop)** – EOS
* **sys-clk team (m4xw, p-sam, nautalis)** – sys-clk
* **b0rd2death** – Ultrahand sys-clk & Status Monitor fork
* **MasaGratoR and ZachyCatGames** - General help
* **MasaGratoR** - Status Monitor & Display Refresh Rate Driver
* **Dom, Samybigio, Arcdelta, Miki, Happy, Flopsider, Winnerboi77, Blaise, Alvise, TDRR, agjeococh and Xenshen** - Testing
* **Samybigio2011** - Italian translations
*/