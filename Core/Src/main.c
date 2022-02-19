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

#include "AMS_AlgoDll.h"
#include "AsterProcessing.h"
#include <spi_sd.h>

#include <shutter.h>
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
extern int Gflg;
extern volatile bool dma_busy;
extern volatile uint64_t timeout_ticks;
extern volatile uint32_t uart_timeout;
extern FLOAT gLambda[LEN_PIX];
extern int spectro_status[3];
extern volatile Spectro Qneo;
float temperature_inc =0;
float tempo = 123.456;

FATFS FatFs_main;
FIL file_main;
FRESULT fresult_main;

#define 	HUGE_L	4096
char HUGE_BUFFER[HUGE_L];


extern uint32_t volatile Tx7_buffer[TX7_LEN];
extern uint32_t volatile Rx7_buffer[RX7_LEN];


extern volatile FLOAT maxL1;
extern volatile FLOAT maxL2;

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

		//Timeout check
		if(ticks>timeout_ticks && dma_busy){ 
			timeout_ticks = UINT64_MAX; //(uint64_t)(18446744073709551615)
			Qneo.Default=true;
			//disable Rx/Tx DMA
			dma_busy=false;
			
			dma_disable_stream(DMA1, DMA_STREAM0);
			dma_disable_stream(DMA1, DMA_STREAM7);
			spectro_status[0]=SPECTRO_DEFAULT;

		}

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

		//Periodic Request measurement : (10Hz default)
		if(ticks%Qneo.MPeriod_ms==0/*ticks%1000==0*/){
			if(spectro_status[0]==SPECTRO_IDLE){
				spectro_status[1]=SPECTRO_CONFIG;

				if(ticks%Qneo.CPeriod_ms==0){  //1h
					spectro_status[1]=SPECTRO_CALIB;
				}
			}
		}

		if((ticks-button_ticks)>LCD_SLEEP_PERIOD && !DisplayLCD.TurnOFF_event){
			DisplayLCD.TurnOFF_event=true;
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


	//while(1);

SPECTRO_S:
	Spectro_Setup();
	if(Qneo.Default){
		Display_Spectro(false);
        uart_printf("Spectro Not found in setup ! Retrying ...\n");
		Display_Status(menu_field[NOSP_ST_FIELD], LCD_COLOR_RED);
		Qneo.Default=false;
		//timeout_ticks=ticks+uart_timeout;
		reset_Qneo();
		delay_ms(2000);
		goto SPECTRO_S;
    }
	Display_Spectro(true);
	Display_Status(menu_field[SP_FOUND_FIELD], LCD_COLOR_BLACK);
	upscale_Wavelengths();	//set Fdt too
	delay_ms(1000);
	uart_printf("Wavelengths extracted\n");
/* 	
	Calibrate_Dark(6000,10);
	if(Qneo.Default){
        uart_printf("Spectro Not found in 1st Calib ! MCU is halted\n");
		while(1);
	} */
	
	
	//////////////////////////////////////////////////////////////////////////////
	// Algo Initialization
	if(Qneo.Aster){
		status = InitAster(LEN_PIX, Qneo.Lambdas, "");
		uart_printf("Aster init return status : %d\n", status);
	}
	else{
		//		status=SetupPred(Qneo.Lambdas,"PSS_NewRef_Trans.csv", "gAspec.csv", "PSS_NewRef_Algo.csv");
		status=SetupPred(Qneo.Lambdas,"Fdt_Qneo.txt", "gAspec.csv", "PSS_NewRef_Algo.csv");
		uart_printf("Atomm init return status : %d\n", status);
	}
	//////////////////////////////////////////////////////////////////////////////

	if(status!=0){
		char status_str[35];
		char status_arg[35];
		strncpy(status_arg, "Algo Init Status : ", 35);
		strncat(status_arg, "%d", 35);
		snprintf(status_str, 35, status_arg, status);
		Display_Status(status_str, LCD_COLOR_BLACK);
		while(1);
	}

	delay_ms(1000);

	

/*	if(status==0){
		asm("NOP");
		start_us_count(&micro);
		asm("NOP");
		Program_Planck_flash();
		asm("NOP");
		stop_us_count(&micro);
		asm("NOP");
	}

	uart_printf("Flash LogPlank program done in : %lu cycles\n", micro);
*/

/*
	FLOAT planck[LEN_PIX];
	int planck_T=500;

	asm("NOP");
	start_us_count(&micro);
	asm("NOP");
	get_LogPlanck(planck_T, planck);
	asm("NOP");
	stop_us_count(&micro);
	asm("NOP");

	uart_printf("Planck read in : %lu cycles\n", micro);
	delay_ms(1000);
	for (size_t i = 0; i < LEN_PIX; i++)
	{
		//uart_printf("Planck(%d)[%u] = %f\n",planck_T,i,planck[i]);
	}
	

	FIL file_in,file_out;
	FRESULT if_res,of_res,debug_res;
	char* token_aster;
	int Temp_aster=300,T_ref;
	FLOAT lh=-1;
	FLOAT spectra_aster[LEN_PIX];
	volatile int TXD[1024];
	
	uint32_t xd=0;
	int res=0;
	int sampleID=1;

	if_res=f_open(&file_in, "Trial_4-Phase_6-8ms.csv", FA_OPEN_EXISTING|FA_READ);


	of_res=f_open(&file_out, "Trial_4-Phase_6-8ms_result.csv", FA_OPEN_EXISTING|FA_READ);	
	


	uart_printf("input file open res : %d\n", if_res);
	uart_printf("output file open res : %d\n", of_res);

	FLOAT s_mean=0;

	uart_printf("Sample;Likelihood;Temp Algo;Temp file;LikeGMM;LikePCA\n");
	for (size_t i = 1; i <=100; i++){

		f_gets_debug(HUGE_BUFFER, HUGE_L, &file_in, &debug_res);
		if(debug_res){
			uart_printf("f_gets fail with result : %d\n", debug_res);
		}
		s_mean=0;
		token_aster=strtok(HUGE_BUFFER,";");
		for (size_t i = 0; i < LEN_PIX; i++)
		{
			token_aster=strtok(NULL, ";");
			spectra_aster[i]=strtof(token_aster, NULL);
			s_mean+=spectra_aster[i];
		}

		s_mean/=LEN_PIX;

		//uart_printf("Spectra Mean = %f\n", s_mean);

		//TXD[xd++]=strtol(HUGE_BUFFER, NULL, 10);

		f_gets(HUGE_BUFFER, HUGE_L, &file_out);

		T_ref=strtol(HUGE_BUFFER, NULL, 10);


		for (size_t j = 0; j<1; j++) {
				asm("NOP");
				res = Process(8,spectra_aster,&Temp_aster, &lh);
				asm("NOP");
				
				//uart_printf("Sample;Likelihood;Temp Algo;Temp file;LikeGMM;LikePCA\n");
				uart_printf("%d;%f;%d;%d;%f;%f\n",i, lh, Temp_aster, T_ref, maxL1, maxL2);
				//uart_printf("Sample %d : result :%d\nLH = %f - T = %d vs %d\n", i, res, lh, Temp_aster, T_ref);
				//uart_printf("LikeGMM : %f - LikePCA : %f\n", maxL1, maxL2);
		}
		//uart_printf("\n");

		//delay_ms(100);
	}
	
	f_close(&file_in);
	f_close(&file_out);

	uart_printf("Done \n");
	while (1);
*/
	Gflg=0;

	Setup_mesure(&Qneo);
	
	start_us_count(&micro);
	led_ticks=ticks+1000;
	while(ticks<led_ticks);
	stop_us_count(&micro);

	uart_printf("1sec = %d cycles\n", micro);
	
	setup_Servo();

	uart_printf("Servo Setup\n");
/*
	timer_set_oc_value(TIM8, TIM_OC1, (20310*(40))/2000);
	delay_ms(5000);
	timer_set_oc_value(TIM8, TIM_OC1, (20310*(230))/2000);
	while(1);
*/

	uart_timeout=UART_TIMEOUT_POST;
	spectro_status[0]=SPECTRO_IDLE;
	uint16_t data_dac=0;
/* 
	delay_ms(1000);
	
	close_shutter(0);

	delay_ms(1000);

	open_shutter(0);
	 */

	
	Calibrate_Dark(Qneo.Ti,10);
	Qneo.Calib_T=*(FLOAT*) &Rx7_buffer[5];

	if(Qneo.Default){
		Display_Spectro(false);
        uart_printf("Spectro Not found in 1st Calib ! MCU is halted\n");
		Display_Status(menu_field[NOSP_CL_FIELD], LCD_COLOR_RED);
		while(1);
	}

	Reload_Params();

	Display_Spectro(true);

    for(;;){
		int k=0;

		
		spectro_thread();	//main thread 
		
		//blink every 1 sec 
		if(ticks-led_ticks>1000){	//used to visually check if the code is not frozen somewhere (trap loop, exception, periph failure ...)
			test_ticks=ticks;
			gpio_toggle(GPIOB, GREEN_LED);
			led_ticks=ticks;
			test_ticks=ticks-test_ticks;

			if(menu_status==MENU_IDLE){
				char status_str[35];
				char status_arg[35];
				strncpy(status_arg, menu_field[RUN_FIELD], 35);
				strncat(status_arg, "%.1fHz - @%dms", 35);
				snprintf(status_str, 35, status_arg, 1000./Qneo.MPeriod_ms, Qneo.Ti_ms-1);
				Display_Status(status_str, LCD_COLOR_BLACK);
			}
		}
	}
	return 0;
}

