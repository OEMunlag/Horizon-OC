/*
 * HOC Configurator - KIP Handler
 * Copyright (C) Dominatorul, Souldbminer
 */

#pragma once
#include <string>
#include <cstdint>
#include <vector>

class KipHandler {
private:
    std::string kipPath = "sdmc:/atmosphere/kips/hoc.kip";
    const uint8_t MAGIC[4] = {'C', 'U', 'S', 'T'};
    
    struct KipData {
        uint32_t custRev;
        uint32_t mtcConf;
        uint32_t hpMode;
        uint32_t commonCpuBoostClock;
        uint32_t commonEmcMemVolt;
        uint32_t eristaCpuMaxVolt;
        uint32_t eristaEmcMaxClock;
        uint32_t marikoCpuMaxVolt;
        uint32_t marikoEmcMaxClock;
        uint32_t marikoEmcVddqVolt;
        uint32_t marikoCpuUV;
        uint32_t marikoGpuUV;
        uint32_t eristaCpuUV;
        uint32_t eristaGpuUV;
        uint32_t commonGpuVoltOffset;
        uint32_t marikoEmcDvbShift;
        
        // Memory timings
        uint32_t t1_tRCD;
        uint32_t t2_tRP;
        uint32_t t3_tRAS;
        uint32_t t4_tRRD;
        uint32_t t5_tRFC;
        uint32_t t6_tRTW;
        uint32_t t7_tWTR;
        uint32_t t8_tREFI;
        uint32_t mem_burst_read_latency;
        uint32_t mem_burst_write_latency;
        
        // Additional voltages
        uint32_t marikoCpuHighVmin;
        uint32_t marikoCpuLowVmin;

        uint32_t eristaGpuVmin;
        uint32_t marikoGpuVmin;
        uint32_t marikoGpuVmax;
        
        uint32_t marikoGpuFullUnlock;

        // GPU voltages for each frequency (Mariko)
        uint32_t g_volt_76800;
        uint32_t g_volt_153600;
        uint32_t g_volt_230400;
        uint32_t g_volt_307200;
        uint32_t g_volt_384000;
        uint32_t g_volt_460800;
        uint32_t g_volt_537600;
        uint32_t g_volt_614400;
        uint32_t g_volt_691200;
        uint32_t g_volt_768000;
        uint32_t g_volt_844800;
        uint32_t g_volt_921600;
        uint32_t g_volt_998400;
        uint32_t g_volt_1075200;
        uint32_t g_volt_1152000;
        uint32_t g_volt_1228800;
        uint32_t g_volt_1267200;
        uint32_t g_volt_1305600;
        uint32_t g_volt_1344000;
        uint32_t g_volt_1382400;
        uint32_t g_volt_1420800;
        uint32_t g_volt_1459200;
        uint32_t g_volt_1497600;
        uint32_t g_volt_1536000;
        
        // GPU voltages for each frequency (Erista)
        uint32_t g_volt_e_76800;
        uint32_t g_volt_e_115200;
        uint32_t g_volt_e_153600;
        uint32_t g_volt_e_192000;

        uint32_t g_volt_e_230400;
        uint32_t g_volt_e_268800;

        uint32_t g_volt_e_307200;
        uint32_t g_volt_e_345600;

        uint32_t g_volt_e_384000;
        uint32_t g_volt_e_422400;

        uint32_t g_volt_e_460800;
        uint32_t g_volt_e_499200;

        uint32_t g_volt_e_537600;
        uint32_t g_volt_e_576000;

        uint32_t g_volt_e_614400;
        uint32_t g_volt_e_652800;

        uint32_t g_volt_e_691200;
        uint32_t g_volt_e_729600;

        uint32_t g_volt_e_768000;
        uint32_t g_volt_e_806400;

        uint32_t g_volt_e_844800;
        uint32_t g_volt_e_883200;

        uint32_t g_volt_e_921600;
        uint32_t g_volt_e_960000;
        uint32_t g_volt_e_998400;
        uint32_t g_volt_e_1036800;
        uint32_t g_volt_e_1075200;
    };
    
    KipData data;

public:
    KipHandler(const std::string& path) : kipPath(path) {
        // Initialize with defaults
    }
    
    bool readKip();
    bool writeKip();
    
    // Getters
    KipData& getData() { return data; }
    const KipData& getData() const { return data; }
    
    // Setters for all KipData fields
//     void setCustRev(uint32_t val) { data.custRev = val; }
    void setMtcConf(uint32_t val) { data.mtcConf = val; }
    void setHpMode(uint32_t val) { data.hpMode = val; }
    void setCommonCpuBoostClock(uint32_t val) { data.commonCpuBoostClock = val; }
    void setCommonEmcMemVolt(uint32_t val) { data.commonEmcMemVolt = val; }
    void setEristaCpuMaxVolt(uint32_t val) { data.eristaCpuMaxVolt = val; }
    void setEristaEmcMaxClock(uint32_t val) { data.eristaEmcMaxClock = val; }
    void setMarikoCpuMaxVolt(uint32_t val) { data.marikoCpuMaxVolt = val; }
    void setMarikoEmcMaxClock(uint32_t val) { data.marikoEmcMaxClock = val; }
    void setMarikoEmcVddqVolt(uint32_t val) { data.marikoEmcVddqVolt = val; }
    void setMarikoCpuUV(uint32_t val) { data.marikoCpuUV = val; }
    void setMarikoGpuUV(uint32_t val) { data.marikoGpuUV = val; }
    void setEristaCpuUV(uint32_t val) { data.eristaCpuUV = val; }
    void setEristaGpuUV(uint32_t val) { data.eristaGpuUV = val; }
    void setCommonGpuVoltOffset(uint32_t val) { data.commonGpuVoltOffset = val; }
    void setMarikoEmcDvbShift(uint32_t val) { data.marikoEmcDvbShift = val; }
    
    // Memory timing setters
    void setT1_tRCD(uint32_t val) { data.t1_tRCD = val; }
    void setT2_tRP(uint32_t val) { data.t2_tRP = val; }
    void setT3_tRAS(uint32_t val) { data.t3_tRAS = val; }
    void setT4_tRRD(uint32_t val) { data.t4_tRRD = val; }
    void setT5_tRFC(uint32_t val) { data.t5_tRFC = val; }
    void setT6_tRTW(uint32_t val) { data.t6_tRTW = val; }
    void setT7_tWTR(uint32_t val) { data.t7_tWTR = val; }
    void setT8_tREFI(uint32_t val) { data.t8_tREFI = val; }
    void setMemBurstReadLatency(uint32_t val) { data.mem_burst_read_latency = val; }
    void setMemBurstWriteLatency(uint32_t val) { data.mem_burst_write_latency = val; }
    
    // Additional voltage setters
    void setMarikoCpuHighVmin(uint32_t val) { data.marikoCpuHighVmin = val; }
    void setMarikoCpuLowVmin(uint32_t val) { data.marikoCpuLowVmin = val; }
    void setEristaGpuVmin(uint32_t val) { data.eristaGpuVmin = val; }
    void setMarikoGpuVmin(uint32_t val) { data.marikoGpuVmin = val; }
    void setMarikoGpuVmax(uint32_t val) { data.marikoGpuVmax = val; }
    void setMarikoGpuFullUnlock(uint32_t val) { data.marikoGpuFullUnlock = val; }
    
    // GPU voltage setters for Mariko
    void setGVolt76800(uint32_t val) { data.g_volt_76800 = val; }
    void setGVolt153600(uint32_t val) { data.g_volt_153600 = val; }
    void setGVolt230400(uint32_t val) { data.g_volt_230400 = val; }
    void setGVolt307200(uint32_t val) { data.g_volt_307200 = val; }
    void setGVolt384000(uint32_t val) { data.g_volt_384000 = val; }
    void setGVolt460800(uint32_t val) { data.g_volt_460800 = val; }
    void setGVolt537600(uint32_t val) { data.g_volt_537600 = val; }
    void setGVolt614400(uint32_t val) { data.g_volt_614400 = val; }
    void setGVolt691200(uint32_t val) { data.g_volt_691200 = val; }
    void setGVolt768000(uint32_t val) { data.g_volt_768000 = val; }
    void setGVolt844800(uint32_t val) { data.g_volt_844800 = val; }
    void setGVolt921600(uint32_t val) { data.g_volt_921600 = val; }
    void setGVolt998400(uint32_t val) { data.g_volt_998400 = val; }
    void setGVolt1075200(uint32_t val) { data.g_volt_1075200 = val; }
    void setGVolt1152000(uint32_t val) { data.g_volt_1152000 = val; }
    void setGVolt1228800(uint32_t val) { data.g_volt_1228800 = val; }
    void setGVolt1267200(uint32_t val) { data.g_volt_1267200 = val; }
    void setGVolt1305600(uint32_t val) { data.g_volt_1305600 = val; }
    void setGVolt1344000(uint32_t val) { data.g_volt_1344000 = val; }
    void setGVolt1382400(uint32_t val) { data.g_volt_1382400 = val; }
    void setGVolt1420800(uint32_t val) { data.g_volt_1420800 = val; }
    void setGVolt1459200(uint32_t val) { data.g_volt_1459200 = val; }
    void setGVolt1497600(uint32_t val) { data.g_volt_1497600 = val; }
    void setGVolt1536000(uint32_t val) { data.g_volt_1536000 = val; }
    
    // GPU voltage setters for Erista
    void setGVoltE76800(uint32_t val) { data.g_volt_e_76800 = val; }
    void setGVoltE115200(uint32_t val) { data.g_volt_e_115200 = val; }
    void setGVoltE153600(uint32_t val) { data.g_volt_e_153600 = val; }
    void setGVoltE192000(uint32_t val) { data.g_volt_e_192000 = val; }
    void setGVoltE230400(uint32_t val) { data.g_volt_e_230400 = val; }
    void setGVoltE268800(uint32_t val) { data.g_volt_e_268800 = val; }
    void setGVoltE307200(uint32_t val) { data.g_volt_e_307200 = val; }
    void setGVoltE345600(uint32_t val) { data.g_volt_e_345600 = val; }
    void setGVoltE384000(uint32_t val) { data.g_volt_e_384000 = val; }
    void setGVoltE422400(uint32_t val) { data.g_volt_e_422400 = val; }
    void setGVoltE460800(uint32_t val) { data.g_volt_e_460800 = val; }
    void setGVoltE499200(uint32_t val) { data.g_volt_e_499200 = val; }
    void setGVoltE537600(uint32_t val) { data.g_volt_e_537600 = val; }
    void setGVoltE576000(uint32_t val) { data.g_volt_e_576000 = val; }
    void setGVoltE614400(uint32_t val) { data.g_volt_e_614400 = val; }
    void setGVoltE652800(uint32_t val) { data.g_volt_e_652800 = val; }
    void setGVoltE691200(uint32_t val) { data.g_volt_e_691200 = val; }
    void setGVoltE729600(uint32_t val) { data.g_volt_e_729600 = val; }
    void setGVoltE768000(uint32_t val) { data.g_volt_e_768000 = val; }
    void setGVoltE806400(uint32_t val) { data.g_volt_e_806400 = val; }
    void setGVoltE844800(uint32_t val) { data.g_volt_e_844800 = val; }
    void setGVoltE883200(uint32_t val) { data.g_volt_e_883200 = val; }
    void setGVoltE921600(uint32_t val) { data.g_volt_e_921600 = val; }
    void setGVoltE960000(uint32_t val) { data.g_volt_e_960000 = val; }
    void setGVoltE998400(uint32_t val) { data.g_volt_e_998400 = val; }
    void setGVoltE1036800(uint32_t val) { data.g_volt_e_1036800 = val; }
    void setGVoltE1075200(uint32_t val) { data.g_volt_e_1075200 = val; }
    
    // Getters for all KipData fields
    uint32_t getMtcConf() const { return data.mtcConf; }
    uint32_t getHpMode() const { return data.hpMode; }
    uint32_t getCommonCpuBoostClock() const { return data.commonCpuBoostClock; }
    uint32_t getCommonEmcMemVolt() const { return data.commonEmcMemVolt; }
    uint32_t getEristaCpuMaxVolt() const { return data.eristaCpuMaxVolt; }
    uint32_t getEristaEmcMaxClock() const { return data.eristaEmcMaxClock; }
    uint32_t getMarikoCpuMaxVolt() const { return data.marikoCpuMaxVolt; }
    uint32_t getMarikoEmcMaxClock() const { return data.marikoEmcMaxClock; }
    uint32_t getMarikoEmcVddqVolt() const { return data.marikoEmcVddqVolt; }
    uint32_t getMarikoCpuUV() const { return data.marikoCpuUV; }
    uint32_t getMarikoGpuUV() const { return data.marikoGpuUV; }
    uint32_t getEristaCpuUV() const { return data.eristaCpuUV; }
    uint32_t getEristaGpuUV() const { return data.eristaGpuUV; }
    uint32_t getCommonGpuVoltOffset() const { return data.commonGpuVoltOffset; }
    uint32_t getMarikoEmcDvbShift() const { return data.marikoEmcDvbShift; }
    
    // Memory timing getters
    uint32_t getT1_tRCD() const { return data.t1_tRCD; }
    uint32_t getT2_tRP() const { return data.t2_tRP; }
    uint32_t getT3_tRAS() const { return data.t3_tRAS; }
    uint32_t getT4_tRRD() const { return data.t4_tRRD; }
    uint32_t getT5_tRFC() const { return data.t5_tRFC; }
    uint32_t getT6_tRTW() const { return data.t6_tRTW; }
    uint32_t getT7_tWTR() const { return data.t7_tWTR; }
    uint32_t getT8_tREFI() const { return data.t8_tREFI; }
    uint32_t getMemBurstReadLatency() const { return data.mem_burst_read_latency; }
    uint32_t getMemBurstWriteLatency() const { return data.mem_burst_write_latency; }
    
    // Additional voltage getters
    uint32_t getMarikoCpuHighVmin() const { return data.marikoCpuHighVmin; }
    uint32_t getMarikoCpuLowVmin() const { return data.marikoCpuLowVmin; }
    uint32_t getEristaGpuVmin() const { return data.eristaGpuVmin; }
    uint32_t getMarikoGpuVmin() const { return data.marikoGpuVmin; }
    uint32_t getMarikoGpuVmax() const { return data.marikoGpuVmax; }
    uint32_t getMarikoGpuFullUnlock() const { return data.marikoGpuFullUnlock; }
    
    // GPU voltage getters for Mariko
    uint32_t getGVolt76800() const { return data.g_volt_76800; }
    uint32_t getGVolt153600() const { return data.g_volt_153600; }
    uint32_t getGVolt230400() const { return data.g_volt_230400; }
    uint32_t getGVolt307200() const { return data.g_volt_307200; }
    uint32_t getGVolt384000() const { return data.g_volt_384000; }
    uint32_t getGVolt460800() const { return data.g_volt_460800; }
    uint32_t getGVolt537600() const { return data.g_volt_537600; }
    uint32_t getGVolt614400() const { return data.g_volt_614400; }
    uint32_t getGVolt691200() const { return data.g_volt_691200; }
    uint32_t getGVolt768000() const { return data.g_volt_768000; }
    uint32_t getGVolt844800() const { return data.g_volt_844800; }
    uint32_t getGVolt921600() const { return data.g_volt_921600; }
    uint32_t getGVolt998400() const { return data.g_volt_998400; }
    uint32_t getGVolt1075200() const { return data.g_volt_1075200; }
    uint32_t getGVolt1152000() const { return data.g_volt_1152000; }
    uint32_t getGVolt1228800() const { return data.g_volt_1228800; }
    uint32_t getGVolt1267200() const { return data.g_volt_1267200; }
    uint32_t getGVolt1305600() const { return data.g_volt_1305600; }
    uint32_t getGVolt1344000() const { return data.g_volt_1344000; }
    uint32_t getGVolt1382400() const { return data.g_volt_1382400; }
    uint32_t getGVolt1420800() const { return data.g_volt_1420800; }
    uint32_t getGVolt1459200() const { return data.g_volt_1459200; }
    uint32_t getGVolt1497600() const { return data.g_volt_1497600; }
    uint32_t getGVolt1536000() const { return data.g_volt_1536000; }
    
    // GPU voltage getters for Erista
    uint32_t getGVoltE76800() const { return data.g_volt_e_76800; }
    uint32_t getGVoltE115200() const { return data.g_volt_e_115200; }
    uint32_t getGVoltE153600() const { return data.g_volt_e_153600; }
    uint32_t getGVoltE192000() const { return data.g_volt_e_192000; }
    uint32_t getGVoltE230400() const { return data.g_volt_e_230400; }
    uint32_t getGVoltE268800() const { return data.g_volt_e_268800; }
    uint32_t getGVoltE307200() const { return data.g_volt_e_307200; }
    uint32_t getGVoltE345600() const { return data.g_volt_e_345600; }
    uint32_t getGVoltE384000() const { return data.g_volt_e_384000; }
    uint32_t getGVoltE422400() const { return data.g_volt_e_422400; }
    uint32_t getGVoltE460800() const { return data.g_volt_e_460800; }
    uint32_t getGVoltE499200() const { return data.g_volt_e_499200; }
    uint32_t getGVoltE537600() const { return data.g_volt_e_537600; }
    uint32_t getGVoltE576000() const { return data.g_volt_e_576000; }
    uint32_t getGVoltE614400() const { return data.g_volt_e_614400; }
    uint32_t getGVoltE652800() const { return data.g_volt_e_652800; }
    uint32_t getGVoltE691200() const { return data.g_volt_e_691200; }
    uint32_t getGVoltE729600() const { return data.g_volt_e_729600; }
    uint32_t getGVoltE768000() const { return data.g_volt_e_768000; }
    uint32_t getGVoltE806400() const { return data.g_volt_e_806400; }
    uint32_t getGVoltE844800() const { return data.g_volt_e_844800; }
    uint32_t getGVoltE883200() const { return data.g_volt_e_883200; }
    uint32_t getGVoltE921600() const { return data.g_volt_e_921600; }
    uint32_t getGVoltE960000() const { return data.g_volt_e_960000; }
    uint32_t getGVoltE998400() const { return data.g_volt_e_998400; }
    uint32_t getGVoltE1036800() const { return data.g_volt_e_1036800; }
    uint32_t getGVoltE1075200() const { return data.g_volt_e_1075200; }
    
    // Utility
    std::string getKipPath() const { return kipPath; }
    void setKipPath(const std::string& path) { kipPath = path; }
};

static inline void storeKipValues() {

}