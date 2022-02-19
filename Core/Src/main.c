#include <main.h>
#include <menu.h>
#include <buttons.h>
#include <spectro.h>
#include "fatfs_sd.h"
#include <libopencm3/cm3/scb.h>
#include "lcd.h"

#include "parameters.h"
#include "diskio.h"
#include "ff.h"

#include <spi_sd.h>

#include <stm32_adafruit_lcd.h>

#define INFINITY_64_UINT 	(uint64_t)(18446744073709551615)

extern uint8_t image_logo[ILI9341_LCD_PIXEL_HEIGHT*ILI9341_LCD_PIXEL_WIDTH*2];
extern gpiopin LCD_DrvE;
extern Display DisplayLCD;
extern volatile uint64_t button_ticks;
extern char menu_field[MENU_FIELDS][Line_Max_Char]; 
extern bool card_mounted;
extern gpiopin card_detect;
extern gpiopin dac_fault1;
extern gpiopin dac_fault2;
extern bool new_card;
extern int menu_status;

extern volatile bool dma_busy;
extern volatile uint64_t timeout_ticks;
extern volatile uint32_t uart_timeout;



float temperature_inc =0;
float tempo = 123.456;

FATFS FatFs_main;
FIL file_main;
FRESULT fresult_main;

#define 	HUGE_L	4096
char HUGE_BUFFER[HUGE_L];


extern uint32_t volatile Tx7_buffer[TX7_LEN];
extern uint32_t volatile Rx7_buffer[RX7_LEN];


/////////////////////////////////////////////////////
//// UART printf SECTION
/////////////////////////////////////////////////////

// used to counter a compiling error (nneded for str manipulation for debugging)
void *_sbrk(int incr) { return (void *)-1; }

void _putchar(char character)
{
	// send char to UART
	usart_send_blocking(USART3,character);  //USART3 is connected to ST-link serial com
}
int uart_printf(const char *format,...) {

	va_list va;
	va_start(va, format);
	const int ret = vprintf_(format, va);
	va_end(va);
	return ret;

}

//with 32 bits it OFs @ 50 days
//with 64 bits it OFs @ half a BILLION years
//inc every ms
volatile uint64_t ticks=0;	
//
volatile uint32_t ticks_us=0; //inc every µs 
volatile uint64_t led_ticks=0;


///////////////////////////////////////////////////////////////////////////////
//DEBUG VARs
volatile int32_t micro=0;
volatile int32_t test_ticks=0;
bool dump_info=false;
static bool first=true;
static uint32_t inc=0;
static bool dac1_wasopen = true;
static bool dac2_wasopen = true;
///////////////////////////////////////////////////////////////////////////////
static void clock_setup(void)
{
	
	rcc_clock_setup_hsi(&rcc_3v3[RCC_CLOCK_3V3_216MHZ]);
	/* Enable GPIOC clock. */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOE);
	rcc_periph_clock_enable(RCC_GPIOF);
	rcc_periph_clock_enable(RCC_GPIOG);
	/* Enable syscfg for exti port mapping */
	rcc_periph_clock_enable(RCC_SYSCFG);

	nvic_set_priority(NVIC_HARD_FAULT_IRQ, 0);

	nvic_enable_irq(NVIC_HARD_FAULT_IRQ);
}

void hard_fault_handler(){
	//Request SW reset
	SCB_AIRCR = SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ;
}

void init_usart(void) {

	//////////////////////////////////////////////////////////////
	// STM32F7:
	//	RX:	-
	//	TX:	D8
	//	Baud:	115200
	//	used for uart debug if we ever need it 
	//	Connect uart 3 to the embedded st-link comm pins to establish a serial connection with PC
	//////////////////////////////////////////////////////////////
	gpio_mode_setup(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8);
	gpio_set_output_options(GPIOD,GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO8);
	gpio_set_af(GPIOD, GPIO_AF7, GPIO8); //set PD8 to UART3_tx

	rcc_periph_clock_enable(RCC_USART3);

	usart_set_baudrate(USART3,128000);
	usart_set_databits(USART3,8);
	usart_set_stopbits(USART3,USART_STOPBITS_1);
	usart_set_mode(USART3,USART_MODE_TX);
	usart_set_parity(USART3,USART_PARITY_NONE);
	usart_set_flow_control(USART3,USART_FLOWCONTROL_NONE);
	usart_enable(USART3);
}

static void gpio_setup(void)
{
	/* Set GPIOB0 LED to 'output' */
	gpio_mode_setup(LED_GPIO, GPIO_MODE_OUTPUT,
		      GPIO_PUPD_NONE, RED_LED|GREEN_LED|BLUE_LED);

	gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, RED_LED|GREEN_LED|BLUE_LED);

	gpio_mode_setup(GPIOC, GPIO_MODE_INPUT,
		      GPIO_PUPD_PULLUP, GPIO13);
}


void delay_setup(void)
{
	/* set up a millisecond free running timer for delay operations used during the main setup */
	rcc_periph_clock_enable(RCC_TIM6);
	/* millisecond counter */
	timer_set_prescaler(TIM6, 10800); //TIMx are clocked by 2*apbx ; increment each 500us
	timer_set_period(TIM6, 0xFFFF); //load with highest value
	timer_one_shot_mode(TIM6);
	
	/* set up a microsecond free running timer for ... things... */
	rcc_periph_clock_enable(RCC_TIM7);
	/* microsecond counter */
	timer_set_prescaler(TIM7, 108); //TIMx are clocked by 2*apbx ; increment each 1us
	timer_set_period(TIM7,1000); //OF each ms
	timer_continuous_mode(TIM7);
	//timer_update_on_overflow(TIM7);
	nvic_set_priority(NVIC_TIM7_IRQ,0b01000000);
	nvic_enable_irq(NVIC_TIM7_IRQ);
	
	//timer_enable_irq(TIM7,TIM_DIER_UIE);
	//timer_enable_counter(TIM7);
	//timer_update_on_overflow(TIM7);

	rcc_periph_clock_enable(RCC_TIM5);
	timer_set_prescaler(TIM5, 0);
	timer_set_period(TIM5,0xFFFFFFFF);
	timer_one_shot_mode(TIM5);
	

}

void tim7_isr(){	//happens every time timer7 overflows
	timer_clear_flag(TIM7,TIM_SR_UIF);
 	//if(timer_get_flag(TIM7,TIM_SR_UIF)){
		ticks++;

		//Periodic check if card is mounted : 1Hz
		if(ticks%1000==0 ){
			gpio_toggle(LED_GPIO,BLUE_LED);

			if(gpio_get(card_detect.gpio,card_detect.pin)){ //card_mounted is the previous state so now its unmounted
				if(card_mounted){	
					gpio_clear(LED_GPIO,RED_LED);
					card_mounted=false;
					menu_status=MENU_NEWSD;
					new_card=true;
					unmount_SD();
					Display_Card(false);
				}
			}
			else if(new_card){	//now its mounted
				gpio_set(LED_GPIO,RED_LED);
				card_mounted=true;
				new_card=false;
				menu_status=MENU_IDLE;
				mount_SD(&FatFs_main);
				Display_Card(true);
			}


			if(!gpio_get(dac_fault1.gpio,dac_fault1.pin)){
				//dac fault1
				if (!dac1_wasopen){
					Display_DAC1(false);
					dac1_wasopen=true;
				}

			}
			else{
				if (dac1_wasopen){
					Display_DAC1(true);
					dac1_wasopen=false;
				}
			}

			
			if(!gpio_get(dac_fault2.gpio,dac_fault2.pin)){
				//dac fault2
				if (!dac2_wasopen){
					Display_DAC2(false);
					dac2_wasopen=true;
				}

			}
			else{
				if (dac2_wasopen){
					Display_DAC2(true);
					dac2_wasopen=false;
				}
			}


			
		}

		
}

void start_us_count(uint32_t *c){
	TIM_EGR(TIM5) = TIM_EGR_UG;
	TIM_CR1(TIM5) |= TIM_CR1_CEN;
	*c=TIM_CNT(TIM5);
}

void stop_us_count(uint32_t *c){
	TIM_CR1(TIM5) &= ~TIM_CR1_CEN;
	*c=TIM_CNT(TIM5)-(*c);
}

/**
 * @brief 
 * Blocking delay using Timer 6 in oneshot mode.
 * Dont use delay after main setup, unless you're debuging
 * @param ms no more than 1<<16
 */
void delay_ms(uint16_t ms)	
{
	TIM_ARR(TIM6) = 10*ms-1;
	TIM_EGR(TIM6) = TIM_EGR_UG;
	timer_enable_counter(TIM6);
	while (TIM_CR1(TIM6) & TIM_CR1_CEN);
}






TCHAR* f_gets_debug (
	TCHAR* buff,	/* Pointer to the buffer to store read string */
	int len,		/* Size of string buffer (items) */
	FIL* fp,		/* Pointer to the file object */
	FRESULT* read_res
)
{
	int nc = 0;
	TCHAR *p = buff;
	BYTE s[4];
	UINT rc;
	DWORD dc;


			/* Byte-by-byte read without any conversion (ANSI/OEM API) */
	len -= 1;	/* Make a room for the terminator */
	while (nc < len) {
		*read_res=f_read(fp, s, 1, &rc);	/* Get a byte */
		if (rc != 1) break;		/* EOF? */
		dc = s[0];
		if (FF_USE_STRFUNC == 2 && dc == '\r') continue;
		*p++ = (TCHAR)dc; nc++;
		if (dc == '\n') break;
	}


	*p = 0;		/* Terminate the string */
	return nc ? buff : 0;


}
















//////////////////////////////////////////////////////////
uint16_t length, r_length;
uint16_t int_status1, int_status2;
uint16_t msg_status;
uint32_t load[100];
uint16_t ret[100];
uint32_t ret32[1025];
volatile uint8_t swap_endian[4]; 
volatile uint32_t loop_micro=0;
/////////////////////////////////////////////////////////////




int main(){
	//char** setting[]={"Setting Spectro","Setting Spectro.","Setting Spectro..","Setting Spectro..."};
	////////////////////////////////////////////////
	// setup peripherals
    clock_setup();
	delay_setup();
    gpio_setup();
    init_usart();
	config_fields();
	buttons_setup();
	LCD_Init();
	//////////////////////////
	SD_FATFS_InitPins();
	/////////////////////////

	uart_printf("starting... %f\n", tempo);

	// Display ///////////////////////////////////////////
	Draw_Overlay();
	Draw_Graph_Axis();
	////////////////////////////////////////////////
	delay_ms(1000);
	

	//timer_problem=timer_get_counter(TIM5);
	//ssd1306_TestFPS();

	//////////////////
	//Check if SD is mounted
	if (!gpio_get(card_detect.gpio,card_detect.pin)){ //0=low=mounted
		card_mounted=true;
		gpio_set(LED_GPIO,RED_LED);
		Display_Card(true);
		//uart_printf("found card\n");
	
	}	
	fresult_main=mount_SD(&FatFs_main);
	if(fresult_main!=FR_OK){
		Display_Card(false);
		Display_Status(menu_field[INSERTSD_FIELD], LCD_COLOR_RED);
		while (gpio_get(card_detect.gpio,card_detect.pin)){
			delay_ms(1000);
		}
		card_mounted=true;
		new_card=false;
		gpio_set(LED_GPIO,RED_LED);
		
		delay_ms(1000);	
		
	}
	else{
		card_mounted=true;
		gpio_set(LED_GPIO,RED_LED);
		new_card=false;
	}

	Display_Card(true);
	// Load all parameters
	load_params_SD();
	// Scan for language packs
	scan_files(LANG_DIR);
	// Load the 1st language pack : English generally
	load_language_pack();

	// init Qneo struct
	init_Qneo();

	//Reload all measurmement parameter before to take into account the algorithm chosen
	Reload_Params();
	delay_ms(1000);
	//////////////////

	enable_button_EXTI();	//start aquiring input after setup



	
	char* token_main;
	char* token_main2;
	int status;


	if(!gpio_get(dac_fault1.gpio,dac_fault1.pin)){
		dac1_wasopen=true;
		Display_DAC1(false);
	}
	else{
		dac1_wasopen=false;
		Display_DAC1(true);
	}

	if(!gpio_get(dac_fault2.gpio,dac_fault2.pin)){
		dac2_wasopen=true;
		Display_DAC2(false);
	}
	else{
		dac2_wasopen=false;
		Display_DAC2(true);
	}

	//enable timer7 after setup is done (for safety)
	timer_enable_irq(TIM7,TIM_DIER_UIE);
	timer_enable_counter(TIM7);

	///////////////////////////
	
	///////////////////////////
	

	init_DAC_spi();
	setup_Spectro_usart();
	init_uart_dma();

	delay_ms(1000);

	

	
	start_us_count(&micro);
	led_ticks=ticks+1000;
	while(ticks<led_ticks);
	stop_us_count(&micro);

	uart_printf("1sec = %d cycles\n", micro);


    for(;;){
		int k=0;

		
		//spectro_thread();	//main thread 
		
		//blink every 1 sec 
		if(ticks-led_ticks>1000){	//used to visually check if the code is not frozen somewhere (trap loop, exception, periph failure ...)
			test_ticks=ticks;
			gpio_toggle(GPIOB, GREEN_LED);
			led_ticks=ticks;
			test_ticks=ticks-test_ticks;
		}
	}
	return 0;
}

