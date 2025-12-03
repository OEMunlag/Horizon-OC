
"""

HOC Configurator

Copyright (C) Souldbminer

This program is free software; you can redistribute it and/or modify it
under the terms and conditions of the GNU General Public License,
version 2, as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""

import configparser
import dearpygui.dearpygui as dpg
import kip as k
import common
from pathlib import Path
import ini
import settings as s

def unimplemented():
    pass

def check_gpu_sched():
    if common.drive == 0:
        return False

    ini_path = Path(common.drive) / "atmosphere/config/system_settings.ini"
    if not ini_path.exists():
        return False

    config = configparser.ConfigParser()
    config.read(ini_path)

    return config.get("am.gpu", "gpu_scheduling_enabled", fallback="0x0").strip() == "0x1"

def toggle_gpu_sched(sender, app_data):

    ini_path = Path(common.drive) / "atmosphere/config/system_settings.ini"

    # Determine value
    value = "0x1" if app_data else "0x0"

    # Ensure the parent folder exists
    ini_path.parent.mkdir(parents=True, exist_ok=True)

    # Update using the helper function (requires str path)
    ini.set_ini_values(str(ini_path), "am.gpu", {"gpu_scheduling_enabled": value})

    common.show_popup("Success", f"Set GPU Scheduling to {app_data}")

def populate():
    offsets = list(range(0, s.mariko_gpu_offset_max + 1, s.mariko_voltage_step))
    processed_offsets = ["Disabled" if v == 0 else f"-{v} mV" for v in offsets]
    voltages = [0] + list(range(s.mariko_gpu_min_volt, s.mariko_gpu_max_volt + 1, s.mariko_voltage_step))  # 0 first for Disabled
    processed_voltages = ["Disabled" if v == 0 else f"{v} mV" for v in voltages]
    voltages_e = [0] + list(range(s.erista_gpu_min_volt, s.erista_gpu_max_volt + 1, s.mariko_voltage_step))  # 0 first for Disabled
    processed_voltages_e = ["Disabled" if v == 0 else f"{v} mV" for v in voltages_e]
    processed_voltages_default = ["Default" if v == 0 else f"{v} mV" for v in voltages]
    voltages_vmin = [0] + list(range(s.mariko_gpu_min_volt, s.mariko_gpu_max_vmin + 1, s.mariko_voltage_step))  # 0 first for Disabled
    processed_voltages_vmin = ["Disabled" if v == 0 else f"{v} mV" for v in voltages_vmin]
    voltages_vmin_e = [0] + list(range(s.erista_gpu_min_volt, s.erista_gpu_max_vmin + 1, s.mariko_voltage_step))  # 0 first for Disabled
    processed_voltages_vmin_e = ["Disabled" if v == 0 else f"{v} mV" for v in voltages_vmin_e]

    freqs_mhz_label = [f"{f} MHz" for f in s.freqs_mhz]
    
    dpg.add_separator(label="Frequencies")

    dpg.add_button(
            label="What is this?",
            callback=common.show_info_window,
            user_data="A higher GPU frequency significantly increases power draw. To sustain higher frequencies without exeeding PMIC limit, undervolt your device \nDO NOT disable GPU scheduling without a adequate undervolt as it can cause hardware damage due to the high power consumption",
            width=120,
            height=64,  # height=0 makes it match text
            small=True,
            tag="freqs_info"
    )
    dpg.add_checkbox(label="GPU Scheduling", default_value=True, tag="gpu_sched", callback=toggle_gpu_sched)

    dpg.add_separator(label="Voltages")

    dpg.add_button(
            label="What is this?",
            callback=common.show_info_window,
            user_data="Lower voltages reduce power draw and tempratures. Higher voltages can boost available clockspeeds. Choose voltages with caution, as high voltages can cause IRREPERABLE hardware damage",
            width=120,
            height=64,  # height=0 makes it match text
            small=True,
            tag="volt_info"
    )
    dpg.add_combo(
         items=processed_voltages_vmin_e,
         default_value="Disabled",
         label="Erista GPU vMin",
         callback=k.grab_kip_storage_values_no_mult,
         tag="eristaGpuVmin"
    )
    dpg.add_combo(
        items=processed_voltages_vmin,
        default_value="Disabled",
        label="Mariko GPU vMin",
        callback=k.grab_kip_storage_values_no_mult,
        tag="marikoGpuVmin"
    )

    dpg.add_combo(
        items=processed_voltages,
        default_value="Disabled",
        label="Mariko GPU vMax",
        callback=k.grab_kip_storage_values_no_mult,
        tag="marikoGpuVmax"
    )

    dpg.add_separator(label="Undervolt")

    dpg.add_button(
        label="What is this?",
        callback=common.show_info_window,
        user_data="A undervolted device consumes less power, heats up less, which enables higher clocks. \nThe GPU Undervolt mode should be set to Default or High, test which one is suitable to your console. Custom is only meant for ADVANCED USERS, as the values in it could cause HARDWARE DAMAGE! \nThe Offset should be kept at the minumum your GPU requires to function at the high ram clock (high ram clock means more GPU power needed)",
        width=120,
        height=64,  # height=0 makes it match text
        small=True,
        tag="uv_info"
    )
    dpg.add_combo(
        items=["No Table (UV0)", "Regular Table (UV1)", "High Table (UV2)", "Custom Table (UV3)"],
        default_value="No Table (UV0)",
        label="Mariko Undervolt Mode",
        callback=k.grab_kip_storage_values_no_mult,
        tag="marikoGpuUV"
    )
    dpg.add_combo(
        items=["No Table (UV0)", "Regular Table (UV1)", "High Table (UV2)", "Custom Table (UV3)"],
        default_value="No Table (UV0)",
        label="Erista Undervolt Mode",
        callback=k.grab_kip_storage_values_no_mult,
        tag="eristaGpuUV"
    )
    dpg.add_combo(
        items=processed_offsets,
        default_value="Disabled",
        label="GPU Volt Offset",
        callback=k.grab_kip_storage_values_no_mult,
        tag="commonGpuVoltOffset"
    )

    dpg.add_combo(
        items=["Disabled (0)", "Enabled (1)"],
        default_value="Disabled",
        label="Mariko GPU PLL Unlock",
        callback=k.grab_kip_storage_values_no_mult,
        tag="marikoGpuFullUnlock"
    )


    dpg.add_separator(label="Custom Table (Mariko)")

    for freq in s.freqs_khz:
        if(freq > s.mariko_meme_threshold):
            mhz_color = s.danger_color
            mhz_label = f"{freq / 1000:.1f} MHz"
            safety_label = ""
        elif(freq > s.mariko_dangerous_gpu_threshold):
            mhz_color = s.danger_color
            mhz_label = f"{freq / 1000:.1f} MHz"
            safety_label = "(DANGEROUS)"
        elif(freq > s.mariko_unsafe_gpu_threshold):
            mhz_color = s.unsafe_color
            mhz_label = f"{freq / 1000:.1f} MHz"
            safety_label = "(UNSAFE)"
        else:
            mhz_label = f"{freq / 1000:.1f} MHz"
            safety_label = ""
            mhz_color = s.safe_color

        mhz_tag = f"combo_{freq}"
        if freq == s.mariko_meme_threshold:
            with dpg.group(horizontal=True):  # align horizontally
                dpg.add_combo(
                    items=processed_voltages,
                    default_value="Disabled",
                    tag="g_volt_" + str(freq),
                    callback=k.grab_kip_storage_values_no_mult
                )
                dpg.add_text(f"{mhz_label} (", color=mhz_color)
                dpg.add_image("coolerhd", width=20, height=20)
                dpg.add_text(")", color=mhz_color)
        else:
            with dpg.group(horizontal=True):
                dpg.add_combo(
                    items=processed_voltages,
                    default_value="Disabled",
                    tag="g_volt_" + str(freq),
                    callback=k.grab_kip_storage_values_no_mult
                )
                dpg.add_text(default_value=f"{mhz_label} {safety_label}", color=mhz_color)

    dpg.add_separator(label="Custom Table (Erista)")

    for freq in s.freqs_khz_e:
        if(freq > s.erista_dangerous_gpu_threshold):
            mhz_label = f"{freq / 1000:.1f} MHz"
            safety_label = "(DANGEROUS)"
            mhz_color = s.danger_color
        elif(freq > s.erista_unsafe_gpu_threshold):
            mhz_color = s.unsafe_color
            mhz_label = f"{freq / 1000:.1f} MHz"
            safety_label = "(UNSAFE)"
        else:
            mhz_color = s.safe_color
            mhz_label = f"{freq / 1000:.1f} MHz"
            safety_label = ""

        with dpg.group(horizontal=True):
            dpg.add_combo(
                items=processed_voltages_e,
                default_value="Disabled",
                tag="g_volt_e_" + str(freq),
                callback=k.grab_kip_storage_values_no_mult
            )
            dpg.add_text(default_value=f"{mhz_label} {safety_label}", color=mhz_color)
