/**
 * ov2640.h
 *
 * 	Created on: 23.02.2019
 *	Modified on:  23.02.2021
 *
 *	Copyright 2021 SimpleMethod
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of
 *this software and associated documentation files (the "Software"), to deal in
 *the Software without restriction, including without limitation the rights to
 *use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *of the Software, and to permit persons to whom the Software is furnished to do
 *so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all
 *copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *THE SOFTWARE.
 ******************************************************************************
 */

#pragma once

#include "main.h"
#include <libopencm3/stm32/dcmi.h>

#define RES160x120      160*120*2
#define RES320x240      240*320*2 //*2 since table is in uint8 and pixel = RGB565
#define RES640x480      640*480*2
#define RES800x600      800*600*2
#define RES1024x768     1024*768*2
#define RES1280x960     1280*960*2

#define CAM_160x120     0
#define CAM_320x240     1
#define CAM_480x272     2
#define CAM_640x480     3

#define READ_I2C        true
#define WRITE_I2C       false


#define CAM_CAPTURE1FRAME(Cam)  Cam.isCapturing=true; Cam.CaptureDone=false


/** 
  * @brief  OV2640 ID  
  */  
#define  OV2640_ID    0x26
#define  CAM_ADDR	  0x60        //0x05
/** 
  * @brief  OV2640 Registers  
  */
/* OV2640 Registers definition when DSP bank selected (0xFF = 0x00) */
#define OV2640_DSP_R_BYPASS             0x05
#define OV2640_DSP_Qs                   0x44
#define OV2640_DSP_CTRL                 0x50
#define OV2640_DSP_HSIZE1               0x51
#define OV2640_DSP_VSIZE1               0x52
#define OV2640_DSP_XOFFL                0x53
#define OV2640_DSP_YOFFL                0x54
#define OV2640_DSP_VHYX                 0x55
#define OV2640_DSP_DPRP                 0x56
#define OV2640_DSP_TEST                 0x57
#define OV2640_DSP_ZMOW                 0x5A
#define OV2640_DSP_ZMOH                 0x5B
#define OV2640_DSP_ZMHH                 0x5C
#define OV2640_DSP_BPADDR               0x7C
#define OV2640_DSP_BPDATA               0x7D
#define OV2640_DSP_CTRL2                0x86
#define OV2640_DSP_CTRL3                0x87
#define OV2640_DSP_SIZEL                0x8C
#define OV2640_DSP_HSIZE2               0xC0
#define OV2640_DSP_VSIZE2               0xC1
#define OV2640_DSP_CTRL0                0xC2
#define OV2640_DSP_CTRL1                0xC3
#define OV2640_DSP_R_DVP_SP             0xD3
#define OV2640_DSP_IMAGE_MODE           0xDA
#define OV2640_DSP_RESET                0xE0
#define OV2640_DSP_MS_SP                0xF0
#define OV2640_DSP_SS_ID                0x7F
#define OV2640_DSP_SS_CTRL              0xF8
#define OV2640_DSP_MC_BIST              0xF9
#define OV2640_DSP_MC_AL                0xFA
#define OV2640_DSP_MC_AH                0xFB
#define OV2640_DSP_MC_D                 0xFC
#define OV2640_DSP_P_STATUS             0xFE
#define OV2640_DSP_RA_DLMT              0xFF

/* OV2640 Registers definition when sensor bank selected (0xFF = 0x01) */
#define OV2640_SENSOR_GAIN              0x00
#define OV2640_SENSOR_COM1              0x03
#define OV2640_SENSOR_REG04             0x04
#define OV2640_SENSOR_REG08             0x08
#define OV2640_SENSOR_COM2              0x09
#define OV2640_SENSOR_PIDH              0x0A
#define OV2640_SENSOR_PIDL              0x0B
#define OV2640_SENSOR_COM3              0x0C
#define OV2640_SENSOR_COM4              0x0D
#define OV2640_SENSOR_AEC               0x10
#define OV2640_SENSOR_CLKRC             0x11
#define OV2640_SENSOR_COM7              0x12
#define OV2640_SENSOR_COM8              0x13
#define OV2640_SENSOR_COM9              0x14
#define OV2640_SENSOR_COM10             0x15
#define OV2640_SENSOR_HREFST            0x17
#define OV2640_SENSOR_HREFEND           0x18
#define OV2640_SENSOR_VSTART            0x19
#define OV2640_SENSOR_VEND              0x1A
#define OV2640_SENSOR_MIDH              0x1C
#define OV2640_SENSOR_MIDL              0x1D
#define OV2640_SENSOR_AEW               0x24
#define OV2640_SENSOR_AEB               0x25
#define OV2640_SENSOR_W                 0x26
#define OV2640_SENSOR_REG2A             0x2A
#define OV2640_SENSOR_FRARL             0x2B
#define OV2640_SENSOR_ADDVSL            0x2D
#define OV2640_SENSOR_ADDVHS            0x2E
#define OV2640_SENSOR_YAVG              0x2F
#define OV2640_SENSOR_REG32             0x32
#define OV2640_SENSOR_ARCOM2            0x34
#define OV2640_SENSOR_REG45             0x45
#define OV2640_SENSOR_FLL               0x46
#define OV2640_SENSOR_FLH               0x47
#define OV2640_SENSOR_COM19             0x48
#define OV2640_SENSOR_ZOOMS             0x49
#define OV2640_SENSOR_COM22             0x4B
#define OV2640_SENSOR_COM25             0x4E
#define OV2640_SENSOR_BD50              0x4F
#define OV2640_SENSOR_BD60              0x50
#define OV2640_SENSOR_REG5D             0x5D
#define OV2640_SENSOR_REG5E             0x5E
#define OV2640_SENSOR_REG5F             0x5F
#define OV2640_SENSOR_REG60             0x60
#define OV2640_SENSOR_HISTO_LOW         0x61
#define OV2640_SENSOR_HISTO_HIGH        0x62 

/** 
 * @brief  OV2640 Features Parameters  
 */
#define OV2640_BRIGHTNESS_LEVEL0        0x40   /* Brightness level -2         */
#define OV2640_BRIGHTNESS_LEVEL1        0x30   /* Brightness level -1         */
#define OV2640_BRIGHTNESS_LEVEL2        0x20   /* Brightness level 0          */
#define OV2640_BRIGHTNESS_LEVEL3        0x10   /* Brightness level +1         */
#define OV2640_BRIGHTNESS_LEVEL4        0x00   /* Brightness level +2         */

#define OV2640_BLACK_WHITE_BW           0x18   /* Black and white effect      */
#define OV2640_BLACK_WHITE_NEGATIVE     0x40   /* Negative effect             */
#define OV2640_BLACK_WHITE_BW_NEGATIVE  0x58   /* BW and Negative effect      */
#define OV2640_BLACK_WHITE_NORMAL       0x00   /* Normal effect               */

#define OV2640_CONTRAST_LEVEL0          0x3418 /* Contrast level -2           */
#define OV2640_CONTRAST_LEVEL1          0x2A1C /* Contrast level -2           */
#define OV2640_CONTRAST_LEVEL2          0x2020 /* Contrast level -2           */
#define OV2640_CONTRAST_LEVEL3          0x1624 /* Contrast level -2           */
#define OV2640_CONTRAST_LEVEL4          0x0C28 /* Contrast level -2           */

#define OV2640_COLOR_EFFECT_ANTIQUE     0xA640 /* Antique effect              */
#define OV2640_COLOR_EFFECT_BLUE        0x40A0 /* Blue effect                 */
#define OV2640_COLOR_EFFECT_GREEN       0x4040 /* Green effect                */
#define OV2640_COLOR_EFFECT_RED         0xC040 /* Red effect                  */   
/**
  * @}
  */
  
/** @defgroup OV2640_Exported_Functions
  * @{
  */ 
void OV2640_Init(uint32_t);
void cringe_function();

typedef struct 
{
    bool        isCapturing;
    bool        CaptureDone;
    uint8_t     RGB565[RES320x240]
}ov2640;


