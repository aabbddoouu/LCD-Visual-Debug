/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_H
#define __LCD_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#include <main.h>
#include <lcd/ili9341/ili9341.h>
#include <stm32_adafruit_lcd.h>
#include "atomm_utils.h"

#define LCD_SLEEP_PERIOD    1000*60


#define OVERLAY_MIDDLE_Y1	106
#define OVERLAY_MIDDLE_Y2	299

#define GRAPH_PIXELS_XRANGE	210
#define GRAPH_PIXELS_YRANGE	180
#define GRAPH_PIXELS_XSTART	230
#define GRAPH_PIXELS_YSTART	289

#define PAGE_GRAPH  0
#define PAGE_PARAM  1
#define PAGE_LANG   2
#define PAGE_MAX    2





typedef struct lcd_fmc_address_st {
    
    uint8_t lcd_reg;
    uint8_t lcd_ram;
} lcd_fmc_address_t;


typedef struct
{
	uint32_t max_value;
	uint32_t min_value;
	uint32_t max_value_inSET;
	uint32_t min_value_inSET;
	uint32_t range;
	uint32_t Table[GRAPH_PIXELS_XRANGE]; //contain Y value in px ranging from 0 to 180 + GRAPH_PIXELS_YSTART
	uint32_t Values[GRAPH_PIXELS_XRANGE]; //contain raw values

	uint32_t index;
	
}Graph;


typedef struct
{
	sFONT*      Font_Default;
	sFONT*      Font_Temperature;
	uint16_t		Background_color;
	uint16_t		Text_color;
	uint16_t		Graph_color;
  uint16_t		OK_color;
	uint16_t		NOK_color;
  int16_t     Page;
  int16_t     Selected_Page;
  bool        Page_transition;
  bool        isON;
  bool        TurnOFF_event;
  bool        TurnON_event;
}Display;




#define SRAM_SECTOR1    ((uint32_t)(0xC0000000 | 0x00000000))

#define LCD             ((lcd_fmc_address_t*)SRAM_SECTOR1)


/* Link function for LCD peripheral (modified) */

//-----------------------------------------------------------------------------
void     LCD_Delay (uint32_t delay);
void     LCD_IO_Init(void);
void     LCD_IO_Bl_OnOff(uint8_t Bl);

void     LCD_IO_WriteCmd8(uint8_t Cmd);
void     LCD_IO_WriteCmd16(uint16_t Cmd);
void     LCD_IO_WriteData8(uint8_t Data);
void     LCD_IO_WriteData16(uint16_t Data);

void     LCD_IO_WriteCmd8DataFill16(uint8_t Cmd, uint16_t Data, uint32_t Size);
void     LCD_IO_WriteCmd8MultipleData8(uint8_t Cmd, uint8_t *pData, uint32_t Size);
void     LCD_IO_WriteCmd8MultipleData16(uint8_t Cmd, uint16_t *pData, uint32_t Size);
void     LCD_IO_WriteCmd16DataFill16(uint16_t Cmd, uint16_t Data, uint32_t Size);
void     LCD_IO_WriteCmd16MultipleData8(uint16_t Cmd, uint8_t *pData, uint32_t Size);
void     LCD_IO_WriteCmd16MultipleData16(uint16_t Cmd, uint16_t *pData, uint32_t Size);

void     LCD_IO_ReadCmd8MultipleData8(uint8_t Cmd, uint8_t *pData, uint32_t Size, uint32_t DummySize);
void     LCD_IO_ReadCmd8MultipleData16(uint8_t Cmd, uint16_t *pData, uint32_t Size, uint32_t DummySize);
void     LCD_IO_ReadCmd8MultipleData24to16(uint8_t Cmd, uint16_t *pData, uint32_t Size, uint32_t DummySize);
void     LCD_IO_ReadCmd16MultipleData8(uint16_t Cmd, uint8_t *pData, uint32_t Size, uint32_t DummySize);
void     LCD_IO_ReadCmd16MultipleData16(uint16_t Cmd, uint16_t *pData, uint32_t Size, uint32_t DummySize);
void     LCD_IO_ReadCmd16MultipleData24to16(uint16_t Cmd, uint16_t *pData, uint32_t Size, uint32_t DummySize);

/* User defined */

//-----------------------------------------------------------------------------
void Draw_Overlay();
void Update_Graph(FLOAT value);
void Draw_Graph_Axis();
void Scale_Graph(uint32_t max_v, uint32_t min_v);
void Display_Status(const char* str, uint16_t Color);
void Display_DAC1(bool status);
void Display_DAC2(bool status);
void Display_Card(bool status);
void Display_Spectro(bool status);
void Clear_Page();
void Select_Page();

void Draw_Select_Param();
void Select_Param(int16_t index);

void Draw_Config_Param();
void Config_Param();

void Draw_Select_Lang();
void Select_Lang(int16_t index);
void Cant_Load_Lang();

void Save_Param();
void Cant_Save_Param();
void Save_Param_Succ();

void Update_Temperature(FLOAT value);






/* The 16 bits values (color codes, bitmap) byte order
 * - 0: ne reverse order
 * - 1: reverse order
 *   note: Using reverse order is only recommended in 8 bit fsmc dma mode so that dma can be turned on. 
           In all other cases it is disadvantageous.
 */
#define  LCD_REVERSE16   0

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup LCD
  * @{
  */
 
/** @defgroup LCD_Exported_Types
  * @{
  */

/** @defgroup LCD_Driver_structure  LCD Driver structure
  * @{
  */
typedef struct
{
  void     (*Init)(void);
  uint16_t (*ReadID)(void);
  void     (*DisplayOn)(void);
  void     (*DisplayOff)(void);
  void     (*SetCursor)(uint16_t, uint16_t);
  void     (*WritePixel)(uint16_t, uint16_t, uint16_t);
  uint16_t (*ReadPixel)(uint16_t, uint16_t);
  
  void     (*SetDisplayWindow)(uint16_t, uint16_t, uint16_t, uint16_t);
  void     (*DrawHLine)(uint16_t, uint16_t, uint16_t, uint16_t);
  void     (*DrawVLine)(uint16_t, uint16_t, uint16_t, uint16_t);
  
  uint16_t (*GetLcdPixelWidth)(void);
  uint16_t (*GetLcdPixelHeight)(void);
  void     (*DrawBitmap)(uint16_t, uint16_t, uint8_t*);
  void     (*DrawRGBImage)(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t*);
  void     (*DrawRGBImage8)(uint16_t, uint16_t, uint16_t, uint16_t, uint8_t*);  
  void     (*FillRect)(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);
  void     (*ReadRGBImage)(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t*);
  void     (*Scroll)(int16_t, uint16_t, uint16_t);
}LCD_DrvTypeDef;    
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __LCD_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
