#pragma once

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
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <math.h>
#include <limits.h>
#include <libopencm3/cm3/systick.h>
#include <defines.h>
#include <errno.h>
#include <dma.h>
#include <flash.h>
#include <buttons.h>

#define LED_GPIO    GPIOB
#define RED_LED     GPIO14
#define GREEN_LED   GPIO0
#define BLUE_LED    GPIO7


int uart_printf(const char *format,...) __attribute((format(printf,1,2)));



#define FLOAT_ERROR 1e-6f   //can be used for float comparison ? if it's not used by the FPU

#define BUFFER_length   50

char buffer[BUFFER_length]; //used for uart str buffer maybe other things too ?


void delay_ms(uint16_t ms);
void stop_us_count(uint32_t *c);
void start_us_count(uint32_t *c);
