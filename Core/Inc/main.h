#ifndef MAIN_H
#define MAIN_H

#ifndef STM32F7
#define STM32F7
#endif

#include "printf.h"
#include <stdio.h>	
#include <string.h>
#include <stdlib.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencmsis/core_cm3.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/adc.h>
#include <math.h>
#include <limits.h>
#include <libopencm3/cm3/systick.h>
#include <defines.h>
#include <errno.h>
#include <dma.h>
#include <flash.h>
#include <buttons.h>
#include <i2c_oled.h>
#include <current.h>
#include <exti.h>

#define LED_GPIO    GPIOB
#define RED_LED     GPIO14
#define GREEN_LED   GPIO0
#define BLUE_LED    GPIO7




int uart_printf(const char *format,...) __attribute((format(printf,1,2)));

typedef struct
{
    uint32_t    Days;
    uint8_t     Hours;
    uint8_t     Minutes;
    uint8_t     Seconds
}RTC;


#define FLOAT_ERROR 1e-6f   //can be used for float comparison ? if it's not used by the FPU


void delay_ms(uint16_t ms);
void delay_us(uint16_t us);
void stop_us_count(uint32_t *c);
void start_us_count(uint32_t *c);

#endif