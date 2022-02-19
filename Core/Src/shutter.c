#include <shutter.h>
#include <buttons.h>

#define MIN_ANGLE	50		//Not actual angle values
#define MAX_ANGLE	220

gpiopin shutter_sens1 = {GPIOE, GPIO3};
gpiopin shutter_sens2 = {GPIOF, GPIO8};


volatile int angle_deviation=0; 
volatile int angle_open=205;
volatile int angle_close=105;



void setup_Servo()
{
	rcc_periph_clock_enable(RCC_TIM8);

	/* Timer global mode:
	 * - No divider
	 * - Alignment edge
	 * - Direction up
	 */

	/* Reset TIM8 peripheral. */
	rcc_periph_reset_pulse(RST_TIM8);
	
	gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6);

	gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO6);
	
/*
	gpio_set(GPIOC, GPIO6);

*/
	gpio_set_af(GPIOC, GPIO_AF3, GPIO6);
	
	timer_set_mode(TIM8, TIM_CR1_CKD_CK_INT,
			   TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

	/* Reset prescaler value. */
	timer_set_prescaler(TIM8, 216);

	/* Reset repetition counter value. */
	timer_set_repetition_counter(TIM8, 0);
	
	/* Enable preload. */
	timer_enable_preload(TIM8);

	/* Continuous mode. */
	timer_continuous_mode(TIM8);

	timer_set_period(TIM8, 20310);

	timer_disable_break(TIM8);	

	/* -- OC1 configuration -- */

	/* Disable outputs. */
	timer_disable_oc_output(TIM8, TIM_OC1);
	timer_disable_oc_output(TIM8, TIM_OC1N);

	timer_disable_oc_clear(TIM8, TIM_OC1);
	timer_enable_oc_preload(TIM8, TIM_OC1);
	timer_set_oc_slow_mode(TIM8, TIM_OC1);
	timer_set_oc_mode(TIM8, TIM_OC1, TIM_OCM_PWM1);


	timer_set_oc_value(TIM8, TIM_OC1, (20310*55)/2000);

	/* Reenable outputs. */
	timer_enable_oc_output(TIM8, TIM_OC1);
	timer_enable_oc_output(TIM8, TIM_OC1N);

	//why is this parameter is hidden in the break register??
	//why should I modify the break reg if i'm not gonna use the break functionality ?????
	//Move MOE bit to CRx reg or make it 1 @ reset -_-
	timer_enable_break_main_output(TIM8);

	timer_generate_event(TIM8, TIM_EGR_UG);
	/* Counter enable. */
	timer_enable_counter(TIM8);
}

void close_shutter(){

    //Close the optical entrance using the servomotor
	bool shutter1=false, shutter2=false;
    int corr_angle=SERVO_CORR_STEP;
    close_shutter_LL(0);
    delay_ms(1000);

IFF:    asm("NOP");
    if(gpio_get(shutter_sens1.gpio,shutter_sens1.pin)){
        shutter1=false;
    }
    else{
        shutter1=true;
    }

    if(gpio_get(shutter_sens2.gpio,shutter_sens2.pin)){
        shutter2=false;
    }
    else{
        shutter2=true;
    }

    if(!shutter2){
        
        close_shutter_LL(-corr_angle);
        delay_ms(100);
        goto IFF;
    }

    else if(!shutter1 && shutter2){

        close_shutter_LL(corr_angle);
        delay_ms(100);
        goto IFF;
    }

    else{
        //OK
    }

    angle_deviation=0;
}



void open_shutter(){

    //Open the optical entrance using the servomotor
	bool shutter1=false, shutter2=false;
    int corr_angle=SERVO_CORR_STEP;

    open_shutter_LL(0);
    delay_ms(1000);

IFFF:    asm("NOP");
    if(gpio_get(shutter_sens1.gpio,shutter_sens1.pin)){
        shutter1=false;
    }
    else{
        shutter1=true;
    }

    if(gpio_get(shutter_sens2.gpio,shutter_sens2.pin)){
        shutter2=false;
    }
    else{
        shutter2=true;
    }

    if(shutter1){
        
        open_shutter_LL(-corr_angle);
        delay_ms(100);
        goto IFFF;
    }

    else{
        //OK
    }

	angle_deviation=0;
}

void close_shutter_LL(int corr){
    angle_deviation=angle_close+corr;

	if(angle_deviation>MAX_ANGLE-MIN_ANGLE)	angle_deviation=MAX_ANGLE-MIN_ANGLE;
	else if(angle_deviation<0)	angle_deviation=0;
    
    angle_close = angle_deviation;
	timer_set_oc_value(TIM8, TIM_OC1, (20310*(MIN_ANGLE+angle_close))/2000);
}

void open_shutter_LL(int corr){
    angle_deviation=angle_open+corr;

	if(angle_deviation>MAX_ANGLE-MIN_ANGLE)	angle_deviation=MAX_ANGLE-MIN_ANGLE;
	else if(angle_deviation<0)	angle_deviation=0;
    angle_open = angle_deviation;
	timer_set_oc_value(TIM8, TIM_OC1, (20310*(angle_open))/2000);
}



