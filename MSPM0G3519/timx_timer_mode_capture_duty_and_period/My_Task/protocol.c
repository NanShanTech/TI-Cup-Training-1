/**
 * @file    protocol.c
 * @brief   串口协议解析 — 双字节命令 + Serial_Printf 上报
 *         夏子杰 2026.7.5
 */

#include "protocol.h"
#include "BSP/bsp.h"
#include "tasks.h"

/* ---- 全局状态 ---- */
volatile MeasureMode_t g_mode      = MODE_NONE;
static  MeasureMode_t  s_last_dist = MODE_NONE;

/* ---- 接收状态机 ---- */
typedef enum {
    STATE_IDLE,
    STATE_WAIT_SECOND,
} ParserState_t;

static ParserState_t s_state      = STATE_IDLE;
static uint8_t       s_first_byte = 0;

/* ---- 失真类型名称 ---- */
const char *Protocol_DistName(MeasureMode_t mode)
{
    switch (mode) {
        case MODE_NO_DISTORTION: return "11";
        case MODE_BOTTOM_CLIP:   return "22";
        case MODE_BOTH_CLIP:     return "33";
        case MODE_TOP_CLIP:      return "44";
        case MODE_CROSSOVER:     return "55";
        default:                 return "66";
    }
}

/* ---- 解析一个字节 ---- */
void Protocol_ParseByte(uint8_t byte)
{
    if (byte < 0x11 || byte > 0x77) {
        s_state = STATE_IDLE;
        return;
    }

    switch (s_state) {
        case STATE_IDLE:
            s_first_byte = byte;
            s_state      = STATE_WAIT_SECOND;
            break;

        case STATE_WAIT_SECOND:
            if (byte == s_first_byte) {
                switch (byte) {
                    case MODE_NO_DISTORTION:
                    case MODE_BOTTOM_CLIP:
                    case MODE_BOTH_CLIP:
                    case MODE_TOP_CLIP:
                    case MODE_CROSSOVER:
                        s_last_dist = (MeasureMode_t)byte;
                        g_mode      = (MeasureMode_t)byte;
                        break;

                    case MODE_SPECTRUM:
                        g_mode = MODE_SPECTRUM;
                        break;

                    case MODE_REPEAT_LAST:
                        if (s_last_dist != MODE_NONE) {
                            g_mode = s_last_dist;
                        }
                        break;

                    default:
                        break;
                }
            }
            s_state = STATE_IDLE;
            break;
    }
}

/* ---- 执行当前模式 ---- */
void Protocol_Execute(void)
{
    extern Wave_Struct g_wave_info;

    if (g_mode == MODE_NONE || g_mode == MODE_SPECTRUM) {
        return;
    }

    Serial_Printf("t1.txt=\"%.2f%%\"\xff\xff\xff", (double)g_wave_info.THD);
    Serial_Printf("t3.txt=\"%s\"\xff\xff\xff", dist_name(g_mode));

    /* 引脚控制 — 后续实现 */
    switch (g_mode) {
        case MODE_NO_DISTORTION: /* TODO */ break;
        case MODE_BOTTOM_CLIP:   /* TODO */ break;
        case MODE_BOTH_CLIP:     /* TODO */ break;
        case MODE_TOP_CLIP:      /* TODO */ break;
        case MODE_CROSSOVER:     /* TODO */ break;
        default: break;
    }
}
