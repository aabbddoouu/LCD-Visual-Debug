#pragma once

#include <main.h>




#define UP_BUTTON_EVENT		0
#define DOWN_BUTTON_EVENT 	1
#define LEFT_BUTTON_EVENT 	2
#define RIGHT_BUTTON_EVENT 	3
#define ENTER_BUTTON_EVENT 	4
#define RETURN_BUTTON_EVENT 5
#define NO_BUTTON_EVENT		9

#define BUTTON_DELAY 200 //(ms)

#define CD_DELAY 	100 //sec 


/**
 * @brief struct to compact 
 * 
 */
typedef struct
{
	uint32_t gpio;
	uint32_t pin;
}gpiopin;







void button_thread();
void enable_button_EXTI();
void buttons_setup();
