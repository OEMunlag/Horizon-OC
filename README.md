
<div align="center">

<img src="assets/logo.png" alt="logo" width="350"/>

---

![License: GPL-2.0](https://img.shields.io/badge/GPL--2.0-red?style=for-the-badge)
![Nintendo Switch](https://img.shields.io/badge/Nintendo_Switch-E60012?style=for-the-badge\&logo=nintendo-switch\&logoColor=white)
[![Discord](https://img.shields.io/badge/Discord-5865F2?style=for-the-badge\&logo=discord\&logoColor=white)](https://discord.com/invite/S3eX47dHsB)
![VSCode](https://img.shields.io/badge/VSCode-0078D4?style=for-the-badge\&logo=visual%20studio%20code\&logoColor=white)
![Made with Notepad++](assets/np++.png?raw=true)
![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge\&logo=c%2B%2B\&logoColor=white)
![Downloads](https://img.shields.io/github/downloads/souldbminersmwc/Horizon-OC/total.svg?style=for-the-badge)

---

</div>

## ⚠️ Disclaimer

> **THIS TOOL CAN BE DANGEROUS IF MISUSED. PROCEED WITH CAUTION.**
> Due to the design of Horizon OS, **overclocking RAM can cause NAND OR SD CORRUPTION.**
> Ensure you have a **full NAND, PRODINFO, EMUMMC and SD backup** before proceeding.

---

## About

**Horizon OC** is an open-source overclocking tool for Nintendo Switch consoles running **Atmosphere custom firmware**.
It enables advanced CPU, GPU, and RAM tuning with user-friendly configuration tools.

---

## Features

* **CPU:** Up to 1963MHz (Mariko) / 1785MHz (Erista)
* **GPU:** Up to 1075MHz (Mariko) / 921MHz (Erista)
* **RAM:** Up to 1866MHz (Mariko) / 1600MHz (Erista)
* Over/undervolting support
* Built-in configurator
* Compatible with most homebrew

> *Higher (potentially dangerous) frequencies are unlockable via configuration.*
> *Erista and Mariko units can usually push a bit further fully safely with a bit of undervolting, however this may not work on all units.*
> *The exact maximum overclock possible varies per console, although most consoles should be able to do this safely.*

---

## Installation

1. Ensure you have the latest versions of

   * [Atmosphere](https://github.com/Atmosphere-NX/Atmosphere)
   * [Ultrahand Overlay](https://github.com/ppkantorski/Ultrahand-Overlay)
2. Download and extract the **Horizon OC Package** to the root of your SD card.
3. If using **Hekate**, edit `hekate_ipl.ini` to include:

   ```
   kip1=atmosphere/kips/hoc.kip
   secmon=exosphere.bin
   ```

   *(No changes needed if using fusee.)*

---

## Configuration

1. Open the Horizon OC Overlay
2. Open the settings menu
3. Adjust your overclocking settings as desired.
4. Click **Save KIP Settings** to apply your configuration.

---

## Building from Source

Refer to COMPILATION.md

---

## Credits
* **Lightos's Cat** - Cat

* **Souldbminer** – hoc-clk and loader development
* **Lightos** – loader patches development
* **SciresM** - Atmosphere CFW
* **KazushiMe** – Switch OC Suite
* **hanai3bi (meha)** – Switch OC Suite, EOS, sys-clk-eos
* **NaGaa95** – L4T-OC-kernel
* **B3711 (halop)** – EOS
* **sys-clk team (m4xw, p-sam, natinusala)** – sys-clk
* **b0rd2death** – Ultrahand sys-clk & Status Monitor fork
* **MasaGratoR and ZachyCatGames** - General help
* **MasaGratoR** - Status Monitor & Display Refresh Rate Driver
* **Dom, Samybigio, Arcdelta, Miki, Happy, Flopsider, Winnerboi77, Blaise, Alvise, TDRR, agjeococh and Xenshen** - Testing
* **Samybigio2011** - Italian translations
