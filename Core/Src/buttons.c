#include <buttons.h>
#include <menu.h>
#include <spectro.h>
#include "lcd.h"

extern int menu_status;
extern int spectro_status[3];
extern volatile uint64_t ticks;
extern parameter Temperature;
extern parameter Sampling_freq;
extern uint8_t param_count;
extern bool new_card;
extern bool card_mounted;
extern Display DisplayLCD;
//Timing stuff for button interrupts
volatile uint64_t button_ticks=0;
uint64_t CD_ticks=0;
uint64_t shutter_ticks=0;
bool shutter_overshoot=false;

gpiopin enter_button ={GPIOF, GPIO12}; //PF12
gpiopin down_button ={GPIOB, GPIO10}; //PB10
gpiopin up_button ={GPIOE, GPIO11};
gpiopin left_button ={GPIOF, GPIO14};
gpiopin return_button ={GPIOE, GPIO13};
gpiopin right_button ={GPIOF, GPIO15};


gpiopin card_detect={GPIOG,GPIO2};

gpiopin dac_fault1={DAC_FAULT_PORT,DAC_FAULT1};
gpiopin dac_fault2={DAC_FAULT_PORT,DAC_FAULT2};


//Hold the state the current button event
int button_status = NO_BUTTON_EVENT;

//const char* button_text[] = {"up","down","left","right","enter","return"};

/**
 * @brief Setup buttons pins and any other inputs 
 * 
 */
void buttons_setup(){
	//set the buttons' pins to input 
	gpio_mode_setup(enter_button.gpio,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,(uint16_t) enter_button.pin);
	gpio_mode_setup(return_button.gpio,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,(uint16_t)return_button.pin);
	gpio_mode_setup(up_button.gpio,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,(uint16_t)up_button.pin);
	gpio_mode_setup(down_button.gpio,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,(uint16_t)down_button.pin);
	gpio_mode_setup(left_button.gpio,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,(uint16_t)left_button.pin);
	gpio_mode_setup(right_button.gpio,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,(uint16_t)right_button.pin);
	gpio_mode_setup(card_detect.gpio,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,(uint16_t)card_detect.pin);
	gpio_mode_setup(dac_fault1.gpio,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,(uint16_t)dac_fault1.pin);
	gpio_mode_setup(dac_fault2.gpio,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,(uint16_t)dac_fault2.pin);

	//Setup external interupt 
	//need to be automated switch case or if or even better #if

	/* Configure the EXTI subsystem. */
	exti_select_source(enter_button.pin, enter_button.gpio);
	exti_set_trigger(enter_button.pin, EXTI_TRIGGER_RISING);

	exti_select_source(down_button.pin, down_button.gpio);
	exti_set_trigger(down_button.pin, EXTI_TRIGGER_RISING);

	exti_select_source(left_button.pin, left_button.gpio);
	exti_set_trigger(left_button.pin, EXTI_TRIGGER_RISING);

	exti_select_source(right_button.pin, right_button.gpio);
	exti_set_trigger(right_button.pin, EXTI_TRIGGER_RISING);

	exti_select_source(up_button.pin, up_button.gpio);
	exti_set_trigger(up_button.pin, EXTI_TRIGGER_RISING);

	exti_select_source(return_button.pin, return_button.gpio);
	exti_set_trigger(return_button.pin, EXTI_TRIGGER_RISING);

	exti_select_source(card_detect.pin, card_detect.gpio);
	if(card_mounted){
		//exti_set_trigger(card_detect.pin, EXTI_TRIGGER_RISING);
	}else
	{
		//exti_set_trigger(card_detect.pin, EXTI_TRIGGER_FALLING);
	}

	//exti_select_source(dac_fault1.pin, dac_fault1.gpio);
	//exti_set_trigger(dac_fault1.pin, EXTI_TRIGGER_FALLING);
	//exti_select_source(dac_fault2.pin, dac_fault2.gpio);
	//exti_set_trigger(dac_fault1.pin, EXTI_TRIGGER_FALLING);
	
}

/**
 * @brief Enable IRQn for buttons' events
 * 
 */
void enable_button_EXTI(){
	/* Enable EXTIx interrupt. */
	nvic_set_priority(NVIC_EXTI15_10_IRQ, 	0b10000000);
	nvic_set_priority(NVIC_EXTI9_5_IRQ, 	0b10000000);
	nvic_set_priority(NVIC_EXTI4_IRQ, 		0b10000000);


	nvic_enable_irq(NVIC_EXTI15_10_IRQ);
	//nvic_enable_irq(NVIC_EXTI9_5_IRQ);
	//nvic_enable_irq(NVIC_EXTI4_IRQ);

	
	exti_enable_request(enter_button.pin);
	exti_enable_request(return_button.pin);
	exti_enable_request(up_button.pin);
	exti_enable_request(down_button.pin);
	exti_enable_request(left_button.pin);
	exti_enable_request(right_button.pin);
	//exti_enable_request(dac_fault1.pin);
	//exti_enable_request(dac_fault2.pin);
	//exti_enable_request(card_detect.pin);

}

/**
 * @brief ISR for button events
 * 
 */
void exti15_10_isr(){//direction buttons
	if(ticks-button_ticks>=BUTTON_DELAY){
		spectro_status[2]=SPECTRO_MENU;
		
		if(exti_get_flag_status(up_button.pin)){
			DisplayLCD.TurnON_event = true;
			button_status=UP_BUTTON_EVENT;
			
		}
		else if(exti_get_flag_status(down_button.pin)){
			DisplayLCD.TurnON_event = true;
			button_status=DOWN_BUTTON_EVENT;
			
		}
		else if(exti_get_flag_status(left_button.pin)){
			DisplayLCD.TurnON_event = true;
			button_status=LEFT_BUTTON_EVENT;
			
		}
		else if(exti_get_flag_status(right_button.pin)){
			DisplayLCD.TurnON_event = true;
			button_status=RIGHT_BUTTON_EVENT;
			
		}
	
		if(exti_get_flag_status(enter_button.pin)){
			DisplayLCD.TurnON_event = true;
			button_status=ENTER_BUTTON_EVENT;

		}
		else if(exti_get_flag_status(return_button.pin)){
	
			button_status=RETURN_BUTTON_EVENT;

		}
		button_ticks=ticks;
	}

	

	exti_reset_request(enter_button.pin);
	exti_reset_request(return_button.pin);

	

	exti_reset_request(up_button.pin);
	exti_reset_request(down_button.pin);
	exti_reset_request(left_button.pin);
	exti_reset_request(right_button.pin);
}

/**
 * @brief ISR for button events (cont.)
 * 
 */
void exti9_5_isr(){	//enter & return button
/*
	if(ticks-button_ticks>=BUTTON_DELAY){
		spectro_status[2]=SPECTRO_MENU;
		if(exti_get_flag_status(enter_button.pin)){
			button_status=ENTER_BUTTON_EVENT;


		}
		else if(exti_get_flag_status(return_button.pin)){
			button_status=RETURN_BUTTON_EVENT;


		}
		button_ticks=ticks;
	}

*/
}

/**
 * @brief ISR for sd card detection
 * Currently not used 
 */
void exti2_isr(){	//card detect : low when inserted : high when removed 
	if(ticks-CD_ticks>=CD_DELAY ){
		if(card_mounted){ //card_mounted is the previous state so now its unmounted
			gpio_clear(LED_GPIO,RED_LED);
			card_mounted=false;
			new_card=true;
			CD_ticks=ticks;
			//cd pin is high, we watch for falling edge
			exti_set_trigger(return_button.pin, EXTI_TRIGGER_FALLING);
			uart_printf("card unmounted\n");
		}
		else{	//now its mounted
			gpio_set(LED_GPIO,RED_LED);
			card_mounted=true;
			CD_ticks=ticks;
			spectro_status[2]=SPECTRO_MENU;
			menu_status=MENU_IDLE;
			//cd pin is low, we watch for rising edge
			exti_set_trigger(return_button.pin, EXTI_TRIGGER_RISING);
			uart_printf("card mounted\n");
		}
	}
	
	
	exti_reset_request(card_detect.pin);

}


void button_thread(){
	switch (button_status)
	{
	case NO_BUTTON_EVENT:
		//waiting
		break;
	case ENTER_BUTTON_EVENT:
		//write_update_oled(button_text[button_event]);
		//uart_printf("\n");


		button_status=NO_BUTTON_EVENT;
		break;
	case RETURN_BUTTON_EVENT:
		//write_update_oled(button_text[button_event]);
		//uart_printf("\n");
		button_status=NO_BUTTON_EVENT;
		break;
	case UP_BUTTON_EVENT:
		//write_update_oled(button_text[button_event]);
		//uart_printf("\n");
		button_status=NO_BUTTON_EVENT;
		break;
	case DOWN_BUTTON_EVENT:
		//write_update_oled(button_text[button_event]);
		//uart_printf("\n");
		button_status=NO_BUTTON_EVENT;
		break;
	case RIGHT_BUTTON_EVENT:
		//write_update_oled(button_text[button_event]);
		//uart_printf("\n");
		button_status=NO_BUTTON_EVENT;
		break;
	case LEFT_BUTTON_EVENT:
		//write_update_oled(button_text[button_event]);
		//uart_printf("\n");

		button_status=NO_BUTTON_EVENT;
		break;
	
	default:
		//write_update_oled("error !");
		break;
	}

}