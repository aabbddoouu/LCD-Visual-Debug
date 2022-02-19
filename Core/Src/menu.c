#include<menu.h>
#include<buttons.h>
#include<spectro.h>
#include<parameters.h>
#include "ff.h"
#include "lcd.h"

extern gpiopin return_button;
extern volatile uint64_t ticks;
extern volatile uint64_t button_ticks;
extern int button_status;
extern int spectro_status[3];
extern FATFS FatFs;
extern FIL fil;
extern FRESULT fresult;
extern int16_t lang_count;
extern int16_t param_count;
extern parameter param_list[MAX_PARAMS];
extern lang language[MAX_LANG];
extern Display DisplayLCD;
extern volatile Spectro Qneo;
//extern char SD_buffer[LINE_SD_BUFFER];

int menu_status=MENU_IDLE;	//contains the current menu state
uint64_t temp_ticks=0; //timing stuff for temperature update
uint64_t graph_ticks=0;
uint64_t saved_ticks=0;//timing stuff for "saved" screen duration
bool first = true;	//used to detect if it's the 1st time we're entering the MENU_FIRST state 
					//so we don't send the line over and over again (see corresponding case)
bool new_card=true;//did the user change the sd card ?
bool card_mounted=false;
//static char* setting[] = {"Language", "Parameters"};
bool setting_params = true;
char menu_field[MENU_FIELDS][Line_Max_Char]; //stores menu fields text
//char param_field[MAX_PARAMS][20];

int16_t curr_lang=0;
int16_t curr_param=0; // not very safe but w/e (as long as we handle it well)
uint16_t desc_lines=0;
int16_t desc_chunck=0;
int16_t max_chunck=0;	//stores the max index of desc_pnt[]
uint8_t desc_pnt[DESC_length_M/10];//stores the pointers to the description lines
									//to show on screen
//////////////////////////////////////////////////////////////
uint32_t return_ticks=0;

/////////////////////////////////////////////////
static char* default_field[MENU_FIELDS]={
    "Running @",
    "Spectro Not found in Calib",
    "Spectro Found",
    "Wavelengths extracted",
    "Spectro Not found in setup",
    "Insert Card !",
    "Temperature",
	"GRAPH",
	"PARAMETERS",
    "LANGUAGE",
	"Parameter",
	"Description",
    "Value",
	"Press Enter to Confirm",
	"CANNOT SAVE : NO CARD",
    "SAVED !",
	"Language",
	"Booting up"
};

void config_fields(){
	for (size_t i = 0; i < MENU_FIELDS; i++)
	{
		strcpy(menu_field[i],default_field[i]);
	}
	
}


//dummy parameter that will host the selected parameter proprieties 
parameter_menu dummyParam={
	(uint8_t)0,
	(int32_t)0,
	(int32_t)0,
	"00000000.00000000",
	0.0,
	"00000000.00000000",
	"00000000.00000000",
	0.0,
	0.0,
	(uint8_t)0,
	(uint8_t)0,
	(uint8_t)0
};

float temp1;	//temp var declaration. i like to allocate the memory beforehand 
int32_t temp_int=1;	//not sure if used



/**
 * @brief State Machine that handles menu's logic
 * 
 */
void menu_thread(){

	if(ticks-temp_ticks>=UPDATE_DELAY){ //every UPDATE_DELAY (ms)
		Update_Temperature(Qneo.Temperature);
		temp_ticks=ticks;

		//check what to do with screen
		if(DisplayLCD.TurnOFF_event && DisplayLCD.isON && (ticks-button_ticks)>LCD_SLEEP_PERIOD){
			DisplayLCD.TurnOFF_event=false;
			LCD_DisplayOff();
			
		}
		if(DisplayLCD.TurnON_event){
			DisplayLCD.TurnON_event=false;
			LCD_DisplayOn();
		}
	}

	switch (menu_status)
	{
	case MENU_IDLE:
	//First time we enter the thread 
		if(new_card && card_mounted){
		
			load_params_SD();
			scan_files(LANG_DIR);
			curr_lang=0;
			while (strcmp(language[curr_lang].name, DEFAULT_LANG))
			{
				curr_lang++;
			}
			
			load_language_pack();
			Request_Reload();
			new_card=false;
			first=true;
			saved_ticks=ticks;
		}

		if(ticks-graph_ticks>=UPDATE_DELAY && DisplayLCD.Page==PAGE_GRAPH){
            Update_Graph(Qneo.Temperature);
            graph_ticks=ticks;
        }

		if(return_ticks > 1000000){
			LCD_DisplayOff();
		}

		switch (button_status)
		{
		case NO_BUTTON_EVENT:
			if(gpio_get(return_button.gpio, return_button.pin))	
				return_ticks++;
			else
				return_ticks=0;

			break;

		case RETURN_BUTTON_EVENT:
			DisplayLCD.Selected_Page=PAGE_GRAPH;
			DisplayLCD.Page=PAGE_GRAPH;
			Select_Page();
			break;

		case ENTER_BUTTON_EVENT:
			if(DisplayLCD.Selected_Page!=DisplayLCD.Page){
				DisplayLCD.Page_transition=true;
				DisplayLCD.Page=DisplayLCD.Selected_Page;
				
				}

				switch (DisplayLCD.Page)
				{
				case PAGE_LANG:
					Select_Lang(curr_lang);
					menu_status=MENU_LANG;
					break;
				
				case PAGE_PARAM:
					setup_desc(curr_param);
					Select_Param(curr_param);
					menu_status=MENU_PARAM;
					break;
				
				default:
					break;
			}
			break;

		case RIGHT_BUTTON_EVENT:
			if(++DisplayLCD.Selected_Page > PAGE_MAX){
				DisplayLCD.Selected_Page=0;
			}
			Select_Page();
			break;
		case LEFT_BUTTON_EVENT:
			if(--DisplayLCD.Selected_Page < 0){
				DisplayLCD.Selected_Page=PAGE_MAX;
			}
			Select_Page();
			break;
		
		default:
			
			break;
		}

		//after each menu case reset the button status
		button_status=NO_BUTTON_EVENT; 
		break;
	case MENU_NEWSD:
		//called when we want to exit the menu
		first=true;
		button_status=NO_BUTTON_EVENT;
		temp_ticks=0;
		spectro_status[2]=SPECTRO_IDLE;
		menu_status=MENU_IDLE;
		DisplayLCD.Page_transition=true;
		DisplayLCD.Selected_Page=PAGE_GRAPH;
		DisplayLCD.Page=PAGE_GRAPH;
		Select_Page();
		//unmount_SD();
		break;
		
	
	case MENU_LANG:
	//choose display lang
		switch (button_status)
		{
		case NO_BUTTON_EVENT:
			//waiting 
			break;
		case ENTER_BUTTON_EVENT:
			//first=true;
			if (card_mounted){
			/* function to load language pack */
				load_language_pack();
				menu_status=MENU_IDLE;
				DisplayLCD.Page_transition=true;
				Select_Page();
			}
			else{
				menu_status=MENU_IDLE;
				DisplayLCD.Page_transition=true;
				Select_Page();
			}
			
			break;
		case RETURN_BUTTON_EVENT:
			//first=true;
			menu_status=MENU_IDLE;
			DisplayLCD.Page_transition=true;
			Select_Page();
			
			break;
		//using left/right to naviagte thru the param
		case RIGHT_BUTTON_EVENT:
			if(++curr_lang>=lang_count) {curr_lang=0; }
			Select_Lang(curr_lang);
			break;
		case LEFT_BUTTON_EVENT:
			if(--curr_lang<0) {curr_lang=lang_count-1; }
			Select_Lang(curr_lang);
			break;
		
		default:
			
			break;
		}
		button_status=NO_BUTTON_EVENT;
		break;

	case MENU_PARAM:
	//choose param
		switch (button_status)
		{
		case NO_BUTTON_EVENT:

			break;
		case ENTER_BUTTON_EVENT:
			//copy param content into dummy param 
			dummyParam.ID=param_list[curr_param].ID;
			
			dummyParam.integer_digits=MAX_DIGITS-param_list[curr_param].integer_digits;
			dummyParam.floating_digits=(param_list[curr_param].floating_digits==0?
										MAX_DIGITS-1 : param_list[curr_param].floating_digits+MAX_DIGITS);
			dummyParam.cursor_pos=dummyParam.integer_digits;

			strcpy(dummyParam.char_maxvalue,param_list[curr_param].maxvalue);
			strcpy(dummyParam.char_minvalue,param_list[curr_param].minvalue);
			//dummyParam.max_value=strtof(dummyParam.char_maxvalue);
			sscanf(dummyParam.char_maxvalue,"%f",&(dummyParam.max_value));
			//dummyParam.min_value=strtof(dummyParam.char_minvalue);
			sscanf(dummyParam.char_minvalue,"%f",&(dummyParam.min_value));

		
			dummyParam.int_value=(int32_t) param_list[curr_param].value;

			temp1=param_list[curr_param].value - (float)dummyParam.int_value;

			temp1*=powl(10,MAX_DIGITS);
			dummyParam.float_value=temp1;

			sprintf((char *)dummyParam.char_value,"%08d.%08d",dummyParam.int_value,dummyParam.float_value);


			DisplayLCD.Page_transition=true;
			Config_Param();
			//write_update_oled_menu(&dummyParam,0,20);

			menu_status=MENU_CONFIG;
			break;
		case RETURN_BUTTON_EVENT:
			//first=true;
			menu_status=MENU_IDLE;
			DisplayLCD.Page_transition=true;
			Select_Page();
			break;

		case RIGHT_BUTTON_EVENT:
			if(++curr_param>=param_count) {curr_param=0; }
			//uart_printf("curr param %d\n",curr_param);
			setup_desc(curr_param);
			Select_Param(curr_param);
			
			break;
		case LEFT_BUTTON_EVENT:
			if(--curr_param<0) {curr_param=param_count-1; }
			//uart_printf("curr param %d\n",curr_param);
			setup_desc(curr_param);
			Select_Param(curr_param);

			break;
		
		default:
			break;
		}
		button_status=NO_BUTTON_EVENT;
		break;

	case MENU_CONFIG:
	//configuring selected param 
		/**
		 *	integer digits
		 *    v
		 *  XXXXX|X|XX.XXXXXXXX
		 * cursor-^	       ^
		 * 		      decimal digits
		 */

		switch (button_status)
		{
		case NO_BUTTON_EVENT:
			//waiting 
			break;
		case ENTER_BUTTON_EVENT:
			menu_status=MENU_SAVE;
			Save_Param();
			break;
		case RETURN_BUTTON_EVENT:
			DisplayLCD.Page_transition=true;
			Select_Param(curr_param);
			menu_status=MENU_PARAM;
			break;

		case UP_BUTTON_EVENT:
			if(dummyParam.char_value[dummyParam.cursor_pos]++=='9'){	//look at how much space and computing we saved !!!!!!!!!!
				dummyParam.char_value[dummyParam.cursor_pos]='0';
			}
			//bound check here
			bound_check(&dummyParam);
			Config_Param();
			break;
		case DOWN_BUTTON_EVENT:
			if(dummyParam.char_value[dummyParam.cursor_pos]--=='0'){
				dummyParam.char_value[dummyParam.cursor_pos]='9';
			}
			//bound check here
			bound_check(&dummyParam);
			Config_Param();
			break;
		case RIGHT_BUTTON_EVENT:
			//cursor movement with bound check
			if(++(dummyParam.cursor_pos)>dummyParam.floating_digits){
				dummyParam.cursor_pos=dummyParam.floating_digits;
			}
			if(dummyParam.cursor_pos==MAX_DIGITS){
				dummyParam.cursor_pos++;
			}

			Config_Param();
			break;
		case LEFT_BUTTON_EVENT:
			if(dummyParam.cursor_pos==dummyParam.integer_digits){
				
			}
			else{
				dummyParam.cursor_pos--;
				if(dummyParam.cursor_pos==MAX_DIGITS){
					dummyParam.cursor_pos--;
				}
			}
			Config_Param();

			break;
		
		default:
			//write_update_oled("error !");
			break;
		}
		button_status=NO_BUTTON_EVENT;
		break;
	case MENU_SAVE:
	//Confirm saving change
			switch (button_status)
		{
		case NO_BUTTON_EVENT:
			//waiting
			break;
		case ENTER_BUTTON_EVENT:
			param_list[dummyParam.ID].value=dummyParam.value;
			first=true;
			//delay_ms(100);
			if (card_mounted){
				Save_Param_Succ();
				update_param_file(dummyParam.ID);//update file in SD card
				Request_Reload();
			}
			else{
				Cant_Save_Param();
			}			
			
			
			Select_Param(curr_param);
			menu_status=MENU_PARAM;



			break;
		case RETURN_BUTTON_EVENT:
			//show_param_menu(param_list[curr_param]);
			//write_update_oled("Aborted!");
			menu_status=MENU_CONFIG;
			Config_Param();
			break;
		
		default:
			//write_update_oled("error !");
			break;
		}
		button_status=NO_BUTTON_EVENT;
		break;
	
	default:
		break;
	}
}

/**
 * @brief increase cursor digit
 * 
 * @param param 
 */
void increase_digit(parameter_menu* param){
	if(param->char_value[param->cursor_pos]++=='9'){	//look at how much space and computing we saved !!!!!!!!!!
		param->char_value[param->cursor_pos]='0';
	}
}

/**
 * @brief decrease cursor digit
 * 
 * @param param 
 */
void decrease_digit(parameter_menu* param){
	if(param->char_value[param->cursor_pos]--=='0'){	//look at how much space and computing we saved !!!!!!!!!!
		param->char_value[param->cursor_pos]='9';
	}
}

/**
 * @brief currently not used
 * 
 * @param param 
 * @return float 
 */
float char2float(parameter_menu* param){
	
	float x=0.0;
	
	/*		DONT KNOW WHY THIS DOESNT WORK : didnt want to use sscanf (too high level)
	size_t i=1;
	int xD=0;
	for (; i <= MAX_DIGITS; i++)
	{
		xD=param->char_value[i-1]-'0';
		x+= (float)xD*powf(10,MAX_DIGITS-i);
	}
	//i==9;
	for (; i <= 2*MAX_DIGITS; i++)
	{
		xD=param->char_value[i-1]-'0';
		x+= (float)xD*powf(10,MAX_DIGITS-i);
	}
	*/
	sscanf(param->char_value,"%f",&x);
	return x;
	
}

/**
 * @brief 
 * 
 * @param param 
 */
void bound_check(parameter_menu* param){
	sscanf(param->char_value,"%f",&(param->value));
	
	if(param->max_value<param->value){
		param->value=param->max_value;
		//update_param_charValue(param);
		strcpy(param->char_value,param->char_maxvalue);
	}

	if(param->min_value>param->value){
		param->value=param->min_value;
		//update_param_charValue(param);
		strcpy(param->char_value,param->char_minvalue);
	}
}

/**
 * @brief Updates the param's char_value with its current value
 * 
 * @param param 
 */
void update_param_charValue(parameter_menu* param){
	
	param->int_value=(int32_t) param->value;

	temp1=param->value - (float)param->int_value;
	temp1*=powl(10,MAX_DIGITS);
	param->float_value=fabs(temp1); //incase of negative numbers

	sprintf((char *)param->char_value,"%08d.%08d",param->int_value,param->float_value);
}


char* get_menu_field(int i){
#ifdef USE_ASCII
	return menu_field[i];
#endif
#ifdef USE_UNICODE

#endif
}


char* get_param_name(int id){
#ifdef USE_ASCII
	return param_list[id].name;
#endif
#ifdef USE_UNICODE
	//use some conversion
#endif
}


/**
 * @brief assign max_chunk and desc_pnt : decompose the desc str into chunks depending on the screen resolution
 * 
 * @param id 
 */
void setup_desc(int16_t id){
#ifdef USE_ASCII
	size_t length = strlen(param_list[id].description);
	size_t max_lines=length/Char_per_line+2;
	desc_pnt[0]=0;
	for (size_t i = 0; ; i++)
	{
		
		desc_pnt[i+1]=desc_pnt[i]+Char_per_line;
		if(desc_pnt[i+1]>length-1)	{
			desc_pnt[i+1]=length;
			max_chunck=i;
			break;
		}
		if(char_is_delimiter(&param_list[id].description[desc_pnt[i+1]])){
			desc_pnt[i+1]++;
			//continue;
		}
		else{
			do
			{
				desc_pnt[i+1]--;
			} while (!char_is_delimiter(&param_list[id].description[desc_pnt[i+1]]));
			desc_pnt[i+1]++;
			
		}

		

	}
	

#endif
#ifdef USE_UNICODE
	//use some conversion
#endif
}

bool char_is_delimiter(char* c){
#ifdef USE_ASCII
	if(*c==' ' ||*c=='.' ||*c==',' ||*c==':' ||*c=='-' ||*c==';' ){
		return true;
	}
	return false;
#endif

#ifdef USE_UNICODE
	//use some conversion
#endif
}
