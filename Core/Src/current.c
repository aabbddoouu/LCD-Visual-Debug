#include <current.h>

#define ZERO_mV  2260

ADC_H ADC_Handler={
    0,0,0,0,true
};


void setup_current_M(){
    rcc_periph_clock_enable(RCC_ADC3);
    gpio_mode_setup(GPIOF, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO3);
    gpio_mode_setup(GPIOF, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO10);
    adc_disable_dma(ADC3);
    adc_power_off(ADC_CURRENT);
    adc_set_clk_prescale(0x0);
    adc_set_resolution(ADC_CURRENT, ADC_CR1_RES_12BIT);
    adc_set_sample_time(ADC_CURRENT, ADC_CURRENT_CHANNEL, ADC_SMPR_SMP_480CYC);
    adc_set_sample_time(ADC_CURRENT, ADC_V0_CHANNEL, ADC_SMPR_SMP_480CYC);
    adc_enable_scan_mode(ADC_CURRENT);
    adc_set_single_conversion_mode(ADC_CURRENT);
    uint8_t Channel_T[] = {ADC_V0_CHANNEL, ADC_CURRENT_CHANNEL};
    adc_set_regular_sequence(ADC_CURRENT,2, Channel_T);
    adc_set_right_aligned(ADC_CURRENT);
    adc_eoc_after_each(ADC_CURRENT);//EOC is set after each sequence sequence
    adc_enable_eoc_interrupt(ADC_CURRENT);
    nvic_enable_irq(NVIC_ADC_IRQ);
    nvic_set_priority(NVIC_ADC_IRQ,0b11000000); 
    adc_power_on(ADC_CURRENT);

    ADC_Handler.Filter_index=0;

}

void adc_isr(){
    if(ADC_SR(ADC_CURRENT) & ADC_SR_EOC){
        if(ADC_Handler.first_conv){
            ADC_Handler.V0_f=ADC_DR(ADC_CURRENT);
            ADC_Handler.first_conv=false;
            
        }
        else{
            ADC_Handler.Current_f=ADC_DR(ADC_CURRENT); //get the 12-bit converted value
            ADC_Handler.first_conv=true;
            ADC_Handler.Update_C=true;
        }
    }
}