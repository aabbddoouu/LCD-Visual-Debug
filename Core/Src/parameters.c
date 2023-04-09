#include <fsm.h>
//fatfs lib
#include "diskio.h"



extern uint16_t curr_lang;
extern char menu_field[MENU_FIELDS][Line_Max_Char];

#define PARAM_FILENAME "parameters.csv"
#define COPY_FILENAME	"parameters_copy.csv"
#define LINE_STR_FORMAT "%s;%f;%s;%s;%d;%d\n"

int16_t param_count=0;
int16_t lang_count=0;

FATFS FatFs;
FIL fil,fil_copy;
FRESULT fresult;

parameter param_list[MAX_PARAMS];


char SD_buffer[LINE_SD_BUFFER];

/*
parameter param_temp={
    (uint8_t) 0,
    (const char*) "XXXXXXXXXXXXXXXXXXX",
    0.f,
    "00000000.00000000",
    "00000000.00000000",
    (uint8_t) 0,
    (uint8_t) 0 
};
*/

extern volatile uint64_t ticks;
volatile uint64_t sd_ticks;
uint64_t sprint_ticks;

/**
 * @brief 
 * 
 */
void load_params_SD(){
	param_count=0;
	fresult=f_open(&fil, PARAM_FILENAME, FA_OPEN_EXISTING | FA_READ);
	f_rewind(&fil); //just be be extra sure

	uart_printf("Param file open result %d\n",fresult);											

	if (fresult == FR_OK) {
		//discard 1st line
		float temp_float;
		float max_value;
		float min_value;
		
		f_gets(SD_buffer,LINE_SD_BUFFER,&fil);
		//memset(SD_buffer,0,LINE_SD_BUFFER);
		asm("nop");
		while (f_gets(SD_buffer,LINE_SD_BUFFER,&fil) != 0 && param_count<MAX_PARAMS) {	
			/*			
			sscanf(SD_buffer,"%s;%f;%s;%s;%d;%d\n",&param_list[param_count].name, &param_list[param_count].value,
					&param_list[param_count].maxvalue, &param_list[param_count].minvalue,
					&param_list[param_count].integer_digits, &param_list[param_count].floating_digits);
					*/
			char* token;
			token=strtok(SD_buffer,";");
			strcpy(param_list[param_count].name,token);
			//uart_printf("%s\n",token);
			//delay_ms(10);
			//uart_printf("%s\n",param_list[param_count].name);
			token = strtok(NULL, ";");
			param_list[param_count].value=strtof(token,NULL);
			//sscanf(token,"%f",&param_list[param_count].value);
			//param_list[param_count].value=XD;
			//uart_printf("%d.%04d\n", (int)param_list[param_count].value, ((int)(param_list[param_count].value * 10000.0 + 0.5))%10000);
			//delay_ms(10);

			////////////////////////////////////////////////////////////////////////////////////////////
			token = strtok(NULL, ";");
			max_value=strtof(token,NULL);
			//sscanf(token,"%f",&max_value);
			temp_float=max_value-(int32_t)max_value;
			temp_float*=powl(10,MAX_DIGITS);
			temp_float=fabs(temp_float);
			sprintf(param_list[param_count].maxvalue,"%08d.%08d",(int32_t)max_value,(int32_t)temp_float);
			//uart_printf("%s\n",param_list[param_count].maxvalue);
			//delay_ms(10);
			token = strtok(NULL, ";");
			min_value=strtof(token,NULL);
			//sscanf(token,"%f",&min_value);
			temp_float=min_value-(int32_t)min_value;
			temp_float*=powl(10,MAX_DIGITS);
			sprintf(param_list[param_count].minvalue,"%08d.%08d",(int32_t)min_value,(int32_t)temp_float);
			//uart_printf("%s\n",token);
			//delay_ms(10);
			//////////////////////////////////////////////////////////////////////////////////////////////
			token = strtok(NULL, ";");
			param_list[param_count].integer_digits=strtoul(token,NULL,10);
			//uart_printf("%s\n",token);
			//delay_ms(10);
			token = strtok(NULL, ";"); 
			param_list[param_count].floating_digits=strtoul(token,NULL,10);
			//uart_printf("%s\n",token);
			//delay_ms(10);
			param_list[param_count].ID=param_count;
			//token = strtok(NULL, ";");
			//memset(SD_buffer,0,LINE_SD_BUFFER);
			
			//to the next param !
			param_count++;
		}
		if (SD_buffer!= NULL && param_count==MAX_PARAMS) uart_printf("Max param count reached !\n");
		f_close(&fil);
		
	}
	
}

static volatile uint32_t micros=0;

/**
 * @brief 
must be done after each confirmed change.
another way is to pass the param_list as an arg
to let the user decide when to save changes to the SD card
 * 
 * @param ID 
 */
void update_param_file(uint8_t ID){ 

//Section 1 : Open files : 17ms @27MHz
//////////////////////////////////////////////////////
	sd_ticks=ticks;	
	fresult=f_open(&fil, PARAM_FILENAME, FA_OPEN_EXISTING | FA_READ | FA_WRITE);
	f_rewind(&fil); //just be be extra sure
	
	//uart_printf("Param file open result %d\n",fresult);
	//delay_ms(10);
	fresult=f_open(&fil_copy, COPY_FILENAME, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
	//uart_printf("Param copy file open result %d\n",fresult);
	//delay_ms(10);
	//copy from line 1 (1st line being line with header text) to line ($ID-1)
//////////////////////////////////////////////////////

//Section 2 : R/W files : 19ms @27MHz
//////////////////////////////////////////////////////
	//sd_ticks=ticks-sd_ticks;
	//sd_ticks=ticks;
	uint8_t i = 0;
	for (; i <= ID; i++)
	{
		f_gets(SD_buffer,LINE_SD_BUFFER,&fil);
		f_puts(SD_buffer,&fil_copy);
	}
	
	//write new param line
	f_gets(SD_buffer,LINE_SD_BUFFER,&fil);
	micros=0;

	start_us_count(&micros);
	sprintf(SD_buffer,LINE_STR_FORMAT,param_list[ID].name, param_list[ID].value,
					param_list[ID].maxvalue, param_list[ID].minvalue,
					param_list[ID].integer_digits, param_list[ID].floating_digits);

		//delay_ms(100);
	stop_us_count(&micros);	
	asm("nop");
	
	f_puts(SD_buffer,&fil_copy);
	//copy the rest of the lines
	for (i=ID+1; i < param_count; i++)
	{
		f_gets(SD_buffer,LINE_SD_BUFFER,&fil);
		f_puts(SD_buffer,&fil_copy);
	}
	//remove original file
	f_close(&fil);
	f_close(&fil_copy);

	//sd_ticks=ticks-sd_ticks;
	//sd_ticks=ticks;
//////////////////////////////////////////////////////

//Section 3 : Delete & Rename : 28 ms @27MHz WTF ??
//////////////////////////////////////////////////////
		
	fresult=f_unlink(PARAM_FILENAME);
	//uart_printf("Param file delete result %d\n",fresult);
	//rename copied file
	//f_close(&fil_copy);
	
	fresult=f_rename(COPY_FILENAME,PARAM_FILENAME);
	//uart_printf("Param copy file rename result %d\n",fresult);
//////////////////////////////////////////////////////
	sd_ticks=ticks-sd_ticks;
	
	asm("nop");
	uart_printf("Time made to save file : %d ms\n",(uint32_t) sd_ticks);
	uart_printf("Sprintf cycles : %d = %d us\n",(uint32_t) micros, (uint32_t)micros/108);
	//delay_ms(10);
	//uart_printf("Time made to format line file : %d ms\n",sprint_ticks);

}

/**
 * @brief Scan lang files from SD
 * 
 * @param path 
 * @return FRESULT 
 */
FRESULT scan_files (char* path){
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;
	
	lang_count=0;
    res = f_opendir(&dir, path);                       /* Open the directory */
    //uart_printf("Open Dir result : %d\n",res);
	if (res == FR_OK) {
		lang_count=0;
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                i = strlen(path);
                sprintf(&path[i], "/%s", fno.fname);
                res = scan_files(path);                    /* Enter the directory */
                if (res != FR_OK) break;
                path[i] = 0;
            } else {                                       /* It is a file. */
                char* token;
				token=strtok(fno.fname,".");
				//language[lang_count].ID=lang_count;
				//strcpy(language[lang_count].name,token);
				lang_count++;
				//uart_printf("%s\n", language[lang_count++].name);
				//delay_ms(100);

			}	
        }
        f_closedir(&dir);
    }

    return res;
}