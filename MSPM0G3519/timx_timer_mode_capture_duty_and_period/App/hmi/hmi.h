/**
 * @file    hmi.h
 * @brief   陶晶驰串口屏 HMI 协议
 *
 *   帧格式: 控件名="值"\xff\xff\xff
 */

#ifndef __HMI_H
#define __HMI_H

#include "App/app_types.h"
#include <stdint.h>

typedef struct {
    uint8_t port;
} HMI_Comm;

#define HMI_TAIL  "\xff\xff\xff"

void HMI_Init(HMI_Comm *self, uint8_t port);
void HMI_SendStr(HMI_Comm *self, const char *ctl, const char *text);
void HMI_SendInt(HMI_Comm *self, const char *ctl, int num);
void HMI_SendFloat(HMI_Comm *self, const char *ctl, float num, int decimals);
void HMI_WaveAdd(HMI_Comm *self, const char *ctl, int ch, int val);
void HMI_WaveClear(HMI_Comm *self, const char *ctl, int ch);
void HMI_SendInitScreen(HMI_Comm *self);
void HMI_ReportWave(HMI_Comm *self, Wave_Struct *wave);

#endif /* __HMI_H */
