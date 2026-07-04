/**
 * 全局配置参数 — MSPM0G3519 定时器捕获 + 信号处理模板
 *
 *   所有可调参数集中于此文件，模块头文件不再分散定义配置常量。
 */

#ifndef __APP_CONFIG_H
#define __APP_CONFIG_H

/*===========================================================================
 * 一、系统硬件参数
 *===========================================================================*/
#define CPU_FREQ_HZ           80000000U   /* Cortex-M0+ max CPU clock. */
#define FLASH_SIZE_KB         512U        /* MSPM0G3519 program flash. */
#define DATA_FLASH_SIZE_KB    16U         /* Dedicated data flash bank. */
#define SRAM_SIZE_KB          128U        /* MSPM0G3519 total SRAM. */
#define ADC_MAX_SPS           4000000U    /* 12-bit ADC maximum sample rate. */
#define DAC_RESOLUTION        4095U       /* 12-bit DAC full scale. */
#define UART_INSTANCE_COUNT   7U
#define I2C_INSTANCE_COUNT    3U
#define SPI_INSTANCE_COUNT    3U
#define CANFD_INSTANCE_COUNT  2U

/*---- 定时器时钟 ----*/
#define CAPTURE_TIMER_CLOCK_HZ 80000000U  /* TIMA1 capture clock in SysConfig. */
#define PWM_TIMER_CLOCK_HZ     20000000U  /* TIMG8 PWM clock in SysConfig. */
#define CAPTURE_TIMER_LOAD_VALUE 65535U

/*===========================================================================
 * 二、ADC / DMA 采样参数
 *===========================================================================*/
#define ADC_DEFAULT_SAMPLE_RATE  102400U  /* 默认 ADC 采样率 (Hz)                   */

/*===========================================================================
 * 三、q15 FFT 频谱分析参数（实时链路）
 *===========================================================================*/
#define FFT_Q15_N               256       /* q15 FFT 点数 (256/512/1024/2048/4096)  */
#define FFT_Q15_N_2             (FFT_Q15_N / 2)
#define FFT_Q15_SAMPLE_RATE     102400    /* ADC 采样率 (Hz)                        */
#define FFT_Q15_SCALE_SHIFT     3         /* 采样值左移位数，防 q15 精度丢失        */

/*===========================================================================
 * 四、f32 FFT 频谱分析参数（离线高精度）
 *===========================================================================*/
#define FFT_N               2048          /* FFT 点数（2 的幂）                     */
#define FFT_N_2             (FFT_N / 2)   /* 频谱有效点数                            */
#define FREQ_S              102400        /* ADC 采样率 (Hz)                        */

#define DC_SCOPE            80            /* 直流分量屏蔽范围 (Hz)                   */
#define FREQ_SCOPE          0             /* 峰值搜索邻域 (Hz)，0=禁用               */
#define FREQ_STEP           10            /* 频率规整步长 (Hz)                       */
#define HAMMING_WIN_CORR    1.85185f      /* 汉明窗幅度恢复系数                      */

#define MIN_VALID_DENOM     1e-12f        /* 防止除零的保护值                        */
#define FREQ_LIMIT_HIGH     200000        /* 频率上限 (Hz)                           */
#define FREQ_LIMIT_LOW      5000          /* 频率下限 (Hz)                           */

/*===========================================================================
 * 五、DSP 数字信号处理参数
 *===========================================================================*/
#define DSP_FS_HZ           500000        /* 滤波器设计基准采样率 (Hz)               */

/* RLS 自适应滤波器默认参数 */
#define DSP_RLS_DELTA       100.0f        /* P 矩阵初始逆对角 = 1/delta              */
#define DSP_RLS_LAMBDA      0.999f        /* 遗忘因子 (0.95~0.999)                   */

/* LMS / NLMS 自适应滤波器默认步长 */
#define DSP_LMS_MU          0.01f         /* 步长因子                                */

/*===========================================================================
 * 六、UART 通信参数
 *===========================================================================*/
#define RXBUFFERSIZE         500          /* 串口环形接收缓冲区大小                  */
#define CONSOLE_BAUDRATE     115200       /* 控制台波特率                            */

/*===========================================================================
 * 七、任务调度周期
 *===========================================================================*/
#define TASK_ADC_PERIOD_MS   10           /* ADC 数据采集+FFT 处理周期 (ms)         */
#define TASK_UART_PERIOD_MS  100          /* 串口结果输出周期 (ms)                   */

#endif /* __APP_CONFIG_H */
