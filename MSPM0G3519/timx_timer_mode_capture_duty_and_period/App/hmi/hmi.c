/**
 * @file    hmi.c
 * @brief   陶晶驰串口屏 HMI 协议实现
 */

#include "BSP/bsp.h"
#include "App/app.h"
#include "hmi.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void HMI_Init(HMI_Comm *self, uint8_t port)
{
    self->port = port;
    HMI_Serial_Init();
}

static void HMI_Printf(HMI_Comm *self, const char *fmt, ...)
{
    char buf[128];
    va_list ap;
    int len;

    va_start(ap, fmt);
    len = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    if (len <= 0) return;
    if (len >= (int)sizeof(buf)) len = (int)sizeof(buf) - 1;

    /* 追加协议尾 \xff\xff\xff */
    if (len + 3 <= (int)sizeof(buf)) {
        buf[len + 0] = '\xff';
        buf[len + 1] = '\xff';
        buf[len + 2] = '\xff';
        len += 3;
    }

    HMI_Serial_Send((const uint8_t *)buf, (uint16_t)len);
}

void HMI_SendStr(HMI_Comm *self, const char *ctl, const char *text)
{
    HMI_Printf(self, "%s=\"%s\"", ctl, text);
}

void HMI_SendInt(HMI_Comm *self, const char *ctl, int num)
{
    HMI_Printf(self, "%s=%d", ctl, num);
}

void HMI_SendFloat(HMI_Comm *self, const char *ctl, float num, int decimals)
{
    long scaled = (long)(num * (decimals == 2 ? 100.0f : 1000.0f) + 0.5f);
    HMI_Printf(self, "%s=%ld", ctl, scaled);
}

void HMI_WaveAdd(HMI_Comm *self, const char *ctl, int ch, int val)
{
    HMI_Printf(self, "add %s,%d,%d", ctl, ch, val);
}

void HMI_WaveClear(HMI_Comm *self, const char *ctl, int ch)
{
    HMI_Printf(self, "cle %s,%d", ch);
}

void HMI_SendInitScreen(HMI_Comm *self)
{
    HMI_SendStr(self, "t1.txt", "??");
    HMI_SendStr(self, "t3.txt", "??");
    HMI_SendStr(self, "t5.txt", "??");
    HMI_SendStr(self, "t7.txt", "??");
}

void HMI_ReportWave(HMI_Comm *self, Wave_Struct *wave)
{
    char buf[64];

    if (wave->Freq < 1000.0f) {
        snprintf(buf, sizeof(buf), "%.4f Hz", (double)wave->Freq);
    } else if (wave->Freq < 1000000.0f) {
        snprintf(buf, sizeof(buf), "%.4f KHz", (double)wave->Freq / 1000.0);
    } else {
        snprintf(buf, sizeof(buf), "%.5f MHz", (double)wave->Freq / 1000000.0);
    }
    HMI_SendStr(self, "t1.txt", buf);

    snprintf(buf, sizeof(buf), "%.2f mv", wave->Vpp);
    HMI_SendStr(self, "t3.txt", buf);
}
