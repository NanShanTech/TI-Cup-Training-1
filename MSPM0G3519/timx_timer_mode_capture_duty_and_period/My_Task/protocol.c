/**
 * @file    protocol.c
 * @brief   串口协�??解析 �? 双字节命�? + Serial_Printf 上报
 *          夏子�? 2026.7.5
 */

#include "protocol.h"
#include "BSP/bsp.h"
#include "tasks.h"
#include "ti_msp_dl_config.h"

/* ---- 全局状�? ---- */
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
        case MODE_NO_DISTORTION: return "��ʧ��";
        case MODE_BOTTOM_CLIP:   return "�ײ�ʧ��";
        case MODE_BOTH_CLIP:     return "˫��ʧ��";
        case MODE_TOP_CLIP:      return "����ʧ��";
        case MODE_CROSSOVER:     return "��Խʧ��";
        default:
            if (s_last_dist != MODE_NONE) {
                return Protocol_DistName(s_last_dist);
            }
            return "��ʧ��";
    }
}

/* ---- 解析一�?字节 ---- */
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
                        Relay_Update(g_mode);
                        break;

                    case MODE_SPECTRUM:
                        g_mode = MODE_SPECTRUM;
                        /* 频谱图模式不更新继电�?，保持失真模式电平状�? */
                        break;

                    case MODE_REPEAT_LAST:
                        if (s_last_dist != MODE_NONE) {
                            g_mode = s_last_dist;
                            Relay_Update(g_mode);
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

/* ---- 继电器控�? ---- */
void Relay_Update(MeasureMode_t mode)
{
    /* 全部拉低 */
    DL_GPIO_clearPins(GPIOC, PIN1_PIN_1_PIN | PIN2_PIN_2_PIN |
                             PIN3_PIN_3_PIN | PIN4_PIN_4_PIN |
                             PIN5_PIN_5_PIN);

    /* 对应模式引脚输出高电�? */
    switch (mode) {
        case MODE_NO_DISTORTION:
            DL_GPIO_setPins(PIN1_PORT, PIN1_PIN_1_PIN);
            break;
        case MODE_BOTTOM_CLIP:
            DL_GPIO_setPins(PIN2_PORT, PIN2_PIN_2_PIN);
            break;
        case MODE_BOTH_CLIP:
            DL_GPIO_setPins(PIN3_PORT, PIN3_PIN_3_PIN);
            break;
        case MODE_TOP_CLIP:
            DL_GPIO_setPins(PIN4_PORT, PIN4_PIN_4_PIN);
            break;
        case MODE_CROSSOVER:
            DL_GPIO_setPins(PIN5_PORT, PIN5_PIN_5_PIN);
            break;
        default:
            /* MODE_NONE / MODE_SPECTRUM: 全部低电�? */
            break;
    }
}

/* ---- 执�?�当前模�? ---- */
void Protocol_Execute(void)
{
    extern Wave_Struct g_wave_info;

    if (g_mode == MODE_NONE || g_mode == MODE_SPECTRUM) {
        return;
    }

    Serial_Printf("t1.txt=\"%.2f%%\"\xff\xff\xff", (double)g_wave_info.THD);
    Serial_Printf("t3.txt=\"%s\"\xff\xff\xff", Protocol_DistName(g_mode));
}
