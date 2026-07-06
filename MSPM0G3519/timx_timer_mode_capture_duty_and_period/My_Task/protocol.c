/**
 * @file    protocol.c
 * @brief   涓插彛鍗忥拷??瑙ｆ瀽 锟?? 鍙屽瓧鑺傚懡锟?? + Serial_Printf 涓婃姤
 *         澶忓瓙锟?? 2026.7.5
 */

#include "protocol.h"
#include "BSP/bsp.h"
#include "tasks.h"

/* ---- 鍏ㄥ眬鐘讹拷? ---- */
volatile MeasureMode_t g_mode      = MODE_NONE;
static  MeasureMode_t  s_last_dist = MODE_NONE;

/* ---- 鎺ユ敹鐘舵�佹満 ---- */
typedef enum {
    STATE_IDLE,
    STATE_WAIT_SECOND,
} ParserState_t;

static ParserState_t s_state      = STATE_IDLE;
static uint8_t       s_first_byte = 0;

/* ---- 澶辩湡绫诲瀷鍚嶇О ---- */
const char *Protocol_DistName(MeasureMode_t mode)
{
    switch (mode) {
        case MODE_NO_DISTORTION: return "无失真";
        case MODE_BOTTOM_CLIP:   return "底部失真";
        case MODE_BOTH_CLIP:     return "双向失真";
        case MODE_TOP_CLIP:      return "顶部失真";
        case MODE_CROSSOVER:     return "交越失真";
        default:
            if (s_last_dist != MODE_NONE) {
                return Protocol_DistName(s_last_dist);
            }
            return "无失真";
    }
}

/* ---- 瑙ｆ瀽涓�锟??瀛楄妭 ---- */
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

/* ---- 鎵э拷?锟藉綋鍓嶆ā锟?? ---- */
void Protocol_Execute(void)
{
    extern Wave_Struct g_wave_info;

    if (g_mode == MODE_NONE || g_mode == MODE_SPECTRUM) {
        return;
    }

    Serial_Printf("t1.txt=\"%.2f%%\"\xff\xff\xff", (double)g_wave_info.THD);
    Serial_Printf("t3.txt=\"%s\"\xff\xff\xff", Protocol_DistName(g_mode));

    /* 寮曡剼鎺у埗 锟?? 鍚庣画瀹炵幇 */
    switch (g_mode) {
        case MODE_NO_DISTORTION: /* TODO */ break;
        case MODE_BOTTOM_CLIP:   /* TODO */ break;
        case MODE_BOTH_CLIP:     /* TODO */ break;
        case MODE_TOP_CLIP:      /* TODO */ break;
        case MODE_CROSSOVER:     /* TODO */ break;
        default: break;
    }
}
