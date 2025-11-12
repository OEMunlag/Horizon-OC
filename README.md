
<div align="center">

<img src="assets/logo.png" alt="logo" width="160"/>

---

![License: GPL-2.0](https://img.shields.io/badge/GPL--2.0-red?style=for-the-badge)
![Nintendo Switch](https://img.shields.io/badge/Nintendo_Switch-E60012?style=for-the-badge\&logo=nintendo-switch\&logoColor=white)
[![Discord](https://img.shields.io/badge/Discord-5865F2?style=for-the-badge\&logo=discord\&logoColor=white)](https://discord.com/invite/S3eX47dHsB)
![VSCode](https://img.shields.io/badge/VSCode-0078D4?style=for-the-badge\&logo=visual%20studio%20code\&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge\&logo=c%2B%2B\&logoColor=white)
![Downloads](https://img.shields.io/github/downloads/souldbminersmwc/Horizon-OC/total.svg?style=for-the-badge)

---

</div>

## ⚠️ Disclaimer

> **THIS TOOL CAN BE DANGEROUS IF MISUSED. PROCEED WITH CAUTION.**
> Due to the design of Horizon OS, **overclocking RAM can cause NAND DAMAGE.**
> Ensure you have a **full NAND backup** before proceeding.

---

## 🌀 About

**Horizon OC** is an open-source overclocking tool for Nintendo Switch consoles running **Atmosphere custom firmware**.
It enables advanced CPU, GPU, and RAM tuning with user-friendly configuration tools.

---

## 🚀 Features

* **CPU:** Up to `2397MHz` (Mariko) / `2091MHz` (Erista)
* **GPU:** Up to `1305MHz` (Mariko) / `998MHz` (Erista)
* **RAM:** Up to `3200MHz` (Mariko) / `2360MHz` (Erista)
* Over/undervolting support
* Built-in configurator
* Compatible with most homebrew

> ⚡ *Higher (potentially dangerous) frequencies are unlockable.*
> ⚙️ *Exact maximum values vary per console.*

---

## 🧩 Installation

1. Ensure you have the latest versions of

   * [Atmosphere](https://github.com/Atmosphere-NX/Atmosphere)
   * [Ultrahand Overlay](https://github.com/ppkantorski/Ultrahand-Overlay)
2. Download the latest **hoc.kip** file from the [Releases](../../releases) tab.
3. If using **Hekate**, edit `hekate_ipl.ini` to include:

   ```
   kip1=atmosphere/kips/*
   ```

   *(No changes needed if using fusee.)*
4. Download and extract the **Horizon OC sysmodule** to the root of your SD card.
5. Alternatively, use the **Configurator** and click the **Install** buttons for automatic setup.

---

## ⚙️ Configuration

1. Download the latest **Configurator** on your computer.
2. Run the executable.
3. Select your SD card or UMS drive.
4. Adjust overclocking settings as desired.
5. Click **Save** to apply your configuration.

---

## 🧱 Building from Source

1. Set up a development environment for compiling **Atmosphere**.
2. Clone Atmosphere:

   ```bash
   git clone https://github.com/Atmosphere-NX/Atmosphere.git
   ```
3. Move the cloned folder into `build/`.
4. Insert your `Source/stratosphere` folder into `build/`.
5. Run:

   ```bash
   ./build.sh
   ```

To build the Configurator:

```bash
cd Source/Configurator
build.bat
# or
python -m PyInstaller --onefile --add-data "assets;assets" --icon=assets/icon.ico --noconsole src/main.py
```

---

## 💎 Credits

* **Lightos** & **Dominatorul** – RAM timings
* **KazushiMe** & **meha** – Switch-OC-Suite
* **sys-clk team** – sys-clk
* **b0rd2death** – Ultrahand sys-clk fork
* **Lightos** & **Sammybigio2011** – Early testing

