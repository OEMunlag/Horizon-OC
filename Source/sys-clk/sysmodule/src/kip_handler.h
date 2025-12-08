#ifndef KIP_H
#define KIP_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define KIP_MAGIC       "CUST"
#define KIP_MAGIC_LEN   4
#define KIP_MAX_STRUCT  4096

typedef struct {
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
    uint32_t marikoCpuHighVmin;
    uint32_t marikoCpuLowVmin;
    uint32_t eristaGpuVmin;
    uint32_t marikoGpuVmin;
    uint32_t marikoGpuVmax;

    uint32_t marikoGpuFullUnlock;

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
} kip_data_t;



static long kip_find_magic(FILE *f)
{
    unsigned char buf[4096 + KIP_MAGIC_LEN];
    size_t r;
    long offset = 0;

    memset(buf, 0, sizeof(buf));
    fseek(f, 0, SEEK_SET);

    while ((r = fread(buf + KIP_MAGIC_LEN, 1, 4096, f)) > 0)
    {
        for (size_t i = 0; i < r; i++)
        {
            if (memcmp(buf + i, KIP_MAGIC, KIP_MAGIC_LEN) == 0)
            {
                return offset + i;
            }
        }

        memcpy(buf, buf + 4096, KIP_MAGIC_LEN);
        offset += r;
    }

    return -1;
}



static int kip_read(const char *path, void *out_struct, size_t struct_size)
{
    if (struct_size > KIP_MAX_STRUCT)
        return -3;

    FILE *f = fopen(path, "rb");
    if (!f)
        return -1;

    long magic_pos = kip_find_magic(f);
    if (magic_pos < 0) {
        fclose(f);
        return -2; /* magic not found */
    }

    long data_pos = magic_pos + KIP_MAGIC_LEN;
    fseek(f, data_pos, SEEK_SET);

    size_t r = fread(out_struct, 1, struct_size, f);
    fclose(f);

    return (r == struct_size) ? 0 : -4;
}



static int kip_write(const char *path, const void *in_struct, size_t struct_size)
{
    if (struct_size > KIP_MAX_STRUCT)
        return -3;

    FILE *f = fopen(path, "r+b");
    if (!f)
        return -1;

    long magic_pos = kip_find_magic(f);
    if (magic_pos < 0) {
        fclose(f);
        return -2;
    }

    long data_pos = magic_pos + KIP_MAGIC_LEN;
    fseek(f, data_pos, SEEK_SET);

    size_t w = fwrite(in_struct, 1, struct_size, f);
    fclose(f);

    return (w == struct_size) ? 0 : -4;
}

#endif /* KIP_H */
