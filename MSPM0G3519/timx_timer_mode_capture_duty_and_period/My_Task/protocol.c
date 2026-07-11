/**
 * @file    protocol.c
 * @brief   串口协�??解析 �????? 双字节命�????? + Serial_Printf 上报
 *          夏子�????? 2026.7.5
 */

#include "protocol.h"
#include "BSP/bsp.h"
#include "tasks.h"
#include "ti_msp_dl_config.h"

/* ---- k1/k2 引脚定义（PB6=IOMUX_PINCM23, PB8=IOMUX_PINCM25�????---- */
#define K1_IOMUX    (IOMUX_PINCM23)
#define K2_IOMUX    (IOMUX_PINCM25)
#define KEY_PORT    (GPIOB)
#define K1_PIN      (DL_GPIO_PIN_6)
#define K2_PIN      (DL_GPIO_PIN_8)

/* ---- 全局状�? ---- */
volatile MeasureMode_t g_mode      = MODE_NONE;
static  MeasureMode_t  s_last_dist = MODE_NONE;
static  bool           s_k1_high         = false;
static  bool           s_k2_high         = false;
static  uint8_t        s_relay_response  = 0;  /* 0=�??, 1=k1�??, 2=k1�??, 3=k2�??, 4=k2�?? */

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

/* ---- 解析一�?????字节 ---- */
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
                        /* 频谱图模式不更新继电�?????，保持失真模式电平状�????? */
                        break;

                    case MODE_REPEAT_LAST:
                        if (s_last_dist != MODE_NONE) {
                            g_mode = s_last_dist;
                            Relay_Update(g_mode);
                        }
                        break;

                    case 0x13:  /* k1 翻转 */
                        s_k1_high = !s_k1_high;
                        if (s_k1_high) {
                            DL_GPIO_setPins(KEY_PORT, K1_PIN);
                            s_relay_response = 1;
                        } else {
                            DL_GPIO_clearPins(KEY_PORT, K1_PIN);
                            s_relay_response = 2;
                        }
                        break;

                    case 0x14:  /* k2 翻转 */
                        s_k2_high = !s_k2_high;
                        if (s_k2_high) {
                            DL_GPIO_setPins(KEY_PORT, K2_PIN);
                            s_relay_response = 3;
                        } else {
                            DL_GPIO_clearPins(KEY_PORT, K2_PIN);
                            s_relay_response = 4;
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

/* ---- 继电器初始化 ---- */
void Relay_Init(void)
{
    DL_GPIO_initDigitalOutput(K1_IOMUX);
    DL_GPIO_initDigitalOutput(K2_IOMUX);
    DL_GPIO_clearPins(KEY_PORT, K1_PIN | K2_PIN);
    DL_GPIO_enableOutput(KEY_PORT, K1_PIN | K2_PIN);
}

/* ---- 继电器控�????? ---- */
void Relay_Update(MeasureMode_t mode)
{
    /* 全部拉低 */
    DL_GPIO_clearPins(GPIOC, PIN1_PIN_1_PIN | PIN2_PIN_2_PIN |
                             PIN3_PIN_3_PIN | PIN4_PIN_4_PIN |
                             PIN5_PIN_5_PIN);

    /* 对应模式引脚输出高电�????? */
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
            /* MODE_NONE / MODE_SPECTRUM: 全部低电�????? */
            break;
    }
}

/* ---- 待发送的继电器响�?? ---- */
void Protocol_FlushRelayResponse(void)
{
    switch (s_relay_response) {
        case 1: Serial_Printf("b6.txt=\"k1:2\"\xff\xff\xff"); break;
        case 2: Serial_Printf("b6.txt=\"k1:1\"\xff\xff\xff"); break;
        case 3: Serial_Printf("b7.txt=\"k2:2\"\xff\xff\xff"); break;
        case 4: Serial_Printf("b7.txt=\"k2:1\"\xff\xff\xff"); break;
        default: break;
    }
    s_relay_response = 0;
}

void Protocol_Execute(void)
{
    extern Wave_Struct g_wave_info;

    if (g_mode == MODE_NONE || g_mode == MODE_SPECTRUM) {
        return;
    }

    Serial_Printf("t1.txt=\"%.2f%%\"\xff\xff\xff", (double)g_wave_info.THD);
    Serial_Printf("t3.txt=\"%s\"\xff\xff\xff", Protocol_DistName(g_mode));
}
