"""

HOC Timing Tool

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

import zipfile
import tempfile
from pathlib import Path
import re
import dearpygui.dearpygui as dpg
import numpy as np
import os
import sys
from scipy.signal import savgol_filter

REGISTER_RE = re.compile(r"^(emc|mc)_[A-Za-z0-9_]+\s+0x[0-9A-Fa-f]+$")
if getattr(sys, 'frozen', False):
    assets_path = os.path.join(sys._MEIPASS, 'assets/')
else:
    assets_path = os.path.join(os.path.dirname(__file__), '../assets/')

def safe_r2(y, y_fit):
    ss_res = np.sum((y - y_fit) ** 2)
    ss_tot = np.sum((y - np.mean(y)) ** 2)
    if ss_tot == 0:
        return 0.0
    return max(0.0, 1 - ss_res / ss_tot)

def find_inflection_points(x, y):
    x = np.array(x, dtype=float)
    y = np.array(y, dtype=float)
    
    if len(x) < 3:
        return []
    
    dx = np.diff(x)
    dy = np.diff(y)
    slopes = dy / dx
    
    slope_changes = np.abs(np.diff(slopes))
    
    if len(slope_changes) > 0:
        threshold = np.percentile(slope_changes, 40)
    else:
        return []
    
    inflections = []
    for i in range(len(slope_changes)):
        if slope_changes[i] > threshold:
            inflections.append(i + 1)
    
    inflections = sorted(set(inflections))
    
    if len(inflections) < 2 and len(slope_changes) > 0:
        threshold = np.percentile(slope_changes, 60)
        inflections = []
        for i in range(len(slope_changes)):
            if slope_changes[i] > threshold:
                inflections.append(i + 1)
        inflections = sorted(set(inflections))
    
    return inflections

def fit_piecewise_segments(x, y, reg_name="register"):
    x = np.array(x, dtype=float)
    y = np.array(y, dtype=float)
    
    if len(x) < 3:
        return None
    
    inflections = find_inflection_points(x, y)
    
    breakpoints = [0] + inflections + [len(x) - 1]
    breakpoints = sorted(set(breakpoints))
    
    segments = []
    thresholds = []
    slopes = []
    intercepts = []
    
    for i in range(len(breakpoints) - 1):
        start_idx = breakpoints[i]
        end_idx = breakpoints[i + 1]
        
        x_seg = x[start_idx:end_idx + 1]
        y_seg = y[start_idx:end_idx + 1]
        
        if len(x_seg) < 2:
            continue
        
        try:
            p = np.polyfit(x_seg, y_seg, 1)
            slope, intercept = p[0], p[1]
            
            thresholds.append(x[end_idx])
            slopes.append(slope)
            intercepts.append(intercept)
        except Exception:
            continue
    
    if not thresholds:
        return None
    
    def piecewise(t, thresholds_list=thresholds, slopes_list=slopes, intercepts_list=intercepts):
        if np.isscalar(t):
            for thresh, slp, intcpt in zip(thresholds_list, slopes_list, intercepts_list):
                if t <= thresh:
                    return slp * t + intcpt
            return slopes_list[-1] * t + intercepts_list[-1]
        else:
            result = np.zeros_like(t, dtype=float)
            for i, ti in enumerate(t):
                for thresh, slp, intcpt in zip(thresholds_list, slopes_list, intercepts_list):
                    if ti <= thresh:
                        result[i] = slp * ti + intcpt
                        break
                else:
                    result[i] = slopes_list[-1] * ti + intercepts_list[-1]
            return result
    
    y_fit = piecewise(x)
    r2 = safe_r2(y, y_fit)
    
    formula_lines = []
    for i, (thresh, slp, intcpt) in enumerate(zip(thresholds, slopes, intercepts)):
        if abs(slp) < 1e-6:
            val_str = f"{intcpt:.0f}"
        else:
            slp_simple = round(slp, 4)
            intcpt_simple = round(intcpt, 0)
            if slp_simple == int(slp_simple):
                slp_simple = int(slp_simple)
            if intcpt_simple >= 0:
                val_str = f"{slp_simple} * x + {intcpt_simple}"
            else:
                val_str = f"{slp_simple} * x - {abs(intcpt_simple)}"
        
        if i == len(thresholds) - 1:
            formula_lines.append(f"    return {val_str};")
        else:
            formula_lines.append(f"    if (x <= {thresh:.0f}) return {val_str};")
    
    formula = "float timing(float x) {\n" + "\n".join(formula_lines) + "\n}"
    
    advanced_lines = []
    max_freq = max(x) if len(x) > 0 else 0
    
    for i, (thresh, slp, intcpt) in enumerate(zip(thresholds, slopes, intercepts)):
        if abs(slp) < 1e-6:
            val = f"{intcpt:.0f}"
        else:
            slp_simple = round(slp, 4)
            intcpt_simple = round(intcpt, 0)
            if slp_simple == int(slp_simple):
                slp_simple = int(slp_simple)
            if intcpt_simple >= 0:
                val = f"({slp_simple} * freq + {intcpt_simple})"
            else:
                val = f"({slp_simple} * freq - {abs(intcpt_simple)})"
        
        if thresh >= max_freq - 1:
            advanced_lines.append(f"WRITE_ALL_PARAM_REG(EMC_{reg_name}, {val});")
        elif i == 0:
            advanced_lines.append(f"if (freq <= {thresh:.0f}) {{")
            advanced_lines.append(f"    WRITE_ALL_PARAM_REG(EMC_{reg_name}, {val});")
            advanced_lines.append("}")
        else:
            advanced_lines.append(f"else if (freq <= {thresh:.0f}) {{")
            advanced_lines.append(f"    WRITE_ALL_PARAM_REG(EMC_{reg_name}, {val});")
            advanced_lines.append("}")
    
    advanced_formula = "\n".join(advanced_lines)
    
    return {
        'fn': piecewise,
        'formula': formula,
        'advanced_formula': advanced_formula,
        'r2': r2,
        'thresholds': thresholds,
        'slopes': slopes,
        'intercepts': intercepts,
        'reg_name': reg_name
    }


def parse_dump_file(path: Path):
    registers = {}
    try:
        for line in path.read_text(errors="ignore").splitlines():
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split()
            if len(parts) < 2:
                continue
            name, val = parts[0], parts[-1]
            if not (name.lower().startswith("emc_") or name.lower().startswith("mc_")):
                continue
            if not val.startswith("0x"):
                continue
            try:
                registers[name] = int(val, 16)
            except ValueError:
                pass
    except Exception:
        pass
    return registers


def analyze_zip(zip_path: Path):
    tmpdir = Path(tempfile.mkdtemp(prefix="timingtool_extract_"))
    with zipfile.ZipFile(zip_path, "r") as z:
        z.extractall(tmpdir)

    results = {}
    for base_dir in tmpdir.iterdir():
        if not base_dir.is_dir():
            continue
        base_latency = base_dir.name
        results.setdefault(base_latency, {"mc": {}, "emc": {}})

        for typ in ("mc", "emc"):
            folder = base_dir / typ
            if not folder.exists():
                continue
            for dump in folder.glob("*.txt"):
                m = re.search(r"(\d+)", dump.name)
                if not m:
                    continue
                freq = int(m.group(1))
                registers = parse_dump_file(dump)
                for reg, val in registers.items():
                    results[base_latency][typ].setdefault(reg, {})[freq] = val

    return results



dpg.create_context()
dpg.create_viewport(title="Horizon OC Timing Tool", width=1920, height=1080)
dpg.maximize_viewport()

with dpg.font_registry():
    lexend = dpg.add_font(assets_path + "Lexend.ttf", 16)

with dpg.window(label="HOC Timing Tool", width=1920, height=1080, tag="main_window"):
    with dpg.tab_bar(tag="root_tabs"):
        with dpg.tab(label=" File", tag="file_tab"):
            dpg.add_text("Timing Analyzer\nSelect a ZIP file structured as:\n<base_latency>/<mc|emc>/<freq>_mc.txt")
            dpg.add_button(label="Select ZIP File...", callback=lambda s,a: dpg.show_item("file_dialog"))
            dpg.add_separator()
            dpg.add_text("Status:")
            dpg.add_text("Waiting...", tag="status_text")

        with dpg.tab(label="Graphs", tag="graph_tab"):
            with dpg.tab_bar(tag="main_tabs"):
                dpg.add_tab(label="No Data", tag="placeholder_tab")
        
        with dpg.tab(label="Code", tag="code_tab"):
            with dpg.tab_bar(tag="code_tabs"):
                dpg.add_tab(label="No Data", tag="code_placeholder_tab")


def handle_file_selection(sender, app_data):
    if not app_data["selections"]:
        return
    zip_path = list(app_data["selections"].values())[0]
    dpg.set_value("status_text", f"Analyzing {zip_path} ...")
    try:
        data = analyze_zip(Path(zip_path))
    except Exception as e:
        dpg.set_value("status_text", f"Error: {e}")
        return

    dpg.delete_item("main_tabs", children_only=True)

    if not data:
        dpg.add_tab(label="No valid data", parent="main_tabs")
        dpg.set_value("status_text", "No valid data found in ZIP.")
        return

    dpg.delete_item("code_tabs", children_only=True)

    for base_latency, lat_data in sorted(data.items()):
        with dpg.tab(label=f"{base_latency}bl", parent="main_tabs"):
            with dpg.tab_bar():
                for typ in ("mc", "emc"):
                    with dpg.tab(label=typ.upper()):
                        if not lat_data[typ]:
                            dpg.add_text(f"No {typ.upper()} data.")
                            continue

                        search_tag = f"search_{base_latency}_{typ}"
                        dpg.add_input_text(label="Search Timings", tag=search_tag, width=500)

                        with dpg.child_window(width=-1, height=850, horizontal_scrollbar=True) as scroll_area:
                            for reg_name, freq_map in sorted(lat_data[typ].items()):
                                freqs = sorted(freq_map.keys())
                                vals = [freq_map[f] for f in freqs]
                                if len(freqs) < 2:
                                    continue

                                x = np.array(freqs, dtype=float)
                                y = np.array(vals, dtype=float)
                                
                                fit_result = fit_piecewise_segments(x, y, reg_name)
                                
                                if fit_result is None:
                                    continue

                                plot_tag = f"{base_latency}_{typ}_{reg_name}_plot"
                                container_tag = f"{plot_tag}_container"
                                dropdown_tag = f"{plot_tag}_dropdown"
                                value_tag = f"{plot_tag}_value"

                                with dpg.group(tag=container_tag):
                                    with dpg.plot(label=reg_name, height=250, width=-1):
                                        dpg.add_plot_legend()
                                        dpg.add_plot_axis(dpg.mvXAxis, label="Frequency (MHz)")
                                        y_axis = dpg.add_plot_axis(dpg.mvYAxis, label="Register")
                                        dpg.add_line_series(freqs, vals, label="Data", parent=y_axis)
                                        
                                        fit_x = np.linspace(min(freqs), max(freqs), 100)
                                        fit_y = fit_result['fn'](fit_x)
                                        dpg.add_line_series(fit_x, fit_y, label=f"Fit (R²={fit_result['r2']:.3f})", parent=y_axis)

                                    dpg.add_text(f"R² = {fit_result['r2']:.4f}", color=(100, 200, 100))
                                    
                                    with dpg.tab_bar():
                                        with dpg.tab(label="Timing Function"):
                                            dpg.add_input_text(default_value=fit_result['formula'], readonly=True, width=-1, height=150, multiline=True, tag=f"{container_tag}_formula")
                                        with dpg.tab(label="Register Write"):
                                            dpg.add_input_text(default_value=fit_result['advanced_formula'], readonly=True, width=-1, height=150, multiline=True, tag=f"{container_tag}_advanced")

                                    def make_freq_callback(freq_map, val_tag):
                                        def _callback(sender, app_data):
                                            freq = int(app_data)
                                            val = freq_map.get(freq)
                                            if val is not None:
                                                dpg.set_value(val_tag, f"Value: 0x{val:08X} ({val})")
                                            else:
                                                dpg.set_value(val_tag, "Value: N/A")
                                        return _callback

                                    dpg.add_combo(
                                        items=[str(f) for f in freqs],
                                        label="Select Frequency",
                                        default_value=str(freqs[0]),
                                        width=150,
                                        callback=make_freq_callback(freq_map, value_tag),
                                        tag=dropdown_tag
                                    )
                                    dpg.add_text(f"Value: 0x{vals[0]:08X} ({vals[0]})", tag=value_tag)
                        
                        def make_filter_closure(scroll_area, search_tag, lat_data=lat_data[typ], base=base_latency, t=typ):
                            def _filter(sender, app_data):
                                query = app_data.strip().lower()
                                for reg_name in lat_data.keys():
                                    container_tag = f"{base}_{t}_{reg_name}_plot_container"
                                    visible = query in reg_name.lower() if query else True
                                    if dpg.does_item_exist(container_tag):
                                        dpg.configure_item(container_tag, show=visible)
                            return _filter

                        dpg.set_item_callback(search_tag, make_filter_closure(scroll_area, search_tag))

    for base_latency, lat_data in sorted(data.items()):
        code_content = ""
        with dpg.tab(label=f"{base_latency}bl", parent="code_tabs"):
            with dpg.tab_bar():
                for typ in ("mc", "emc"):
                    with dpg.tab(label=typ.upper()):
                        typ_code = ""
                        if lat_data[typ]:
                            for reg_name, freq_map in sorted(lat_data[typ].items()):
                                freqs = sorted(freq_map.keys())
                                vals = [freq_map[f] for f in freqs]
                                if len(freqs) < 2:
                                    continue
                                
                                x = np.array(freqs, dtype=float)
                                y = np.array(vals, dtype=float)
                                fit_result = fit_piecewise_segments(x, y, reg_name)
                                
                                if fit_result:
                                    typ_code += fit_result['advanced_formula'] + "\n\n"
                        
                        if typ_code:
                            dpg.add_input_text(default_value=typ_code, readonly=True, width=-1, height=-1, multiline=True)
                        else:
                            dpg.add_text(f"No {typ.upper()} data.")
        
    dpg.set_value("status_text", "Done.")


with dpg.file_dialog(directory_selector=False, show=False, callback=handle_file_selection, tag="file_dialog", width=500, height=300, modal=True):
    dpg.add_file_extension(".zip")

dpg.set_primary_window("main_window", True)

dpg.bind_font(lexend)
dpg.setup_dearpygui()
dpg.show_viewport()
dpg.start_dearpygui()
dpg.destroy_context()