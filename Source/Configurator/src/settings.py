import common as c

# Links

kip_download_link="https://github.com/souldbminersmwc/Horizon-OC/releases/latest/download/loader.kip"
hoc_clk_download_link="https://github.com/souldbminersmwc/Horizon-OC/releases/latest/download/hoc-clk.zip"
nx_ovlloader_link = "https://github.com/ppkantorski/nx-ovlloader/releases/latest/download/nx-ovlloader+.zip"
ultrahand_link = "https://github.com/ppkantorski/Ultrahand-Overlay/releases/latest/download/ovlmenu.ovl"
status_monitor_link = "https://github.com/ppkantorski/Status-Monitor-Overlay/releases/latest/download/Status-Monitor-Overlay.ovl "
saltynx_link = "https://github.com/masagrator/SaltyNX/releases/latest/download/SaltyNX.zip"
reversenx_link = "https://github.com/masagrator/ReverseNX-RT/releases/latest/download/ReverseNX-RT-ovl.ovl"

# Frequencies

freqs_khz = [
    76800, 153600, 230400, 307200, 384000, 460800, 537600, 614400, 691200, 768000,
    844800, 921600, 998400, 1075200, 1152000, 1228800, 1267200, 1305600, 1344000, 1382400, 1420800,
    1459200, 1497600, 1536000
]
freqs_khz_e = [
    76800, 153600, 230400, 307200, 384000, 460800, 537600, 614400, 691200, 768000,
    844800, 921600, 998400, 1075200 #, 1152000, 1228800 # Disabled by default as these freqs can cause board damage
]

freqs_mhz = [
    76.8, 153.6, 230.4, 307.2, 384.0, 460.8, 537.6, 614.4, 691.2, 768.0,
    844.8, 921.6, 998.4, 1075.2, 1152.0, 1228.8, 1267.2, 1305.6, 1344.0, 1382.4,
    1420.8, 1459.2, 1497.6, 1536.0
]

adjusted_freqs_khz = [
0, 1600000, 1633000, 1666000, 1700000, 1733000, 1766000, 1800000, 1833000, 1866000, 1900000,
1933000, 1966000, 2000000, 2033000, 2066000, 2100000, 2133000, 2166000, 2200000, 2233000,
2266000, 2300000, 2333000, 2366000, 2400000, 2433000, 2466000, 2500000, 2533000, 2566000,
2600000, 2633000, 2666000, 2700000, 2733000, 2766000, 2800000, 2833000, 2866000, 2900000,
2933000, 2966000, 3000000, 3033000, 3066000, 3100000, 3133000, 3166000, 3200000, 3233000,
3266000, 3300000, 3333000, 3366000, 3400000, 3433000, 3466000, 3500000
]
voltages_uv = [
0, 1050000, 1062500, 1075000, 1087500, 1100000, 1112500, 1125000,
1137500, 1150000, 1162500, 1175000, 1187500, 1200000, 1212500, 1237500
]
vddq_uv = [
0, 550000, 555000, 560000, 565000, 570000, 575000, 580000, 585000, 590000, 595000,
600000, 605000, 610000, 615000, 620000, 625000, 630000, 635000, 640000, 645000,
650000
]
freqs_hz_cpu = [
    1020000, 1122000, 1224000, 1326000, 1428000, 1581000, 1683000,
    1785000, 1887000, 1963500, 2091000, 2193000, 2295000, 2397000,
    2499000, 2601000, 2703000, 2805000, 2907000
]
freqs_mhz_cpu = [
    1020.0, 1122.0, 1224.0, 1326.0, 1428.0, 1581.0, 1683.0,
    1785.0, 1887.0, 1963.5, 2091.0, 2193.0, 2295.0, 2397.0,
    2499.0, 2601.0, 2703.0, 2805.0, 2907.0
]
# Loading/saving

variables = [
    ("custRev", "u32"),
    ("mtcConf", "u32"),
    ("hpMode", "u32"),
    ("commonCpuBoostClock", "u32"),
    ("commonEmcMemVolt", "u32"),
    ("eristaCpuMaxVolt", "u32"),
    ("eristaEmcMaxClock", "u32"),
    ("marikoCpuMaxVolt", "u32"),
    ("marikoEmcMaxClock", "u32"),
    ("marikoEmcVddqVolt", "u32"),
    ("marikoCpuUV", "u32"),
    ("marikoGpuUV", "u32"),
    ("eristaCpuUV", "u32"),
    ("eristaGpuUV", "u32"),
    ("commonGpuVoltOffset", "u32"),
    ("marikoEmcDvbShift", "u32"),
    # advanced config
    ("t1_tRCD", "u32"),
    ("t2_tRP", "u32"),
    ("t3_tRAS", "u32"),
    ("t4_tRRD", "u32"),
    ("t5_tRFC", "u32"),
    ("t6_tRTW", "u32"),
    ("t7_tWTR", "u32"),
    ("t8_tREFI", "u32"),
    ("mem_burst_latency", "u32"),
    ("marikoCpuVmin", "u32"),
    ("eristaGpuVmin", "u32"),
    ("marikoGpuVmin", "u32"),
    ("marikoGpuVmax", "u32"),
    ("g_volt_76800", "u32"),
    ("g_volt_153600", "u32"),
    ("g_volt_230400", "u32"),
    ("g_volt_307200", "u32"),
    ("g_volt_384000", "u32"),
    ("g_volt_460800", "u32"),
    ("g_volt_537600", "u32"),
    ("g_volt_614400", "u32"),
    ("g_volt_691200", "u32"),
    ("g_volt_768000", "u32"),
    ("g_volt_844800", "u32"),
    ("g_volt_921600", "u32"),
    ("g_volt_998400", "u32"),
    ("g_volt_1075200", "u32"),
    ("g_volt_1152000", "u32"),
    ("g_volt_1228800", "u32"),
    ("g_volt_1267200", "u32"),
    ("g_volt_1305600", "u32"),
    ("g_volt_1344000", "u32"),
    ("g_volt_1382400", "u32"),
    ("g_volt_1420800", "u32"),
    ("g_volt_1459200", "u32"),
    ("g_volt_1497600", "u32"),
    ("g_volt_1536000", "u32"),

    
    ("g_volt_e_76800", "u32"),
    ("g_volt_e_153600", "u32"),
    ("g_volt_e_230400", "u32"),
    ("g_volt_e_307200", "u32"),
    ("g_volt_e_384000", "u32"),
    ("g_volt_e_460800", "u32"),
    ("g_volt_e_537600", "u32"),
    ("g_volt_e_614400", "u32"),
    ("g_volt_e_691200", "u32"),
    ("g_volt_e_768000", "u32"),
    ("g_volt_e_844800", "u32"),
    ("g_volt_e_921600", "u32"),
    ("g_volt_e_998400", "u32"),
    ("g_volt_e_1075200", "u32"),
#    ("g_volt_e_1152000", "u32"),
#    ("g_volt_e_1228800", "u32"),


]

fmt_map = {
    "u32": "I",
    "double": "d",
}

def load_all_vars():
    c.load_entry_object("custRev", 0)
    c.load_entry_object("mtcConf", 0)
    c.load_entry_object("commonCpuBoostClock", 1)
    c.load_entry_object("commonEmcMemVolt", 2)
    c.load_entry_object("eristaCpuMaxVolt", 3)
    c.load_entry_object("eristaEmcMaxClock", 1)
    c.load_entry_object("marikoCpuMaxVolt", 3)
    c.load_entry_object("marikoEmcMaxClock", 1)
    c.load_entry_object("marikoEmcVddqVolt", 2)
    c.load_entry_object("marikoCpuUV", 5)
    c.load_entry_object("marikoGpuUV", 4)
    c.load_entry_object("eristaCpuUV", 5)
    c.load_entry_object("eristaGpuUV", 4)
    c.load_entry_object("commonGpuVoltOffset", 3)
    c.load_entry_object("marikoEmcDvbShift", 0)

    # Advanced memory config
    c.load_entry_object("t1_tRCD", 5)
    c.load_entry_object("t2_tRP", 5)
    c.load_entry_object("t3_tRAS", 5)
    c.load_entry_object("t4_tRRD", 5)
    c.load_entry_object("t5_tRFC", 5)
    c.load_entry_object("t6_tRTW", 5)
    c.load_entry_object("t7_tWTR", 5)
    c.load_entry_object("t8_tREFI", 5)
    c.load_entry_object("mem_burst_latency", 5)
    c.load_entry_object("marikoCpuVmin", 3)
    c.load_entry_object("eristaGpuVmin", 3)
    c.load_entry_object("marikoGpuVmin", 3)
    c.load_entry_object("marikoGpuVmax", 3)
    # GPU voltage arrays
    for freq in [
        "76800", "153600", "230400", "307200", "384000", "460800", "537600",
        "614400", "691200", "768000", "844800", "921600", "998400", "1075200",
        "1152000", "1228800", "1267200", "1305600", "1344000", "1382400",
        "1420800", "1459200", "1497600", "1536000"
    ]:
        c.load_entry_object(f"g_volt_{freq}", 3)

    for e_freq in [
        "76800", "153600", "230400", "307200", "384000", "460800", "537600",
        "614400", "691200", "768000", "844800", "921600", "998400", "1075200"# ,
        # "1152000", "1228800"
    ]:
        c.load_entry_object(f"g_volt_e_{e_freq}", 3)


# Timings

timing_vars = [
    "d.t1_tRCD",
    "d.t2_tRP",
    "d.t3_tRAS",
    "d.t4_tRRD",
    "d.t5_tRFC",
    "d.t6_tRTW",
    "d.t7_tWTR",
    "d.t8_tREFI"
]

timing_preset_default = [
    0, # tRCD
    0, # tRP
    0, # tRAS
    0, # tRRD
    0, # tRFC
    0, # tRTW
    0, # tWTR
    0, # tREFI
]

timing_preset_aamgcl_c = [
    4, # tRCD
    4, # tRP
    5, # tRAS
    5, # tRRD
    5, # tRFC
    5, # tRTW
    7, # tWTR
    6, # tREFI
]

timing_preset_aamgcl_st = [
    4, # tRCD
    4, # tRP
    8, # tRAS
    6, # tRRD
    5, # tRFC
    7, # tRTW
    8, # tWTR
    6, # tREFI
]

timing_preset_mgcj_c = [
    3, # tRCD
    2, # tRP
    4, # tRAS
    2, # tRRD
    4, # tRFC
    4, # tRTW
    4, # tWTR
    6, # tREFI
]

timing_preset_mgcj_st = [
    4, # tRCD
    3, # tRP
    8, # tRAS
    2, # tRRD
    5, # tRFC
    4, # tRTW
    4, # tWTR
    6, # tREFI
]

timing_preset_ab_mgcl_c = [
    4, # tRCD
    4, # tRP
    4, # tRAS
    4, # tRRD
    4, # tRFC
    5, # tRTW
    6, # tWTR
    6, # tREFI
]

timing_preset_ab_mgcl_st = [
    4, # tRCD
    4, # tRP
    8, # tRAS
    5, # tRRD
    5, # tRFC
    6, # tRTW
    8, # tWTR
    6, # tREFI
]
timing_preset_hb_mgch_c = [
    4, # tRCD
    4, # tRP
    4, # tRAS
    0, # tRRD
    1, # tRFC
    5, # tRTW
    4, # tWTR
    6, # tREFI
]
timing_preset_hb_mgch_st = [
    4, # tRCD
    5, # tRP
    9, # tRAS
    1, # tRRD
    2, # tRFC
    6, # tRTW
    4, # tWTR
    6, # tREFI
]

timing_preset_wtf_c = [
    4, # tRCD
    4, # tRP
    2, # tRAS
    5, # tRRD
    4, # tRFC
    6, # tRTW
    3, # tWTR
    6, # tREFI
]

timing_preset_wtf_st = [
    5, # tRCD
    5, # tRP
    4, # tRAS
    5, # tRRD
    5, # tRFC
    6, # tRTW
    5, # tWTR
    6, # tREFI
]

timing_preset_wte_c = [
    2, # tRCD
    2, # tRP
    2, # tRAS
    2, # tRRD
    4, # tRFC
    4, # tRTW
    4, # tWTR
    6, # tREFI
]

timing_preset_wte_st = [
    3, # tRCD
    5, # tRP
    3, # tRAS
    3, # tRRD
    5, # tRFC
    4, # tRTW
    5, # tWTR
    6, # tREFI
]

timing_preset_wtb_c = [
    4, # tRCD
    4, # tRP
    5, # tRAS
    5, # tRRD
    2, # tRFC
    6, # tRTW
    5, # tWTR
    6, # tREFI
]

timing_preset_wtb_st = [
    6, # tRCD
    6, # tRP
    7, # tRAS
    7, # tRRD
    2, # tRFC
    6, # tRTW
    5, # tWTR
    6, # tREFI
]

timing_preset_nee_c = [
    3, # tRCD
    3, # tRP
    2, # tRAS
    2, # tRRD
    5, # tRFC
    5, # tRTW
    4, # tWTR
    6, # tREFI
]

timing_preset_nee_st = [
    4, # tRCD
    4, # tRP
    4, # tRAS
    3, # tRRD
    7, # tRFC
    6, # tRTW
    5, # tWTR
    6, # tREFI
]

timing_preset_nme_c = [
    2, # tRCD
    2, # tRP
    1, # tRAS
    0, # tRRD
    1, # tRFC
    4, # tRTW
    3, # tWTR
    6, # tREFI
]

timing_preset_nme_st = [
    3, # tRCD
    3, # tRP
    4, # tRAS
    0, # tRRD
    1, # tRFC
    4, # tRTW
    4, # tWTR
    6, # tREFI
]


# INI

skinTarget = 54  # default value


PROFILES = {
    "V1_Erista": {
        "tskin_pcb_coefficients_console_on_fwdbg": 'str!"[6396, 119440]"',
        "tskin_pcb_coefficients_handheld_on_fwdbg": 'str!"[5817, 129580]"',
        "tskin_soc_coefficients_console_on_fwdbg": 'str!"[6182, 112480]"',
        "tskin_soc_coefficients_handheld_on_fwdbg": 'str!"[5464, 174190]"',
    },
    "V2_Mariko": {
        "tskin_pcb_coefficients_console_on_fwdbg": 'str!"[7338, 112161]"',
        "tskin_pcb_coefficients_handheld_on_fwdbg": 'str!"[6357, 168124]"',
        "tskin_soc_coefficients_console_on_fwdbg": 'str!"[6728, 129810]"',
        "tskin_soc_coefficients_handheld_on_fwdbg": 'str!"[5675, 203453]"',
    },
    "Lite_Mariko": {
        "tskin_pcb_coefficients_console_on_fwdbg": 'str!"[7338, 112161]"',
        "tskin_pcb_coefficients_handheld_on_fwdbg": 'str!"[5594, 209601]"',
        "tskin_soc_coefficients_console_on_fwdbg": 'str!"[6728, 129810]"',
        "tskin_soc_coefficients_handheld_on_fwdbg": 'str!"[5235, 199759]"',
    },
    "OLED_Mariko": {
        "tskin_pcb_coefficients_console_on_fwdbg": 'str!"[8051, -45213]"',
        "tskin_pcb_coefficients_handheld_on_fwdbg": 'str!"[7176, -33954]"',
        "tskin_soc_coefficients_console_on_fwdbg": 'str!"[7831, 57590]"',
        "tskin_soc_coefficients_handheld_on_fwdbg": 'str!"[9029, 4274]"',
    },
}

BATTERY_SAVE_OPTIONS = {
    "bgtc": {
        "enable_halfawake": "u32!0x0",
        "minimum_interval_normal": "u32!0x7FFFFFFF",
        "minimum_interval_save": "u32!0x7FFFFFFF",
        "battery_threshold_save": "u32!0x64",
        "battery_threshold_stop": "u32!0x64",
    },
    "npns": {
        "background_processing": "u8!0x0",
        "sleep_periodic_interval": "u32!0x7FFFFFFF",
        "sleep_processing_timeout": "u32!0x0",
        "sleep_max_try_count": "u32!0x0",
    },
    "ns.notification": {
        "enable_download_task_list": "u8!0x0",
        "enable_download_ticket": "u8!0x0",
        "enable_network_update": "u8!0x0",
        "enable_random_wait": "u8!0x0",
        "enable_request_on_cold_boot": "u8!0x0",
        "enable_send_rights_usage_status_request": "u8!0x0",
        "enable_sync_elicense_request": "u8!0x0",
        "enable_version_list": "u8!0x0",
        "retry_interval_min": "u32!0x7FFFFFFF",
        "retry_interval_max": "u32!0x7FFFFFFF",
        "version_list_waiting_limit_bias": "u32!0x7FFFFFFF",
        "version_list_waiting_limit_min": "u32!0x7FFFFFFF",
    },
    "account": {
        "na_required_for_network_service": "u8!0x0",
        "na_license_verification_enabled": "u8!0x0",
    },
    "account.daemon": {
        "background_awaking_periodicity": "u32!0x7FFFFFFF",
        "initial_schedule_delay": "u32!0x7FFFFFFF",
        "profile_sync_interval": "u32!0x7FFFFFFF",
        "na_info_refresh_interval": "u32!0x7FFFFFFF",
    },
    "capsrv": {
        "enable_album_screenshot_filedata_verification": "u8!0x0",
        "enable_album_movie_filehash_verification": "u8!0x0",
        "enable_album_movie_filesign_verification": "u8!0x0",
    },
    "friends": {
        "background_processing": "u8!0x0",
    },
    "notification.presenter": {
        "snooze_interval_in_seconds": "u32!0x7FFFFFFF",
        "connection_retry_count": "u32!0x0",
        "alarm_pattern_total_repeat_count": "u32!0x0",
        "alarm_pattern_with_vibration_repeat_count": "u32!0x0",
    },
    "prepo": {
        "transmission_interval_min": "u32!0x7FFFFFFF",
        "transmission_retry_interval_min": "u32!0x7FFFFFFF",
        "transmission_retry_interval_max": "u32!0x7FFFFFFF",
        "transmission_interval_in_sleep": "u32!0x7FFFFFFF",
        "statistics_save_interval_min": "u32!0x7FFFFFFF",
        "statistics_post_interval": "u32!0x7FFFFFFF",
        "save_system_report": "u8!0x0",
    },
    "olsc": {
        "default_auto_upload_global_setting": "u8!0x0",
        "default_auto_download_global_setting": "u8!0x0",
        "autonomy_registration_interval_seconds": "u32!0x7FFFFFFF",
        "network_service_license_info_cache_expiration_seconds": "u32!0x7FFFFFFF",
        "postponed_transfer_task_processing_interval_seconds": "u32!0x7FFFFFFF",
        "retry_offset_seconds": "u32!0x7FFFFFFF",
        "network_trouble_detection_span_seconds": "u32!0x7FFFFFFF",
        "network_connection_polling_interval_seconds": "u32!0x7FFFFFFF",
        "is_save_data_backup_policy_check_required": "u8!0x0",
        "is_global_transfer_task_autonomy_registration_enabled": "u8!0x0",
        "is_on_event_transfer_task_registration_enabled": "u8!0x0",
        "is_periodic_transfer_task_registration_enabled": "u8!0x0",
    },
    "ntc": {
        "is_autonomic_correction_enabled": "u8!0x0",
        "autonomic_correction_interval_seconds": "u32!0x7FFFFFFF",
        "autonomic_correction_failed_retry_interval_seconds": "u32!0x7FFFFFFF",
        "autonomic_correction_immediate_try_count_max": "u32!0x0",
        "autonomic_correction_immediate_try_interval_milliseconds": "u32!0x7FFFFFFF",
    },
    "systemupdate": {
        "bgnup_retry_seconds": "u32!0x7FFFFFFF",
    },
    "ns.rights": {
        "skip_account_validation_on_rights_check": "u8!0x1",
        "next_available_time_of_unexpected_error": "u32!0x7FFFFFFF",
    },
    "pctl": {
        "intermittent_task_interval_seconds": "u32!0x7FFFFFFF",
    },
    "sprofile": {
        "adjust_polling_interval_by_profile": "u8!0x0",
        "polling_interval_sec_max": "u32!0x7FFFFFFF",
        "polling_interval_sec_min": "u32!0x7FFFFFFF",
    },
}

PSM_OPTIONS = [
    {"name": "1024mA", "value": "u32!0x400"},
    {"name": "1280mA", "value": "u32!0x500"},
    {"name": "1536mA", "value": "u32!0x600"},
    {"name": "1660mA (Lite Default)", "value": "u32!0x67C"},
    {"name": "1792mA", "value": "u32!0x700"},
    {"name": "2048mA (Default)", "value": "u32!0x800"},
    {"name": "2304mA (UNSAFE)", "value": "u32!0x900"},
    {"name": "2560mA (UNSAFE)", "value": "u32!0xA00"},
    {"name": "2816mA (DANGEROUS)", "value": "u32!0xB00"},
    {"name": "3072mA (DANGEROUS)", "value": "u32!0xC00"},
]

skin_t_entries = {
    "use_configurations_on_fwdbg": "u8!0x1",
    "tskin_rate_table_console_on_fwdbg": f'str!"[[-1000000,40000,0,0],[36000,43000,51,51],[43000,49000,51,128],[49000,{skinTarget}000,128,255],[{skinTarget}000,1000000,255,255]]"',
    "tskin_rate_table_handheld_on_fwdbg": f'str!"[[-1000000,40000,0,0],[36000,43000,51,51],[43000,49000,51,128],[49000,{skinTarget}000,128,255],[{skinTarget}000,1000000,255,255]]"',
    "holdable_tskin": "u32!0xEA60",
    "touchable_tskin": "u32!0xEA60"
}
# Thresholds

mariko_meme_threshold = 1536000

mariko_dangerous_gpu_threshold = 1382400
mariko_unsafe_gpu_threshold = 1152000

erista_dangerous_gpu_threshold = 1151000
erista_unsafe_gpu_threshold = 922000

mariko_voltage_step = 5
erista_voltage_step = 5

mariko_gpu_offset_max = 50

mariko_gpu_min_volt = 480
erista_gpu_min_volt = 700


mariko_gpu_max_volt = 960
erista_gpu_max_volt = 1000

mariko_gpu_max_vmin = 700
erista_gpu_max_vmin = 850

mariko_cpu_min_vmin = 700
mariko_cpu_max_vmin = 750

# TODO: Make more stuff configurable


# COLORS
danger_color = (255, 0, 0, 255)
unsafe_color = (255, 165, 0, 255)
safe_color = (255, 255, 255, 255)