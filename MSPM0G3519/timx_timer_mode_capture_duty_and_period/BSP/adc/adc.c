#include "BSP/adc/adc.h"

#include "BSP/bsp.h"
#include "ti_msp_dl_config.h"

static volatile bool s_adc_capture_done = false;

void ADC_Init(void)
{
    SampleTimer_Stop();
    DL_DMA_disableChannel(DMA, DMA_CH2_CHAN_ID);
    DL_ADC12_disableConversions(ADC12_0_INST);

    DL_ADC12_initSingleSample(ADC12_0_INST,
        DL_ADC12_REPEAT_MODE_DISABLED, DL_ADC12_SAMPLING_SOURCE_AUTO,
        DL_ADC12_TRIG_SRC_EVENT, DL_ADC12_SAMP_CONV_RES_12_BIT,
        DL_ADC12_SAMP_CONV_DATA_FORMAT_UNSIGNED);
    DL_ADC12_configConversionMem(ADC12_0_INST, ADC12_0_ADCMEM_0,
        DL_ADC12_INPUT_CHAN_0, DL_ADC12_REFERENCE_VOLTAGE_VDDA_VSSA,
        DL_ADC12_SAMPLE_TIMER_SOURCE_SCOMP0, DL_ADC12_AVERAGING_MODE_DISABLED,
        DL_ADC12_BURN_OUT_SOURCE_DISABLED, DL_ADC12_TRIGGER_MODE_TRIGGER_NEXT,
        DL_ADC12_WINDOWS_COMP_MODE_DISABLED);
    DL_ADC12_setPowerDownMode(ADC12_0_INST, DL_ADC12_POWER_DOWN_MODE_MANUAL);
    DL_ADC12_setSampleTime0(ADC12_0_INST, 3);
    DL_ADC12_enableDMA(ADC12_0_INST);
    DL_ADC12_setDMASamplesCnt(ADC12_0_INST, 1);
    DL_ADC12_enableDMATrigger(ADC12_0_INST, DL_ADC12_DMA_MEM0_RESULT_LOADED);
    DL_ADC12_setSubscriberChanID(ADC12_0_INST, ADC12_0_INST_SUB_CH);

    DL_DMA_setTransferMode(DMA, DMA_CH2_CHAN_ID, DL_DMA_SINGLE_TRANSFER_MODE);
    DL_DMA_setSrcIncrement(DMA, DMA_CH2_CHAN_ID, DL_DMA_ADDR_UNCHANGED);
    DL_DMA_setDestIncrement(DMA, DMA_CH2_CHAN_ID, DL_DMA_ADDR_INCREMENT);
    DL_DMA_setSrcWidth(DMA, DMA_CH2_CHAN_ID, DL_DMA_WIDTH_HALF_WORD);
    DL_DMA_setDestWidth(DMA, DMA_CH2_CHAN_ID, DL_DMA_WIDTH_HALF_WORD);

    DL_ADC12_clearInterruptStatus(ADC12_0_INST, DL_ADC12_INTERRUPT_DMA_DONE);
    NVIC_ClearPendingIRQ(ADC12_0_INST_INT_IRQN);
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
}

bool ADC_StartNormal(uint16_t *buffer, uint16_t sampleCount)
{
    if ((buffer == 0U) || (sampleCount == 0U)) {
        return false;
    }

    SampleTimer_Stop();
    DL_DMA_disableChannel(DMA, DMA_CH2_CHAN_ID);
    DL_ADC12_disableConversions(ADC12_0_INST);

    s_adc_capture_done = false;

    DL_DMA_setSrcAddr(
        DMA, DMA_CH2_CHAN_ID,
        DL_ADC12_getMemResultAddress(ADC12_0_INST, ADC12_0_ADCMEM_0));
    DL_DMA_setDestAddr(DMA, DMA_CH2_CHAN_ID, (uint32_t)buffer);
    DL_DMA_setTransferSize(DMA, DMA_CH2_CHAN_ID, sampleCount);
    DL_DMA_setTransferMode(DMA, DMA_CH2_CHAN_ID, DL_DMA_SINGLE_TRANSFER_MODE);
    DL_DMA_enableChannel(DMA, DMA_CH2_CHAN_ID);

    DL_ADC12_clearInterruptStatus(ADC12_0_INST, DL_ADC12_INTERRUPT_DMA_DONE);
    NVIC_ClearPendingIRQ(ADC12_0_INST_INT_IRQN);

    DL_ADC12_enableConversions(ADC12_0_INST);
    SampleTimer_Start();

    return true;
}

void ADC_Stop(void)
{
    SampleTimer_Stop();
    DL_DMA_disableChannel(DMA, DMA_CH2_CHAN_ID);
    DL_ADC12_disableConversions(ADC12_0_INST);
}

bool ADC_IsCaptureDone(void)
{
    return s_adc_capture_done;
}

void ADC_ClearCaptureDone(void)
{
    s_adc_capture_done = false;
}

void ADC_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_DMA_DONE:
            ADC_Stop();
            s_adc_capture_done = true;
            break;
        default:
            break;
    }
}
