#include <buttons.h>
#include <fsm.h>
//#include <spectro.h>
#include "lcd.h"

extern int menu_status;
extern int spectro_status[3];
extern volatile uint64_t ticks;

extern uint8_t param_count;
extern bool new_card;
extern bool card_mounted;
extern Display DisplayLCD;
//Timing stuff for button interrupts
volatile uint64_t button_ticks=0;
uint64_t CD_ticks=0;
uint64_t shutter_ticks=0;
bool shutter_overshoot=false;

gpiopin enter_button ={GPIOC, GPIO2}; //PF12
gpiopin down_button ={GPIOA, GPIO15}; //PB10
gpiopin up_button ={GPIOF, GPIO6};
gpiopin left_button ={GPIOF, GPIO7};
gpiopin return_button ={GPIOC, GPIO3};
gpiopin right_button ={GPIOC, GPIO10};





//Hold the state the current button event
int button_status = NO_BUTTON_EVENT;

//const char* button_text[] = {"up","down","left","right","enter","return"};

/**
 * @brief Setup buttons pins and any other inputs 
 * 
 */
void buttons_setup(){
	//set the buttons' pins to input 
	gpio_mode_setup(enter_button.port,GPIO_MODE_INPUT,GPIO_PUPD_PULLDOWN,(uint16_t) enter_button.pin);
	gpio_mode_setup(return_button.port,GPIO_MODE_INPUT,GPIO_PUPD_PULLDOWN,(uint16_t)return_button.pin);
	gpio_mode_setup(up_button.port,GPIO_MODE_INPUT,GPIO_PUPD_PULLDOWN,(uint16_t)up_button.pin);
	gpio_mode_setup(down_button.port,GPIO_MODE_INPUT,GPIO_PUPD_PULLDOWN,(uint16_t)down_button.pin);
	gpio_mode_setup(left_button.port,GPIO_MODE_INPUT,GPIO_PUPD_PULLDOWN,(uint16_t)left_button.pin);
	gpio_mode_setup(right_button.port,GPIO_MODE_INPUT,GPIO_PUPD_PULLDOWN,(uint16_t)right_button.pin);

	//Setup external interupt 
	//need to be automated switch case or if or even better #if

	/* Configure the EXTI subsystem. */
	exti_select_source(enter_button.pin, enter_button.port);
	exti_set_trigger(enter_button.pin, EXTI_TRIGGER_RISING);

	exti_select_source(down_button.pin, down_button.port);
	exti_set_trigger(down_button.pin, EXTI_TRIGGER_RISING);

	exti_select_source(left_button.pin, left_button.port);
	exti_set_trigger(left_button.pin, EXTI_TRIGGER_RISING);

	exti_select_source(right_button.pin, right_button.port);
	exti_set_trigger(right_button.pin, EXTI_TRIGGER_RISING);

	exti_select_source(up_button.pin, up_button.port);
	exti_set_trigger(up_button.pin, EXTI_TRIGGER_RISING);

	exti_select_source(return_button.pin, return_button.port);
	exti_set_trigger(return_button.pin, EXTI_TRIGGER_RISING);


	
}

/**
 * @brief Enable IRQn for buttons' events
 * 
 */
void enable_button_EXTI(){
	/* Enable EXTIx interrupt. */
	nvic_set_priority(NVIC_EXTI15_10_IRQ, 	0b11000000);
	nvic_set_priority(NVIC_EXTI9_5_IRQ, 	0b11000000);
	nvic_set_priority(NVIC_EXTI2_IRQ, 		0b11000000);
	nvic_set_priority(NVIC_EXTI3_IRQ, 		0b11000000);


	nvic_enable_irq(NVIC_EXTI15_10_IRQ);
	nvic_enable_irq(NVIC_EXTI9_5_IRQ);
	nvic_enable_irq(NVIC_EXTI3_IRQ);
	nvic_enable_irq(NVIC_EXTI2_IRQ);

	
	exti_enable_request(enter_button.pin);
	exti_enable_request(return_button.pin);
	exti_enable_request(up_button.pin);
	exti_enable_request(down_button.pin);
	exti_enable_request(left_button.pin);
	exti_enable_request(right_button.pin);


}






/**
 * @brief ISR for button events
 * 
 */
void exti15_10_isr(){//direction buttons

	if(ticks-button_ticks>=BUTTON_DELAY){
		

		if(exti_get_flag_status(right_button.pin)){
					DisplayLCD.TurnON_event = true;
					button_status=RIGHT_BUTTON_EVENT;
					
				}

		else if(exti_get_flag_status(down_button.pin)){
			DisplayLCD.TurnON_event = true;
			button_status=DOWN_BUTTON_EVENT;
		}
	}
		button_ticks=ticks;
		exti_reset_request(right_button.pin);
		exti_reset_request(down_button.pin);

	
}

/**
 * @brief ISR for button events (cont.)
 * 
 */
void exti9_5_isr(){	//enter & return button

	if(ticks-button_ticks>=BUTTON_DELAY){
		
		if(exti_get_flag_status(up_button.pin)){
			DisplayLCD.TurnON_event = true;
			button_status=UP_BUTTON_EVENT;
			
		}

			
		else if(exti_get_flag_status(left_button.pin)){
			DisplayLCD.TurnON_event = true;
			button_status=LEFT_BUTTON_EVENT;
			
		}
		
		button_ticks=ticks;
}

	exti_reset_request(up_button.pin);
	exti_reset_request(left_button.pin);
		
}	

/**
 * @brief ISR for Btn events (Cont.)
 * Currently not used 
 */
void exti2_isr(){	//card detect : low when inserted : high when removed 
	if(ticks-button_ticks>=BUTTON_DELAY){
	
		DisplayLCD.TurnON_event = true;
		button_status=ENTER_BUTTON_EVENT;
	}
	button_ticks=ticks;
	exti_reset_request(enter_button.pin);

		
}




/**
 * @brief ISR for Btn events (Cont.)
 * Currently not used 
 */
void exti3_isr(){	//card detect : low when inserted : high when removed 
	if(ticks-button_ticks>=BUTTON_DELAY){

		button_status=RETURN_BUTTON_EVENT;
	}
	button_ticks=ticks;
	exti_reset_request(return_button.pin);	

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