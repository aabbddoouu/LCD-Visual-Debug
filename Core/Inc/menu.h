#pragma once

#include <main.h>


//Menu States
#define MENU_NEWSD       -1  
#define MENU_IDLE        0
#define MENU_PARAM       1
#define MENU_SETTINGS    2
#define MENU_LANG        3
#define MENU_DESC        4
#define MENU_FIRST      97
#define MENU_CONFIG     98
#define MENU_SAVE       99

//Temperature update rate
#define UPDATE_DELAY    1000U

//Max digits for parameters Floating & Integer part
#define MAX_DIGITS      8U

//Max Menu fields to translate 
#define MENU_FIELDS     30

#define RUN_FIELD           0
#define NOSP_ST_FIELD       4
#define SP_FOUND_FIELD      2
#define WL_EXT_FIELD        3
#define NOSP_CL_FIELD       1
#define INSERTSD_FIELD      5
#define Temp_FIELD          6
#define GRAPH_FIELD         7
#define PARAMS_FIELD        8
#define LANGS_FIELD         9
#define Param_FIELD         10
#define Desc_FIELD          11
#define Value_FIELD         12
#define CONFIRM_FIELD       13
#define NOSD_FIELD          14
#define SAVE_FIELD          15
#define Lang_FIELD          16
#define BOOT_FIELD          17


#define ERROR_FIELD     99

//Max charcters per line
#define Line_Max_Char   40
#define Lines_Screen    8
//Max charcters we can show in the screen
#define length_Screen   Line_Max_Char*Lines_Screen
#define Font_Used       Font16_11
//Max length of param description
#define DESC_length_M   256
#define Char_per_line   (ILI9341_LCD_PIXEL_WIDTH-20)/11
//Upper bound for lines needed to show description 

#define DEFAULT_LANG    "english"

/**
 * @brief 
 * 
 */
typedef struct parameter
{
    uint8_t     ID;
    char        name[20];
    char        description[DESC_length_M];
    float       value;
    char        maxvalue[MAX_DIGITS*2+2];
    char        minvalue[MAX_DIGITS*2+2];
    uint8_t     integer_digits;
    uint8_t     floating_digits;
    
}parameter;

typedef struct lang
{
    uint8_t     ID;
    char        name[20];
    
}lang;

/**
 * @brief 
 * 
 */
typedef struct parameter_menu
{
    uint8_t     ID;
    int32_t     int_value; //int part value
    int32_t     float_value; //fraction part value
    char        char_value[MAX_DIGITS*2+2]; //+1 for '.' +1 for EOF
    float       value; 
    char        char_maxvalue[MAX_DIGITS*2+2];
    char        char_minvalue[MAX_DIGITS*2+2];
    float       max_value;
    float       min_value;
    uint8_t     integer_digits; 
    uint8_t     floating_digits;
    uint8_t     cursor_pos; //cursor position while editing 
}parameter_menu;






void show_param(parameter_menu* param);
void show_temperature(float T);
void menu_thread();
void show_param_menu();
void setup_params();
void show_language_menu();
void show_setting_menu(int i);
void config_fields();

void update_param_charValue(parameter_menu* param);
void bound_check(parameter_menu* param);
float char2float(parameter_menu* param);
void decrease_digit(parameter_menu* param);
void increcrease_digit(parameter_menu* param);
void write_update_oled_menu(parameter_menu* param, uint16_t curX, uint16_t curY);


char* get_menu_field(int i);
char* get_param_name(int i);
void Show_param_desc(int16_t* id);
void setup_desc(int16_t id);
bool char_is_delimiter(char* c);
