/**
 * ov2640.c
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

#include "ov2640.h"

static volatile uint8_t reg_read=0;  

/* Initialization sequence for 480x272 resolution */
volatile uint8_t OV2640_480x272[237][2]=
{
  {0xff, 0x00},  /* Device control register list Table 12 */
  {0x2c, 0xff},  /* Reserved                              */
  {0x2e, 0xdf},  /* Reserved                              */
  {0xff, 0x01},  /* Device control register list Table 13 */
  {0x3c, 0x32},  /* Reserved                              */
  {0x11, 0x00},  /* Clock Rate Control                    */
  {0x09, 0x02},  /* Common control 2                      */
  {0x04, 0xA8},  /* Mirror                                */
  {0x13, 0xe5},  /* Common control 8                      */
  {0x14, 0x48},  /* Common control 9                      */
  {0x2c, 0x0c},  /* Reserved                              */
  {0x33, 0x78},  /* Reserved                              */
  {0x3a, 0x33},  /* Reserved                              */
  {0x3b, 0xfB},  /* Reserved                              */
  {0x3e, 0x00},  /* Reserved                              */
  {0x43, 0x11},  /* Reserved                              */
  {0x16, 0x10},  /* Reserved                              */
  {0x4a, 0x81},  /* Reserved                              */
  {0x21, 0x99},  /* Reserved                              */
  {0x24, 0x40},  /* Luminance signal High range           */
  {0x25, 0x38},  /* Luminance signal low range            */
  {0x26, 0x82},  /*                                       */
  {0x5c, 0x00},  /* Reserved                              */
  {0x63, 0x00},  /* Reserved                              */
  {0x46, 0x3f},  /* Frame length adjustment               */
  {0x0c, 0x3c},  /* Common control 3                      */
  {0x61, 0x70},  /* Histogram algo low level              */
  {0x62, 0x80},  /* Histogram algo high level             */
  {0x7c, 0x05},  /* Reserved                              */
  {0x20, 0x80},  /* Reserved                              */
  {0x28, 0x30},  /* Reserved                              */
  {0x6c, 0x00},  /* Reserved                              */
  {0x6d, 0x80},  /* Reserved                              */
  {0x6e, 0x00},  /* Reserved                              */
  {0x70, 0x02},  /* Reserved                              */
  {0x71, 0x94},  /* Reserved                              */
  {0x73, 0xc1},  /* Reserved                              */
  {0x3d, 0x34},  /* Reserved                              */
  {0x5a, 0x57},  /* Reserved                              */
  {0x12, 0x00},  /* Common control 7                      */
  {0x11, 0x00},  /* Clock Rate Control                   2*/
  {0x17, 0x11},  /* Horiz window start MSB 8bits          */
  {0x18, 0x75},  /* Horiz window end MSB 8bits            */
  {0x19, 0x01},  /* Vert window line start MSB 8bits      */
  {0x1a, 0x97},  /* Vert window line end MSB 8bits        */
  {0x32, 0x36},
  {0x03, 0x0f},
  {0x37, 0x40},
  {0x4f, 0xbb},
  {0x50, 0x9c},
  {0x5a, 0x57},
  {0x6d, 0x80},
  {0x6d, 0x38},
  {0x39, 0x02},
  {0x35, 0x88},
  {0x22, 0x0a},
  {0x37, 0x40},
  {0x23, 0x00},
  {0x34, 0xa0},
  {0x36, 0x1a},
  {0x06, 0x02},
  {0x07, 0xc0},
  {0x0d, 0xb7},
  {0x0e, 0x01},
  {0x4c, 0x00},
  {0xff, 0x00},
  {0xe5, 0x7f},
  {0xf9, 0xc0},
  {0x41, 0x24},
  {0xe0, 0x14},
  {0x76, 0xff},
  {0x33, 0xa0},
  {0x42, 0x20},
  {0x43, 0x18},
  {0x4c, 0x00},
  {0x87, 0xd0},
  {0x88, 0x3f},
  {0xd7, 0x03},
  {0xd9, 0x10},
  {0xd3, 0x82},
  {0xc8, 0x08},
  {0xc9, 0x80},
  {0x7d, 0x00},
  {0x7c, 0x03},
  {0x7d, 0x48},
  {0x7c, 0x08},
  {0x7d, 0x20},
  {0x7d, 0x10},
  {0x7d, 0x0e},
  {0x90, 0x00},
  {0x91, 0x0e},
  {0x91, 0x1a},
  {0x91, 0x31},
  {0x91, 0x5a},
  {0x91, 0x69},
  {0x91, 0x75},
  {0x91, 0x7e},
  {0x91, 0x88},
  {0x91, 0x8f},
  {0x91, 0x96},
  {0x91, 0xa3},
  {0x91, 0xaf},
  {0x91, 0xc4},
  {0x91, 0xd7},
  {0x91, 0xe8},
  {0x91, 0x20},
  {0x92, 0x00},
  {0x93, 0x06},
  {0x93, 0xe3},
  {0x93, 0x02},
  {0x93, 0x02},
  {0x93, 0x00},
  {0x93, 0x04},
  {0x93, 0x00},
  {0x93, 0x03},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x96, 0x00},
  {0x97, 0x08},
  {0x97, 0x19},
  {0x97, 0x02},
  {0x97, 0x0c},
  {0x97, 0x24},
  {0x97, 0x30},
  {0x97, 0x28},
  {0x97, 0x26},
  {0x97, 0x02},
  {0x97, 0x98},
  {0x97, 0x80},
  {0x97, 0x00},
  {0x97, 0x00},
  {0xc3, 0xef},
  {0xff, 0x00},
  {0xba, 0xdc},
  {0xbb, 0x08},
  {0xb6, 0x24},
  {0xb8, 0x33},
  {0xb7, 0x20},
  {0xb9, 0x30},
  {0xb3, 0xb4},
  {0xb4, 0xca},
  {0xb5, 0x43},
  {0xb0, 0x5c},
  {0xb1, 0x4f},
  {0xb2, 0x06},
  {0xc7, 0x00},
  {0xc6, 0x51},
  {0xc5, 0x11},
  {0xc4, 0x9c},
  {0xbf, 0x00},
  {0xbc, 0x64},
  {0xa6, 0x00},
  {0xa7, 0x1e},
  {0xa7, 0x6b},
  {0xa7, 0x47},
  {0xa7, 0x33},
  {0xa7, 0x00},
  {0xa7, 0x23},
  {0xa7, 0x2e},
  {0xa7, 0x85},
  {0xa7, 0x42},
  {0xa7, 0x33},
  {0xa7, 0x00},
  {0xa7, 0x23},
  {0xa7, 0x1b},
  {0xa7, 0x74},
  {0xa7, 0x42},
  {0xa7, 0x33},
  {0xa7, 0x00},
  {0xa7, 0x23},
  {0xc0, 0xc8},
  {0xc1, 0x96},
  {0x8c, 0x00},
  {0x86, 0x3d},
  {0x50, 0x92},
  {0x51, 0x90},
  {0x52, 0x2c},
  {0x53, 0x00},
  {0x54, 0x00},
  {0x55, 0x88},
  {0x5a, 0x50},
  {0x5b, 0x3c},
  {0x5c, 0x00},
  {0xd3, 0x04},
  {0x7f, 0x00},
  {0xda, 0x00},
  {0xe5, 0x1f},
  {0xe1, 0x67},
  {0xe0, 0x00},
  {0xdd, 0x7f},
  {0x05, 0x00},
  {0xff, 0x00},
  {0xe0, 0x04},
  {0xc0, 0xc8},
  {0xc1, 0x96},
  {0x86, 0x3d},
  {0x50, 0x92},
  {0x51, 0x90},
  {0x52, 0x2c},
  {0x53, 0x00},
  {0x54, 0x00},
  {0x55, 0x88},
  {0x57, 0x00},
  {0x5a, 0x50},
  {0x5b, 0x3c},
  {0x5c, 0x00},
  {0xd3, 0x04},
  {0xe0, 0x00},
  {0xFF, 0x00},
  {0x05, 0x00},
  {0xDA, 0x08},
  {0xda, 0x09},
  {0x98, 0x00},
  {0x99, 0x00},
  {0x00, 0x00},
  {0xff, 0x00},
  {0xe0, 0x04},
  {0xc0, 0xc8},
  {0xc1, 0x96},
  {0x86, 0x35},
  {0x50, 0x80},
  {0x51, 0x90},
  {0x52, 0x2c},
  {0x53, 0x00},
  {0x54, 0x00},
  {0x55, 0x88},
  {0x57, 0x00},
  {0x5a, 0x78},
  {0x5b, 0x44},
  {0x5c, 0x00},
  {0xd3, 0x04},
  {0xe0, 0x00},
};

/* Initialization sequence for VGA resolution (640x480)*/
volatile uint8_t OV2640_VGA[237][2]=
{
  {0xff, 0x00},  /* Device control register list Table 12 */
  {0x2c, 0xff},  /* Reserved                              */
  {0x2e, 0xdf},  /* Reserved                              */
  {0xff, 0x01},  /* Device control register list Table 13 */
  {0x3c, 0x32},  /* Reserved                              */
  {0x11, 0x00},  /* Clock Rate Control                    */
  {0x09, 0x02},  /* Common control 2                      */
  {0x04, 0xA8},  /* Mirror                                */
  {0x13, 0xe5},  /* Common control 8                      */
  {0x14, 0x48},  /* Common control 9                      */
  {0x2c, 0x0c},  /* Reserved                              */
  {0x33, 0x78},  /* Reserved                              */
  {0x3a, 0x33},  /* Reserved                              */
  {0x3b, 0xfB},  /* Reserved                              */
  {0x3e, 0x00},  /* Reserved                              */
  {0x43, 0x11},  /* Reserved                              */
  {0x16, 0x10},  /* Reserved                              */
  {0x4a, 0x81},  /* Reserved                              */
  {0x21, 0x99},  /* Reserved                              */
  {0x24, 0x40},  /* Luminance signal High range           */
  {0x25, 0x38},  /* Luminance signal low range            */
  {0x26, 0x82},  /*                                       */
  {0x5c, 0x00},  /* Reserved                              */
  {0x63, 0x00},  /* Reserved                              */
  {0x46, 0x3f},  /* Frame length adjustment               */
  {0x0c, 0x3c},  /* Common control 3                      */
  {0x61, 0x70},  /* Histogram algo low level              */
  {0x62, 0x80},  /* Histogram algo high level             */
  {0x7c, 0x05},  /* Reserved                              */
  {0x20, 0x80},  /* Reserved                              */
  {0x28, 0x30},  /* Reserved                              */
  {0x6c, 0x00},  /* Reserved                              */
  {0x6d, 0x80},  /* Reserved                              */
  {0x6e, 0x00},  /* Reserved                              */
  {0x70, 0x02},  /* Reserved                              */
  {0x71, 0x94},  /* Reserved                              */
  {0x73, 0xc1},  /* Reserved                              */
  {0x3d, 0x34},  /* Reserved                              */
  {0x5a, 0x57},  /* Reserved                              */
  {0x12, 0x00},  /* Common control 7                      */
  {0x11, 0x00},  /* Clock Rate Control                   2*/
  {0x17, 0x11},  /* Horiz window start MSB 8bits          */
  {0x18, 0x75},  /* Horiz window end MSB 8bits            */
  {0x19, 0x01},  /* Vert window line start MSB 8bits      */
  {0x1a, 0x97},  /* Vert window line end MSB 8bits        */
  {0x32, 0x36},
  {0x03, 0x0f},
  {0x37, 0x40},
  {0x4f, 0xbb},
  {0x50, 0x9c},
  {0x5a, 0x57},
  {0x6d, 0x80},
  {0x6d, 0x38},
  {0x39, 0x02},
  {0x35, 0x88},
  {0x22, 0x0a},
  {0x37, 0x40},
  {0x23, 0x00},
  {0x34, 0xa0},
  {0x36, 0x1a},
  {0x06, 0x02},
  {0x07, 0xc0},
  {0x0d, 0xb7},
  {0x0e, 0x01},
  {0x4c, 0x00},
  {0xff, 0x00},
  {0xe5, 0x7f},
  {0xf9, 0xc0},
  {0x41, 0x24},
  {0xe0, 0x14},
  {0x76, 0xff},
  {0x33, 0xa0},
  {0x42, 0x20},
  {0x43, 0x18},
  {0x4c, 0x00},
  {0x87, 0xd0},
  {0x88, 0x3f},
  {0xd7, 0x03},
  {0xd9, 0x10},
  {0xd3, 0x82},
  {0xc8, 0x08},
  {0xc9, 0x80},
  {0x7d, 0x00},
  {0x7c, 0x03},
  {0x7d, 0x48},
  {0x7c, 0x08},
  {0x7d, 0x20},
  {0x7d, 0x10},
  {0x7d, 0x0e},
  {0x90, 0x00},
  {0x91, 0x0e},
  {0x91, 0x1a},
  {0x91, 0x31},
  {0x91, 0x5a},
  {0x91, 0x69},
  {0x91, 0x75},
  {0x91, 0x7e},
  {0x91, 0x88},
  {0x91, 0x8f},
  {0x91, 0x96},
  {0x91, 0xa3},
  {0x91, 0xaf},
  {0x91, 0xc4},
  {0x91, 0xd7},
  {0x91, 0xe8},
  {0x91, 0x20},
  {0x92, 0x00},
  {0x93, 0x06},
  {0x93, 0xe3},
  {0x93, 0x02},
  {0x93, 0x02},
  {0x93, 0x00},
  {0x93, 0x04},
  {0x93, 0x00},
  {0x93, 0x03},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x96, 0x00},
  {0x97, 0x08},
  {0x97, 0x19},
  {0x97, 0x02},
  {0x97, 0x0c},
  {0x97, 0x24},
  {0x97, 0x30},
  {0x97, 0x28},
  {0x97, 0x26},
  {0x97, 0x02},
  {0x97, 0x98},
  {0x97, 0x80},
  {0x97, 0x00},
  {0x97, 0x00},
  {0xc3, 0xef},
  {0xff, 0x00},
  {0xba, 0xdc},
  {0xbb, 0x08},
  {0xb6, 0x24},
  {0xb8, 0x33},
  {0xb7, 0x20},
  {0xb9, 0x30},
  {0xb3, 0xb4},
  {0xb4, 0xca},
  {0xb5, 0x43},
  {0xb0, 0x5c},
  {0xb1, 0x4f},
  {0xb2, 0x06},
  {0xc7, 0x00},
  {0xc6, 0x51},
  {0xc5, 0x11},
  {0xc4, 0x9c},
  {0xbf, 0x00},
  {0xbc, 0x64},
  {0xa6, 0x00},
  {0xa7, 0x1e},
  {0xa7, 0x6b},
  {0xa7, 0x47},
  {0xa7, 0x33},
  {0xa7, 0x00},
  {0xa7, 0x23},
  {0xa7, 0x2e},
  {0xa7, 0x85},
  {0xa7, 0x42},
  {0xa7, 0x33},
  {0xa7, 0x00},
  {0xa7, 0x23},
  {0xa7, 0x1b},
  {0xa7, 0x74},
  {0xa7, 0x42},
  {0xa7, 0x33},
  {0xa7, 0x00},
  {0xa7, 0x23},
  {0xc0, 0xc8},
  {0xc1, 0x96},
  {0x8c, 0x00},
  {0x86, 0x3d},
  {0x50, 0x92},
  {0x51, 0x90},
  {0x52, 0x2c},
  {0x53, 0x00},
  {0x54, 0x00},
  {0x55, 0x88},
  {0x5a, 0x50},
  {0x5b, 0x3c},
  {0x5c, 0x00},
  {0xd3, 0x04},
  {0x7f, 0x00},
  {0xda, 0x00},
  {0xe5, 0x1f},
  {0xe1, 0x67},
  {0xe0, 0x00},
  {0xdd, 0x7f},
  {0x05, 0x00},
  {0xff, 0x00},
  {0xe0, 0x04},
  {0xc0, 0xc8},
  {0xc1, 0x96},
  {0x86, 0x3d},
  {0x50, 0x92},
  {0x51, 0x90},
  {0x52, 0x2c},
  {0x53, 0x00},
  {0x54, 0x00},
  {0x55, 0x88},
  {0x57, 0x00},
  {0x5a, 0x50},
  {0x5b, 0x3c},
  {0x5c, 0x00},
  {0xd3, 0x04},
  {0xe0, 0x00},
  {0xFF, 0x00},
  {0x05, 0x00},
  {0xDA, 0x08},
  {0xda, 0x09},
  {0x98, 0x00},
  {0x99, 0x00},
  {0x00, 0x00},
  {0xff, 0x00},
  {0xe0, 0x04},
  {0xc0, 0xc8},
  {0xc1, 0x96},
  {0x86, 0x3d},
  {0x50, 0x89},
  {0x51, 0x90},
  {0x52, 0x2c},
  {0x53, 0x00},
  {0x54, 0x00},
  {0x55, 0x88},
  {0x57, 0x00},
  {0x5a, 0xA0},
  {0x5b, 0x78},
  {0x5c, 0x00},
  {0xd3, 0x02},
  {0xe0, 0x00},
};

/* Initialization sequence for QVGA resolution (320x240) */
volatile uint8_t OV2640_QVGA[220][2]=
{
  {0xff, 0x00},
  {0x2c, 0xff},
  {0x2e, 0xdf},
  {0xff, 0x01},
 // {OV2640_SENSOR_COM10, 0x0A},
  {0x3c, 0x32},
  {0x11, 0x00},
  {0x09, 0x02},
  {0x04, 0xA8},
  {0x13, 0xe5},
  {0x14, 0x48},
  {0x2c, 0x0c},
  {0x33, 0x78},
  {0x3a, 0x33},
  {0x3b, 0xfB},
  {0x3e, 0x00},
  {0x43, 0x11},
  {0x16, 0x10},
  {0x4a, 0x81},
  {0x21, 0x99},
  {0x24, 0x40},
  {0x25, 0x38},
  {0x26, 0x82},
  {0x5c, 0x00},
  {0x63, 0x00},
  {0x46, 0x3f},
  {0x0c, 0x3c},
  {0x61, 0x70},
  {0x62, 0x80},
  {0x7c, 0x05},
  {0x20, 0x80},
  {0x28, 0x30},
  {0x6c, 0x00},
  {0x6d, 0x80},
  {0x6e, 0x00},
  {0x70, 0x02},
  {0x71, 0x94},
  {0x73, 0xc1},
  {0x3d, 0x34},
  {0x5a, 0x57},
  {0x12, 0x00},
  {0x11, 0x00},
  {0x17, 0x11},
  {0x18, 0x75},
  {0x19, 0x01},
  {0x1a, 0x97},
  {0x32, 0x36},
  {0x03, 0x0f},
  {0x37, 0x40},
  {0x4f, 0xbb},
  {0x50, 0x9c},
  {0x5a, 0x57},
  {0x6d, 0x80},
  {0x6d, 0x38},
  {0x39, 0x02},
  {0x35, 0x88},
  {0x22, 0x0a},
  {0x37, 0x40},
  {0x23, 0x00},
  {0x34, 0xa0},
  {0x36, 0x1a},
  {0x06, 0x02},
  {0x07, 0xc0},
  {0x0d, 0xb7},
  {0x0e, 0x01},
  {0x4c, 0x00},
  {0xff, 0x00},
  {0xe5, 0x7f},
  {0xf9, 0xc0},
  {0x41, 0x24},
  {0xe0, 0x14},
  {0x76, 0xff},
  {0x33, 0xa0},
  {0x42, 0x20},
  {0x43, 0x18},
  {0x4c, 0x00},
  {0x87, 0xd0},
  {0x88, 0x3f},
  {0xd7, 0x03},
  {0xd9, 0x10},
  {0xd3, 0x82},
  {0xc8, 0x08},
  {0xc9, 0x80},
  {0x7d, 0x00},
  {0x7c, 0x03},
  {0x7d, 0x48},
  {0x7c, 0x08},
  {0x7d, 0x20},
  {0x7d, 0x10},
  {0x7d, 0x0e},
  {0x90, 0x00},
  {0x91, 0x0e},
  {0x91, 0x1a},
  {0x91, 0x31},
  {0x91, 0x5a},
  {0x91, 0x69},
  {0x91, 0x75},
  {0x91, 0x7e},
  {0x91, 0x88},
  {0x91, 0x8f},
  {0x91, 0x96},
  {0x91, 0xa3},
  {0x91, 0xaf},
  {0x91, 0xc4},
  {0x91, 0xd7},
  {0x91, 0xe8},
  {0x91, 0x20},
  {0x92, 0x00},
  {0x93, 0x06},
  {0x93, 0xe3},
  {0x93, 0x02},
  {0x93, 0x02},
  {0x93, 0x00},
  {0x93, 0x04},
  {0x93, 0x00},
  {0x93, 0x03},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x96, 0x00},
  {0x97, 0x08},
  {0x97, 0x19},
  {0x97, 0x02},
  {0x97, 0x0c},
  {0x97, 0x24},
  {0x97, 0x30},
  {0x97, 0x28},
  {0x97, 0x26},
  {0x97, 0x02},
  {0x97, 0x98},
  {0x97, 0x80},
  {0x97, 0x00},
  {0x97, 0x00},
  {0xc3, 0xef},
  {0xff, 0x00},
  {0xba, 0xdc},
  {0xbb, 0x08},
  {0xb6, 0x24},
  {0xb8, 0x33},
  {0xb7, 0x20},
  {0xb9, 0x30},
  {0xb3, 0xb4},
  {0xb4, 0xca},
  {0xb5, 0x43},
  {0xb0, 0x5c},
  {0xb1, 0x4f},
  {0xb2, 0x06},
  {0xc7, 0x00},
  {0xc6, 0x51},
  {0xc5, 0x11},
  {0xc4, 0x9c},
  {0xbf, 0x00},
  {0xbc, 0x64},
  {0xa6, 0x00},
  {0xa7, 0x1e},
  {0xa7, 0x6b},
  {0xa7, 0x47},
  {0xa7, 0x33},
  {0xa7, 0x00},
  {0xa7, 0x23},
  {0xa7, 0x2e},
  {0xa7, 0x85},
  {0xa7, 0x42},
  {0xa7, 0x33},
  {0xa7, 0x00},
  {0xa7, 0x23},
  {0xa7, 0x1b},
  {0xa7, 0x74},
  {0xa7, 0x42},
  {0xa7, 0x33},
  {0xa7, 0x00},
  {0xa7, 0x23},
  {0xc0, 0xc8},
  {0xc1, 0x96},
  {0x8c, 0x00},
  {0x86, 0x3d},
  {0x50, 0x92},
  {0x51, 0x90},
  {0x52, 0x2c},
  {0x53, 0x00},
  {0x54, 0x00},
  {0x55, 0x88},
  {0x5a, 0x50},
  {0x5b, 0x3c},
  {0x5c, 0x00},
  {0xd3, 0x04},
  {0x7f, 0x00},
  {0xda, 0x00},
  {0xe5, 0x1f},
  {0xe1, 0x67},
  {0xe0, 0x00},
  {0xdd, 0x7f},
  {0x05, 0x00},
  {0xff, 0x00},
  {0xe0, 0x04},
  {0xc0, 0xc8},
  {0xc1, 0x96},
  {0x86, 0x3d},
  {0x50, 0x92},
  {0x51, 0x90},
  {0x52, 0x2c},
  {0x53, 0x00},
  {0x54, 0x00},
  {0x55, 0x88},
  {0x57, 0x00},
  {0x5a, 0x50},
  {0x5b, 0x3C},
  {0x5c, 0x00},
  {0xd3, 0x08},
  {0xe0, 0x00},
  {0xFF, 0x00},
  {0x05, 0x00},
  {0xDA, 0x08},
  {0xda, 0x09},
  {0x98, 0x00},
  {0x99, 0x00},
  {0x00, 0x00},
};

/* Initialization sequence for QQVGA resolution (160x120) */
volatile uint8_t OV2640_QQVGA[220][2]=
{
  {0xff, 0x00},
  {0x2c, 0xff},
  {0x2e, 0xdf},
  {0xff, 0x01},
  {0x3c, 0x32},
  {0x11, 0x00},
  {0x09, 0x02},
  {0x04, 0xA8},
  {0x13, 0xe5},
  {0x14, 0x48},
  {0x2c, 0x0c},
  {0x33, 0x78},
  {0x3a, 0x33},
  {0x3b, 0xfB},
  {0x3e, 0x00},
  {0x43, 0x11},
  {0x16, 0x10},
  {0x4a, 0x81},
  {0x21, 0x99},
  {0x24, 0x40},
  {0x25, 0x38},
  {0x26, 0x82},
  {0x5c, 0x00},
  {0x63, 0x00},
  {0x46, 0x3f},
  {0x0c, 0x3c},
  {0x61, 0x70},
  {0x62, 0x80},
  {0x7c, 0x05},
  {0x20, 0x80},
  {0x28, 0x30},
  {0x6c, 0x00},
  {0x6d, 0x80},
  {0x6e, 0x00},
  {0x70, 0x02},
  {0x71, 0x94},
  {0x73, 0xc1},
  {0x3d, 0x34},
  {0x5a, 0x57},
  {0x12, 0x00},
  {0x11, 0x00},
  {0x17, 0x11},
  {0x18, 0x75},
  {0x19, 0x01},
  {0x1a, 0x97},
  {0x32, 0x36},
  {0x03, 0x0f},
  {0x37, 0x40},
  {0x4f, 0xbb},
  {0x50, 0x9c},
  {0x5a, 0x57},
  {0x6d, 0x80},
  {0x6d, 0x38},
  {0x39, 0x02},
  {0x35, 0x88},
  {0x22, 0x0a},
  {0x37, 0x40},
  {0x23, 0x00},
  {0x34, 0xa0},
  {0x36, 0x1a},
  {0x06, 0x02},
  {0x07, 0xc0},
  {0x0d, 0xb7},
  {0x0e, 0x01},
  {0x4c, 0x00},
  {0xff, 0x00},
  {0xe5, 0x7f},
  {0xf9, 0xc0},
  {0x41, 0x24},
  {0xe0, 0x14},
  {0x76, 0xff},
  {0x33, 0xa0},
  {0x42, 0x20},
  {0x43, 0x18},
  {0x4c, 0x00},
  {0x87, 0xd0},
  {0x88, 0x3f},
  {0xd7, 0x03},
  {0xd9, 0x10},
  {0xd3, 0x82},
  {0xc8, 0x08},
  {0xc9, 0x80},
  {0x7d, 0x00},
  {0x7c, 0x03},
  {0x7d, 0x48},
  {0x7c, 0x08},
  {0x7d, 0x20},
  {0x7d, 0x10},
  {0x7d, 0x0e},
  {0x90, 0x00},
  {0x91, 0x0e},
  {0x91, 0x1a},
  {0x91, 0x31},
  {0x91, 0x5a},
  {0x91, 0x69},
  {0x91, 0x75},
  {0x91, 0x7e},
  {0x91, 0x88},
  {0x91, 0x8f},
  {0x91, 0x96},
  {0x91, 0xa3},
  {0x91, 0xaf},
  {0x91, 0xc4},
  {0x91, 0xd7},
  {0x91, 0xe8},
  {0x91, 0x20},
  {0x92, 0x00},
  {0x93, 0x06},
  {0x93, 0xe3},
  {0x93, 0x02},
  {0x93, 0x02},
  {0x93, 0x00},
  {0x93, 0x04},
  {0x93, 0x00},
  {0x93, 0x03},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x93, 0x00},
  {0x96, 0x00},
  {0x97, 0x08},
  {0x97, 0x19},
  {0x97, 0x02},
  {0x97, 0x0c},
  {0x97, 0x24},
  {0x97, 0x30},
  {0x97, 0x28},
  {0x97, 0x26},
  {0x97, 0x02},
  {0x97, 0x98},
  {0x97, 0x80},
  {0x97, 0x00},
  {0x97, 0x00},
  {0xc3, 0xef},
  {0xff, 0x00},
  {0xba, 0xdc},
  {0xbb, 0x08},
  {0xb6, 0x24},
  {0xb8, 0x33},
  {0xb7, 0x20},
  {0xb9, 0x30},
  {0xb3, 0xb4},
  {0xb4, 0xca},
  {0xb5, 0x43},
  {0xb0, 0x5c},
  {0xb1, 0x4f},
  {0xb2, 0x06},
  {0xc7, 0x00},
  {0xc6, 0x51},
  {0xc5, 0x11},
  {0xc4, 0x9c},
  {0xbf, 0x00},
  {0xbc, 0x64},
  {0xa6, 0x00},
  {0xa7, 0x1e},
  {0xa7, 0x6b},
  {0xa7, 0x47},
  {0xa7, 0x33},
  {0xa7, 0x00},
  {0xa7, 0x23},
  {0xa7, 0x2e},
  {0xa7, 0x85},
  {0xa7, 0x42},
  {0xa7, 0x33},
  {0xa7, 0x00},
  {0xa7, 0x23},
  {0xa7, 0x1b},
  {0xa7, 0x74},
  {0xa7, 0x42},
  {0xa7, 0x33},
  {0xa7, 0x00},
  {0xa7, 0x23},
  {0xc0, 0xc8},
  {0xc1, 0x96},
  {0x8c, 0x00},
  {0x86, 0x3d},
  {0x50, 0x92},
  {0x51, 0x90},
  {0x52, 0x2c},
  {0x53, 0x00},
  {0x54, 0x00},
  {0x55, 0x88},
  {0x5a, 0x50},
  {0x5b, 0x3c},
  {0x5c, 0x00},
  {0xd3, 0x04},
  {0x7f, 0x00},
  {0xda, 0x00},
  {0xe5, 0x1f},
  {0xe1, 0x67},
  {0xe0, 0x00},
  {0xdd, 0x7f},
  {0x05, 0x00},
  {0xff, 0x00},
  {0xe0, 0x04},
  {0xc0, 0xc8},
  {0xc1, 0x96},
  {0x86, 0x3d},
  {0x50, 0x92},
  {0x51, 0x90},
  {0x52, 0x2c},
  {0x53, 0x00},
  {0x54, 0x00},
  {0x55, 0x88},
  {0x57, 0x00},
  {0x5a, 0x28}, 
  {0x5b, 0x1E}, 
  {0x5c, 0x00},
  {0xd3, 0x08},
  {0xe0, 0x00},
  {0xFF, 0x00},
  {0x05, 0x00},
  {0xDA, 0x08},
  {0xda, 0x09},
  {0x98, 0x00},
  {0x99, 0x00},
  {0x00, 0x00},
};






#define CAM_I2C 	I2C2
#define CAM_I2C_AF	GPIO_AF4
#define CAM_I2C_RCC RCC_I2C2

#define CAM_DCMI	DCMI_BASE
#define CAM_DCMI_AF	GPIO_AF13
#define CAM_DCMI_RCC RCC_DCMI

gpiopin CAM_SDA={GPIOB, GPIO11};
gpiopin CAM_SCL={GPIOB, GPIO10};

gpiopin CAM_D0={GPIOC, GPIO6};
gpiopin CAM_D1={GPIOC, GPIO7};
gpiopin CAM_D2={GPIOC, GPIO8};
gpiopin CAM_D3={GPIOC, GPIO9};
gpiopin CAM_D4={GPIOC, GPIO11};
gpiopin CAM_D5={GPIOD, GPIO3};
gpiopin CAM_D6={GPIOB, GPIO8};
gpiopin CAM_D7={GPIOB, GPIO9};
gpiopin CAM_PCLK={GPIOA, GPIO6};
gpiopin CAM_HSYNC={GPIOA, GPIO4};
gpiopin CAM_VSYNC={GPIOG, GPIO9};
gpiopin CAM_PWDN={GPIOB, GPIO6};
gpiopin CAM_RST={GPIOA, GPIO5};
//XCLK already defined in MCO config

volatile ov2640 Camera = {0, 1, };

static volatile uint32_t i2c_ticks=0;

static void inline I2C_Bitbang_Transaction(uint8_t byte){
    bool falling_edge=false;
    uint8_t mask_bit=0;
    for (int8_t i=7; i>=0 ; i--) {
        if(falling_edge){
            delay_us(6);
            gpio_clear(CAM_SCL.port, CAM_SCL.pin);
            delay_us(6);
            i++;
            falling_edge=false;
            goto END;
        }
        else{
            falling_edge=true;
        }

        
        mask_bit=(uint8_t)(1<<i);

        if (byte&mask_bit)
            gpio_set(CAM_SDA.port, CAM_SDA.pin);
        else
            gpio_clear(CAM_SDA.port, CAM_SDA.pin);
        
        delay_us(6);
        gpio_set(CAM_SCL.port, CAM_SCL.pin);
        delay_us(6);

END :   asm("NOP");
        asm("NOP");
    }
    delay_us(6);
    //Check the X bit (NACK'd by the master -> Camera) : reading the Data or just wait 1 period. Clk must be working
    gpio_clear(CAM_SCL.port, CAM_SCL.pin);
    delay_us(6);
    gpio_clear(CAM_SDA.port, CAM_SDA.pin);
    delay_us(6);
    gpio_set(CAM_SCL.port, CAM_SCL.pin);
    delay_us(12);
    gpio_clear(CAM_SCL.port, CAM_SCL.pin);
    delay_us(6);
}

static void inline I2C_Bitbang_Clock_Only(){ //can extend to N cycles with a loop

    for (int8_t i=7; i>=0 ; i--) {
        delay_us(6);
        gpio_set(CAM_SCL.port, CAM_SCL.pin);
        delay_us(12);
        gpio_clear(CAM_SCL.port, CAM_SCL.pin);
        delay_us(6);
    }
    
}

uint8_t SCCB_Bitbang(bool R_W, uint8_t addr, uint8_t reg_adr, uint8_t reg_data) //Because Using I²C is for faggots - Omnivision 1999
{

    //Send Start "bit" : D = 0 then C = 0 
    gpio_clear(CAM_SDA.port, CAM_SDA.pin);
    delay_us(5);
    gpio_clear(CAM_SCL.port, CAM_SCL.pin);
    delay_us(6);

    //Send Address : on Clk rising - Assert Data around a half period before Clk edge
    //addr <<=1; // 8th bit is discarded
    if(R_W){
        addr++;
    }

    I2C_Bitbang_Transaction(addr);
    //Repeat for Reg address - Same
    I2C_Bitbang_Transaction(reg_adr);
    if(!R_W){    
        //Repeat for Reg Data - Same
        I2C_Bitbang_Transaction(reg_data);
    
        //Send Stop "bit" : D=0 while C is 0. C=1 then D=1.
        //delay_us(6);
        gpio_clear(CAM_SCL.port, CAM_SCL.pin);
        delay_us(6);
        gpio_clear(CAM_SDA.port, CAM_SDA.pin);
        delay_us(6);
        gpio_set(CAM_SCL.port, CAM_SCL.pin);
        delay_us(5);
        gpio_set(CAM_SDA.port, CAM_SDA.pin);
        delay_us(10);
        return;
    }

    //Read one byte (i guess ?)
    //Make SDA an input
    gpio_mode_setup(CAM_SDA.port, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, CAM_SDA.pin);
    delay_us(10);
    I2C_Bitbang_Clock_Only();
    //go back to an Output
    gpio_mode_setup(CAM_SDA.port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, CAM_SDA.pin);
	gpio_set_output_options(CAM_SDA.port, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, CAM_SDA.pin); //Always use PP instead of OD (faster slew rates)
    gpio_set(CAM_SDA.port, CAM_SDA.pin);

    delay_us(6);
    //Check the X bit (NACK'd by the master -> Camera) : reading the Data or just wait 1 period. Clk must be working
    gpio_clear(CAM_SCL.port, CAM_SCL.pin);
    delay_us(6);
    gpio_clear(CAM_SDA.port, CAM_SDA.pin);
    delay_us(6);
    gpio_set(CAM_SCL.port, CAM_SCL.pin);
    delay_us(12);
    gpio_clear(CAM_SCL.port, CAM_SCL.pin);
    delay_us(6);

    //Stop bit
    delay_us(6);
    gpio_set(CAM_SCL.port, CAM_SCL.pin);
    delay_us(5);
    gpio_set(CAM_SDA.port, CAM_SDA.pin);
    delay_us(10);

    
}

void i2c_xfer7_X(uint32_t i2c, uint8_t addr, uint8_t command, uint8_t w)
{   //100kHz => wait 10*10us = 100us. Setup a timer to do this ? 
    I2C_ICR(i2c)|=I2C_ICR_NACKCF; //clear NACKF
    i2c_set_7bit_address(i2c, addr);
    i2c_set_write_transfer_dir(i2c);
    i2c_set_bytes_to_transfer(i2c, 2);
    i2c_disable_autoend(i2c);
    i2c_send_start(i2c);
    
    bool wait = true;
    while (wait) {
        if (i2c_transmit_int_status(i2c)) {
            wait = false;
        }
        while (i2c_nack(i2c)){
          delay_us(120);
          wait=false;
          break;
        } /* Some error */
    }
    //I2C_ICR(i2c)|=I2C_ICR_NACKCF; //clear NACKF

		i2c_send_data(i2c, command);

    wait = true;
    while (wait) {
        if (i2c_transmit_int_status(i2c)) {
            wait = false;
        }
        while (i2c_nack(i2c)){
          delay_us(120);
          wait=false;
          break;
        } /* Some error */
    }
    I2C_ICR(i2c)|=I2C_ICR_NACKCF; //clear NACKF
    i2c_send_data(i2c, w);

    i2c_send_stop(i2c);
}



/**
 * Write value to camera register.
 * @param reg_addr Address of register.
 * @param data New value.
 * @return  Operation status.
 */
static inline void SCCB_Write(uint8_t reg_addr, uint8_t data) {
	//__disable_irq();
	SCCB_Bitbang(WRITE_I2C, CAM_ADDR, reg_addr, data);
	//__enable_irq();
	return;
}

static inline void SCCB_Write_Multi(uint8_t reg_data[][2], uint32_t len) {
	//__disable_irq();
	for(uint32_t i=0; i<len; i++){
        uart_printf("Reg Addr : 0x%x - Reg Data : 0x%x\n", reg_data[i][0], reg_data[i][1]);
		SCCB_Bitbang(WRITE_I2C, CAM_ADDR, reg_data[i][0], reg_data[i][1]);
		delay_us(20);
	}
	//__enable_irq();
	return;
}

/**
 * Reading data from camera registers.
 * @param reg_addr Address of register.
 * @param pdata Value read from register.
 * @return Operation status.
 */
static inline void SCCB_Read(uint8_t reg_addr, uint8_t* data) {
	//__disable_irq();
	*data = SCCB_Bitbang(READ_I2C, CAM_ADDR, reg_addr, data);
	//__enable_irq();
	return;
}

/**
 * Camera initialization.
 * @param p_hi2c Pointer to I2C interface.
 * @param p_hdcmi Pointer to DCMI interface.
 */
void OV2640_Init(uint32_t resolution) {
    //I2C Config
    //RCC_DCKCFGR2&= ~(RCC_DCKCFGR2_I2C2SEL_MASK<<RCC_DCKCFGR2_I2C2SEL_SHIFT);
	//RCC_DCKCFGR2|= (RCC_DCKCFGR2_UARTxSEL_HSI<<RCC_DCKCFGR2_I2C2SEL_SHIFT);
	//rcc_periph_clock_enable(CAM_I2C_RCC);
	//i2c_reset(CAM_I2C);
	gpio_mode_setup(CAM_SDA.port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, CAM_SDA.pin|CAM_SCL.pin);
	gpio_set_output_options(CAM_SDA.port, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, CAM_SDA.pin|CAM_SCL.pin); //Always use PP instead of OD (faster slew rates)
	//gpio_set_af(CAM_SDA.port, CAM_I2C_AF, CAM_SDA.pin|CAM_SCL.pin);	
	//i2c_peripheral_disable(CAM_I2C);
	//i2c_set_7bit_addr_mode(CAM_I2C);
	//i2c_enable_analog_filter(CAM_I2C);
	//i2c_set_digital_filter(CAM_I2C, 0);
	//i2c_set_speed(CAM_I2C,i2c_speed_sm_100k,20U); //since T_cyc has a limit of 10us, I used 20 (instead of its real value of 16) which gives me 87kHz (11.5us).
	//delay_ms(1000);
	//i2c_peripheral_enable(CAM_I2C);

    gpio_set(CAM_SDA.port, CAM_SDA.pin);
    gpio_set(CAM_SCL.port, CAM_SCL.pin);


    delay_ms(10);


	//DCMI Config
	rcc_periph_clock_enable(RCC_DCMI);
    rcc_periph_reset_pulse(RST_DCMI);
	rcc_periph_clock_enable(RCC_DCMI);

	gpio_mode_setup(CAM_D0.port, GPIO_MODE_AF, GPIO_PUPD_NONE, CAM_D0.pin|CAM_D1.pin|CAM_D2.pin|CAM_D3.pin|CAM_D4.pin);
	gpio_mode_setup(CAM_D5.port, GPIO_MODE_AF, GPIO_PUPD_NONE, CAM_D5.pin);
	gpio_mode_setup(CAM_D6.port, GPIO_MODE_AF, GPIO_PUPD_NONE, CAM_D6.pin|CAM_D7.pin);

	gpio_set_af(CAM_D0.port, CAM_DCMI_AF, CAM_D0.pin|CAM_D1.pin|CAM_D2.pin|CAM_D3.pin|CAM_D4.pin);
	gpio_set_af(CAM_D5.port, CAM_DCMI_AF, CAM_D5.pin);
	gpio_set_af(CAM_D6.port, CAM_DCMI_AF, CAM_D6.pin|CAM_D7.pin);

	gpio_mode_setup(CAM_PCLK.port, GPIO_MODE_AF, GPIO_PUPD_NONE, CAM_PCLK.pin|CAM_HSYNC.pin);
	gpio_set_af(CAM_PCLK.port, CAM_DCMI_AF, CAM_PCLK.pin|CAM_HSYNC.pin);

	gpio_mode_setup(CAM_VSYNC.port, GPIO_MODE_AF, GPIO_PUPD_NONE, CAM_VSYNC.pin);
	gpio_set_af(CAM_VSYNC.port, CAM_DCMI_AF, CAM_VSYNC.pin);

	gpio_mode_setup(CAM_PWDN.port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, CAM_PWDN.pin); //Active high pulled low in pcb
    gpio_set_output_options(CAM_PWDN.port, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, CAM_PWDN.pin);
	gpio_mode_setup(CAM_RST.port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, CAM_RST.pin); //Active low
    gpio_set_output_options(CAM_RST.port, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, CAM_RST.pin);

	gpio_clear(CAM_PWDN.port, CAM_PWDN.pin);
	gpio_set(CAM_RST.port, CAM_RST.pin);

    delay_ms(10);
        
    //SW Reset of Camera
    SCCB_Write(OV2640_DSP_RA_DLMT, 0x01);delay_us(100);
    SCCB_Write(OV2640_DSP_RA_DLMT, 0x01);delay_us(100);
    SCCB_Write(OV2640_SENSOR_COM7, 0x80);


    /* Initialize OV2640 */   

    delay_ms(10);


	switch (resolution)
	{
	case CAM_160x120:
	{

		SCCB_Write_Multi(OV2640_QQVGA, sizeof(OV2640_QQVGA)/2);
 
		break; 	
	}    
	case CAM_320x240:
	{

		SCCB_Write_Multi(OV2640_QVGA, sizeof(OV2640_QVGA)/2);

		break;
	}
	case CAM_480x272:
	{

		SCCB_Write_Multi(OV2640_480x272, sizeof(OV2640_480x272)/2);

		break;
	}
	case CAM_640x480:
	{

		SCCB_Write_Multi(OV2640_VGA, sizeof(OV2640_VGA)/2);

		break;
	}    
	default:
	{
		break;
	}
	}

    delay_ms(10);
	
	//delay_ms(1000);
    DCMI_CR=0;
	//V-Hsync active state
	DCMI_CR |= DCMI_CR_PCKPOL; //DCMI_CR_HSPOL|DCMI_CR_VSPOL|
	//Snapshot mode
	DCMI_CR |= DCMI_CR_CM;
	//Enable end of capture interrupt
	DCMI_IER|= DCMI_IER_FRAME;
	//Enable interrupt on NVIC
	nvic_set_priority(NVIC_DCMI_IRQ, 0b11100000);
    nvic_enable_irq(NVIC_DCMI_IRQ);
    
	
	//DCMI DMA Config
	rcc_periph_clock_enable(RCC_DMA2);
    rcc_periph_reset_pulse(RST_DMA2);
	rcc_periph_clock_enable(RCC_DMA2);
	/////////////////////////////////////////////////////////
    //DCMI : DMA 2 Stream 1 Ch 1
    dma_stream_reset(DMA2, DMA_STREAM1);
	dma_set_priority(DMA2, DMA_STREAM1, DMA_SxCR_PL_VERY_HIGH);
	
	dma_set_memory_size(DMA2, DMA_STREAM1, DMA_SxCR_MSIZE_32BIT);
	dma_set_peripheral_size(DMA2, DMA_STREAM1, DMA_SxCR_PSIZE_32BIT);
	
	dma_set_peripheral_address(DMA2, DMA_STREAM1, (uint32_t) &DCMI_DR);
    dma_set_memory_address(DMA2, DMA_STREAM1, (uint32_t) Camera.RGB565);

    dma_enable_memory_increment_mode(DMA2, DMA_STREAM1);
    dma_disable_peripheral_increment_mode(DMA2, DMA_STREAM1);

	dma_set_transfer_mode(DMA2, DMA_STREAM1, 
	DMA_SxCR_DIR_PERIPHERAL_TO_MEM); 

	dma_channel_select(DMA2, DMA_STREAM1, DMA_SxCR_CHSEL_1);	
    //dma_enable_transfer_complete_interrupt(DMA2, DMA_STREAM1);


    //dma_enable_stream(DMA2, DMA_STREAM1);

    //nvic_set_priority(NVIC_DMA2_STREAM1_IRQ, 0b00100000);
    //nvic_enable_irq(NVIC_DMA2_STREAM1_IRQ);

    //delay_ms(1000);

	// Software reset: reset all registers to default values

/*
	SCCB_Write(OV2640_DSP_RA_DLMT, 0x01);delay_us(100);
    SCCB_Write(OV2640_DSP_RA_DLMT, 0x01);delay_us(100);
    SCCB_Write(OV2640_SENSOR_COM7, 0x80);
    SCCB_Read(OV2640_DSP_RA_DLMT, &reg_read);delay_us(2);
    SCCB_Write(OV2640_DSP_RA_DLMT, 0x01);delay_us(2);
    SCCB_Read(OV2640_DSP_RA_DLMT, &reg_read);delay_us(2);
    
    delay_us(100);

    SCCB_Write(OV2640_SENSOR_COM3, 0x04);delay_us(2);
    SCCB_Read(OV2640_SENSOR_COM3, &reg_read);delay_us(2);

    delay_us(100);

    SCCB_Read(OV2640_SENSOR_COM2, &reg_read);delay_us(2);
    SCCB_Read(OV2640_SENSOR_COM1, &reg_read);delay_us(2);
    SCCB_Read(OV2640_DSP_RA_DLMT, &reg_read);delay_us(2);
*/


	
    //while(1);
	//delay_ms(2000);
    

	//Enable DCMI
	DCMI_CR |= DCMI_CR_EN;

}

void cringe_function(){

/*    
    //DCMI Config
	rcc_periph_clock_enable(RCC_DCMI);
    rcc_periph_reset_pulse(RST_DCMI);
	rcc_periph_clock_enable(RCC_DCMI);

	//delay_ms(1000);
    DCMI_CR=0;
	//V-Hsync active state
	DCMI_CR |= DCMI_CR_PCKPOL; //DCMI_CR_HSPOL|DCMI_CR_VSPOL|
	//Snapshot mode
	DCMI_CR |= DCMI_CR_CM;
	//Enable end of capture interrupt
	DCMI_IER|= DCMI_IER_FRAME;
	//Enable interrupt on NVIC
	nvic_set_priority(NVIC_DCMI_IRQ, 0b11100000);
    nvic_enable_irq(NVIC_DCMI_IRQ);

	
	//DCMI DMA Config
	rcc_periph_clock_enable(RCC_DMA2);
    rcc_periph_reset_pulse(RST_DMA2);
	rcc_periph_clock_enable(RCC_DMA2);
	/////////////////////////////////////////////////////////
    
*/ 
    //DCMI : DMA 2 Stream 1 Ch 1
    dma_stream_reset(DMA2, DMA_STREAM1);

	dma_set_priority(DMA2, DMA_STREAM1, DMA_SxCR_PL_VERY_HIGH);
	
	dma_set_memory_size(DMA2, DMA_STREAM1, DMA_SxCR_MSIZE_32BIT);
	dma_set_peripheral_size(DMA2, DMA_STREAM1, DMA_SxCR_PSIZE_32BIT);
	
	dma_set_peripheral_address(DMA2, DMA_STREAM1, (uint32_t) &DCMI_DR);
    dma_set_memory_address(DMA2, DMA_STREAM1, (uint32_t) Camera.RGB565);

    dma_enable_memory_increment_mode(DMA2, DMA_STREAM1);
    dma_disable_peripheral_increment_mode(DMA2, DMA_STREAM1);

	dma_set_transfer_mode(DMA2, DMA_STREAM1, 
	DMA_SxCR_DIR_PERIPHERAL_TO_MEM); 

    dma_set_number_of_data(DMA2, DMA_STREAM1, 38400);

	dma_channel_select(DMA2, DMA_STREAM1, DMA_SxCR_CHSEL_1);	
    //dma_enable_transfer_complete_interrupt(DMA2, DMA_STREAM1);


	//Enable DCMI
	//DCMI_CR |= DCMI_CR_EN;
    
    
}

/**
 * @brief Work in progress 
 * 
 * 
 */
void dcmi_isr(void){
	if(DCMI_MIS & DCMI_MIS_FRAME){
		DCMI_ICR |= DCMI_ICR_FRAME | DCMI_ICR_ERR | DCMI_ICR_LINE | DCMI_ICR_OVR | DCMI_ICR_VSYNC; //reset flag
		Camera.CaptureDone=true;
		Camera.isCapturing=false;
        return;
	}
    asm("NOP");
}


void dma2_stream1_isr(void){
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
}
