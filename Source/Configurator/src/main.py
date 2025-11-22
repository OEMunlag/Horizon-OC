
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

import dearpygui.dearpygui as dpg

# This prevents the window from reopening when closed
dpg.create_context()

from PIL import Image
import numpy as np
import common as c
import sys
import os
import webbrowser
import ctypes

import kip as k
import gpu
import cpu
import ram
from defaults import d
import installer as ins
import about
import misc

true = True
false = False

if getattr(sys, 'frozen', False):
    assets_path = os.path.join(sys._MEIPASS, 'assets/')
else:
    assets_path = os.path.join(os.path.dirname(__file__), '../assets/')

cooler_image_path = assets_path + "coolerhd.png" # coolerHD Emoji from OC server
cooler_image = Image.open(cooler_image_path).convert("RGBA")
cooler_width, cooler_height = cooler_image.size

cooler_image_data = np.array(cooler_image) / 255.0
cooler_image_data = cooler_image_data.flatten()

lightos_image_path = assets_path + "lightos_c.png"
lightos_image = Image.open(lightos_image_path).convert("RGBA")
lightos_width, lightos_height = lightos_image.size

lightos_image_data = np.array(lightos_image) / 255.0
lightos_image_data = lightos_image_data.flatten()

samy_image_path = assets_path + "samy_c.png"
samy_image = Image.open(samy_image_path).convert("RGBA")
samy_width, samy_height = samy_image.size

samy_image_data = np.array(samy_image) / 255.0
samy_image_data = samy_image_data.flatten()

soul_image_path = assets_path + "soul_c.png"
soul_image = Image.open(soul_image_path).convert("RGBA")
soul_width, soul_height = soul_image.size

soul_image_data = np.array(soul_image) / 255.0
soul_image_data = soul_image_data.flatten()

with dpg.font_registry():
    lexend = dpg.add_font(assets_path + "Lexend.ttf", 16)

with dpg.texture_registry(show=False):
    dpg.add_static_texture(width=cooler_width, height=cooler_height, default_value=cooler_image_data, tag="coolerhd")

    dpg.add_static_texture(width=lightos_width, height=lightos_height, default_value=lightos_image_data, tag="lightos")

    dpg.add_static_texture(width=samy_width, height=samy_height, default_value=samy_image_data, tag="samy")

    dpg.add_static_texture(width=soul_width, height=soul_height, default_value=soul_image_data, tag="soul")

with dpg.file_dialog(directory_selector=False, show=False, tag="file_dialog", width=500, height=300, modal=True, callback=k.store):
    dpg.add_file_extension(".kip")
def refresh_drives():
    setattr(ins, "drive_list", ins.get_drives())
    dpg.configure_item("drive_select", items=ins.drive_list)
with dpg.window(label="Configurator", width=400, height=300,id="main_window"):
    with dpg.tab_bar():
        with dpg.tab(label="File"):
            dpg.add_separator(label="Information")
            dpg.add_text("Horizon OC is an open source overclocking tool for the Nintendo Switch Horizon OS")
            dpg.add_text("For more information read the README on GitHub")
            dpg.add_button(
                label="Open GitHub",
                callback=lambda: webbrowser.open("https://github.com/souldbminersmwc/Horizon-OC")
            )
            dpg.add_separator(label="Installation")
            dpg.add_button(label="Refresh Drive List", callback=refresh_drives)
            dpg.add_combo(
                items=ins.drive_list,
                default_value="Select a drive!",
                tag="drive_select",
                callback=ins.check_atmosphere
            )
            dpg.add_checkbox(label="Autosave", default_value=false, callback=ins.autosave_toggle)
            dpg.add_text("", tag="status_text")
            dpg.add_button(label="Save settings", callback=k.write_kip)
            dpg.add_separator(label="Downloads")
            dpg.add_button(label="Install Horizon OC Loader", callback=ins.downloadLoader)
            dpg.add_button(label="Install Horizon OC Clock Manager", callback=lambda: ins.download_and_extract_zip(c, ins.hoc_clk_download_link, zip_filename="hoc_clk_temp.zip", success_message="Installed hoc-clk!"))
            dpg.add_separator(label="Advanced")
            dpg.add_button(label="Manually Select hoc.kip", callback=lambda: dpg.show_item("file_dialog"))
        with dpg.tab(label="GPU", tag="gpu_tab"):
            gpu.populate()

        with dpg.tab(label="CPU", tag="cpu_tab"):
            cpu.populate()

        with dpg.tab(label="RAM", tag="emc_tab"):
            ram.populate()

        with dpg.tab(label="Misc", tag="misc_tab"):
            misc.populate()

        with dpg.tab(label="About", tag="about_tab"):
            about.populate()

        dpg.hide_item("gpu_tab")
        dpg.hide_item("cpu_tab")
        dpg.hide_item("emc_tab")
        dpg.hide_item("misc_tab")

dpg.bind_font(lexend)

dpg.create_viewport(title="Horizon OC Configurator", width=854, height=480)
dpg.set_viewport_large_icon(assets_path + "icon.ico")
dpg.set_viewport_title("Horizon OC Configurator")
dpg.set_primary_window("main_window", True)

dpg.setup_dearpygui()
dpg.show_viewport()

dpg.start_dearpygui()

dpg.destroy_context()
