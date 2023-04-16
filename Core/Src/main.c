#include <main.h>
#include <buttons.h>
#include "fatfs_sd.h"
#include <libopencm3/cm3/scb.h>
#include "lcd.h"
#include <fsm.h>
#include "diskio.h"
#include "ff.h"

#include <spi_sd.h>

#include <stm32_adafruit_lcd.h>

#include "ov2640.h"

#define INFINITY_64_UINT 	(uint64_t)(18446744073709551615)


extern int button_status;

extern gpiopin LCD_DrvE;
extern Display DisplayLCD;
extern volatile uint64_t button_ticks;

extern ADC_H ADC_Handler;

extern bool card_mounted;
extern gpiopin card_detect;
extern gpiopin dac_fault1;
extern gpiopin dac_fault2;
extern bool new_card;
extern int menu_status;

extern volatile bool dma_busy;
extern volatile uint64_t timeout_ticks;
extern volatile uint32_t uart_timeout;

extern const uint8_t dofus[153600];

extern volatile ov2640 Camera;

float temperature_inc =0;
float tempo = 123.456;

FATFS FatFs_main;
FIL file_main;
FRESULT fresult_main;

bool card_mounted=false;
gpiopin card_detect={GPIOG,GPIO2};

#define 	HUGE_L	4096
char HUGE_BUFFER[HUGE_L];

RTC Clock = {0,00,00,00};

//extern uint32_t volatile Tx7_buffer[TX7_LEN];
//extern uint32_t volatile Rx7_buffer[RX7_LEN];


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

	//nvic_set_priority(NVIC_HARD_FAULT_IRQ, 0);

	nvic_enable_irq(NVIC_HARD_FAULT_IRQ);
}

void hard_fault_handler(){
	//Request SW reset
	SCB_AIRCR = SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ;
}

void init_MCO1(void) { // 16MHz for the camera PXCLK
	rcc_set_mco(RCC_CFGR_MCO1_HSI);

	uint32_t reg32;

	reg32 = RCC_CFGR;
	reg32 &= ~(RCC_CFGR_MCOPRE_MASK << RCC_CFGR_MCO1PRE_SHIFT);
	RCC_CFGR = (reg32 | (RCC_CFGR_MCOPRE_DIV_NONE << RCC_CFGR_MCO1PRE_SHIFT)); //mco1 = 16MHz

	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8); //PA8
	gpio_set_af(GPIOA, GPIO_AF0, GPIO8); //MCO1

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
	//LEDs GPIOs
	gpio_mode_setup(LED_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, RED_LED|GREEN_LED|BLUE_LED);
	gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, RED_LED|GREEN_LED|BLUE_LED);

	gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO13);

	gpio_mode_setup(card_detect.port,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,(uint16_t)card_detect.pin);

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
	timer_set_period(TIM7,1006); //OF each ms
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

	rcc_periph_clock_enable(RCC_TIM3);
	timer_set_prescaler(TIM3, 81); //1 ticks = 1us
	timer_set_period(TIM3,0xFFFF);
	timer_one_shot_mode(TIM3);
	

}

void tim7_isr(){	//happens every time timer7 overflows
	timer_clear_flag(TIM7,TIM_SR_UIF);
 	//if(timer_get_flag(TIM7,TIM_SR_UIF)){ //Dont need it since it's the only src of the interrupt
	ticks++;

	//Periodic check if card is mounted : 1Hz
	if(ticks%1000==0){

		Clock.Seconds++;

		if(!gpio_get(card_detect.port,card_detect.pin)){ //card_mounted is the previous state so now its unmounted
			gpio_clear(LED_GPIO,RED_LED);
			card_mounted=false;
			unmount_SD();
			//tell the server that the SD is removed
			}
		else if(!card_mounted){
			gpio_clear(LED_GPIO,RED_LED);
			card_mounted=true;
			mount_SD(&FatFs_main); //Need a better way to do this
		}
	
    /*
		if((ticks-button_ticks)>LCD_SLEEP_PERIOD){
			DisplayLCD.TurnOFF_event=true;
		}
	*/ 
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
 * Dont use delay after the main setup, unless you're debuging
 * @param ms no more than 1<<16
 */
void delay_ms(uint16_t ms)	
{
	TIM_ARR(TIM6) = 10*ms-1;
	TIM_EGR(TIM6) = TIM_EGR_UG;
	timer_enable_counter(TIM6);
	while (TIM_CR1(TIM6) & TIM_CR1_CEN);
}


/**
 * @brief 
 * Blocking delay using Timer 3 in oneshot mode.
 * Dont use delay after the main setup, unless you're debuging
 * @param us no more than ?
 */
void delay_us(uint16_t us)	
{
	TIM_ARR(TIM3) = us;
	TIM_EGR(TIM3) = TIM_EGR_UG;
	timer_enable_counter(TIM3);
	while (TIM_CR1(TIM3) & TIM_CR1_CEN);
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
	oled_setup();
	setup_current_M();
	//config_fields();
	buttons_setup();
	uint8_t ret;
	ret = LCD_Init();
	init_display_struct();
	//////////////////////////
	SD_FATFS_InitPins();
	/////////////////////////
	init_MCO1();

    
	OV2640_Init(CAM_320x240);

	uart_printf("starting... %f\n", tempo);
	uart_printf("LCD INIT returns :  %d\n", ret);

	
	// Display ///////////////////////////////////////////
	////////////////////////////////////////////////
	delay_ms(1000);


	//timer_problem=timer_get_counter(TIM5);
	//ssd1306_TestFPS();

	//////////////////
	
	//Check if SD is mounted
	if (gpio_get(card_detect.port,card_detect.pin)){ //0=low=mounted
		card_mounted=true;
		gpio_set(LED_GPIO,RED_LED);
		//uart_printf("found card\n");
	
	}
MOUNT:	;		
	fresult_main=mount_SD(&FatFs_main);
	uart_printf("Mount Result = %d\n", fresult_main);
	if(fresult_main!=FR_OK){
		while (!gpio_get(card_detect.port,card_detect.pin)){
			delay_ms(1000);
		}
		delay_ms(1000);
		card_mounted=true;
		gpio_set(LED_GPIO,RED_LED);
		goto MOUNT;
	}
	else{
		card_mounted=true;
		gpio_set(LED_GPIO,RED_LED);
	}


	
	//////////////////
	
	f_open(&file_main, "Camera_Nucleo.txt", FA_CREATE_NEW|FA_WRITE);
	f_printf(&file_main, "this is a test @ 23:03 10/05/2022");
	f_close(&file_main);

	

	

	
	char* token_main;
	char* token_main2;
	int status;

	//enable timer7 after setup is done (for safety)
	timer_enable_irq(TIM7,TIM_DIER_UIE);
	timer_enable_counter(TIM7);

	///////////////////////////
	asm("NOP");
	uint64_t ticks1=ticks;
	asm("NOP");
	delay_us(1000);
	asm("NOP");
	uint64_t ticks2=ticks-ticks1;
	asm("NOP");
	uart_printf("1000 us ticks = %d\n", (uint32_t) ticks2);
	///////////////////////////
	

	//init_uart_dma();

	start_us_count(&micro);
	led_ticks=ticks+1000;
	while(ticks<led_ticks);
	stop_us_count(&micro);

	uart_printf("1sec = %d cycles\n", micro);
	
	
	start_us_count(&micro);
	asm("NOP");
	LCD_DrawRGB16Image8(0,0,240,320, dofus); //242,294 
	asm("NOP");
	stop_us_count(&micro);

	uart_printf("frame period = %d ms\n",  micro/109000);
	//LCD_IO_test();
	ssd1306_Init();
	delay_ms(1000);
	update_time_oled("Up 0d-00h-00m-00s");
	update_status_oled("Status : Idle");
	update_current_oled("Current : 0 mA");
	update_test_oled("this is a test");
	
	
	char oled_line[20];
	bool first_conv=true;

	enable_button_EXTI();	//start aquiring input after setup
    uint32_t fps=0;
    for(;;){
		int k=0;

		
		lcd_thread();	//main thread 
		if(!Camera.isCapturing && !Camera.CaptureDone ){
            cringe_function();
            asm("NOP");
			CAM_CAPTURE1FRAME(Camera);
            asm("NOP");
            dma_enable_stream(DMA2, DMA_STREAM1); //is it needed or does it trigger automatically ????
            asm("NOP");
            DCMI_CR |= DCMI_CR_CAPTURE;
            asm("NOP");
		}

		if(Camera.CaptureDone){
            fps++;
			start_us_count(&micro);
			asm("NOP");
			LCD_DrawRGB16Image8(0,0,240,320, Camera.RGB565); //242,294 
			asm("NOP");
			stop_us_count(&micro);
            Camera.CaptureDone=false;
		}
		
		//blink every 1 sec 
		if(ticks-led_ticks>1000){	//used to visually check if the code is not frozen somewhere (trap loop, exception, periph failure ...)
			test_ticks=ticks;
			gpio_toggle(GPIOB, GREEN_LED);
			led_ticks=ticks;
			test_ticks=ticks-test_ticks;
			
			
			switch (button_status)
			{
			case NO_BUTTON_EVENT:
				update_status_oled("Status : No Button");
				break;

			case UP_BUTTON_EVENT:
				update_status_oled("Status : Up");
				break;

			case DOWN_BUTTON_EVENT:
				update_status_oled("Status : Down");
				break;

			case RIGHT_BUTTON_EVENT:
				update_status_oled("Status : Right");
				break;

			case LEFT_BUTTON_EVENT:
				update_status_oled("Status : Left");
				break;

			case ENTER_BUTTON_EVENT:
				update_status_oled("Status : Enter");
				break;

			case RETURN_BUTTON_EVENT:
				update_status_oled("Status : Return");
				break;
			
			default:
				update_status_oled("Status : ERROR!");
				break;
			}


			if(Clock.Seconds>=60){
				Clock.Seconds%=60;
				Clock.Minutes++;
			}
			if(Clock.Minutes>=60){
				Clock.Minutes=0;
				Clock.Hours++;
			}
			if(Clock.Hours>=24){
				Clock.Hours=0;
				Clock.Days++;
			}

			sprintf_(oled_line, "Up %dd-%.2dh-%.2dm-%.2ds",Clock.Days, Clock.Hours, Clock.Minutes, Clock.Seconds);
			update_time_oled(oled_line);
			
			if(ADC_Handler.Update_C){
				if(first_conv){
					first_conv=false;

					for (size_t i = 0; i < ADC_FILTER_LENGHT; i++)
					{
						ADC_Handler.FilterAVG[i]+=ADC_Handler.Current_f;
						ADC_Handler.V0AVG[i]+=ADC_Handler.V0_f;
					}
				}

				(++(ADC_Handler.Filter_index) >= ADC_FILTER_LENGHT) ? ADC_Handler.Filter_index=0 : false;
				ADC_Handler.FilterAVG[ADC_Handler.Filter_index]=ADC_Handler.Current_f;
				ADC_Handler.V0AVG[ADC_Handler.Filter_index]=ADC_Handler.V0_f;

				ADC_Handler.Current_f=0;
				ADC_Handler.V0_f=0;


				for (size_t i = 0; i < ADC_FILTER_LENGHT; i++)
				{
					ADC_Handler.Current_f+=ADC_Handler.FilterAVG[i];
					ADC_Handler.V0_f+=ADC_Handler.V0AVG[i];
				}
				ADC_Handler.Current_f/=ADC_FILTER_LENGHT;
				ADC_Handler.V0_f/=ADC_FILTER_LENGHT;


				uint32_t it = (uint32_t) ADC_Handler.Current_f;
				ADC_Handler.Current_f-=ADC_Handler.V0_f;
				ADC_Handler.Current_f*=3300;
				ADC_Handler.Current_f/=4096;

				if(ADC_Handler.Current_f<0){
					ADC_Handler.Current=0;
				}

				ADC_Handler.Current_f*=10;
				ADC_Handler.Current_f/=4;
				ADC_Handler.Current=(uint32_t) ADC_Handler.Current_f;
				uint32_t v0 = (uint32_t) ADC_Handler.V0_f;

				sprintf_(oled_line, "Current : %dmA",ADC_Handler.Current);
				update_current_oled(oled_line);
				sprintf_(oled_line, "FPS : %d", fps);
				update_test_oled(oled_line);
				ADC_Handler.Update_C=false;	
			}
			else{
				adc_start_conversion_regular(ADC_CURRENT);

			}

			ssd1306_UpdateScreen();

			fps=0;
		}
	}
	return 0;
}

