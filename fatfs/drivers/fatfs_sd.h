/*-----------------------------------------------------------------------/
/  Low level disk interface modlue include file   (C)ChaN, 2013          /
/-----------------------------------------------------------------------*/

#ifndef _DISKIO_DEFINED_SD
#define _DISKIO_DEFINED_SD

#define _USE_WRITE	1	/* 1: Enable disk_write function */
#define _USE_IOCTL	1	/* 1: Enable disk_ioctl fucntion */

#include "diskio.h"
#include "integer.h"

#include "main.h"
#include "defines.h"

void SD_FATFS_InitPins();
void init_spi();

#ifndef FATFS_SPI
#define FATFS_SPI							SPI4
#define RCC_FATFS_SPI                       RCC_SPI4
#define FATFS_SPI_PORT						GPIOE
#define FATFS_SPI_SCLK						GPIO2
#define FATFS_SPI_MOSI						GPIO6
#define FATFS_SPI_MISO						GPIO5
#define FATFS_SPI_AF                        GPIO_AF5
#define FATFS_CS_PORT						GPIOE
#define FATFS_CS_PIN						GPIO4
#endif

#ifndef FATFS_USE_DETECT_PIN
#define FATFS_USE_DETECT_PIN				0
#endif

#ifndef FATFS_USE_WRITEPROTECT_PIN
#define FATFS_USE_WRITEPROTECT_PIN			0
#endif

#if FATFS_USE_DETECT_PIN > 0
#ifndef FATFS_USE_DETECT_PIN_PIN			
#define FATFS_USE_DETECT_PIN_PORT			GPIOB
#define FATFS_USE_DETECT_PIN_PIN			GPIO_PIN_6
#endif
#endif

#if FATFS_USE_WRITEPROTECT_PIN > 0
#ifndef FATFS_USE_WRITEPROTECT_PIN_PIN
#define FATFS_USE_WRITEPROTECT_PIN_RCC		RCC_AHB1Periph_GPIOB			
#define FATFS_USE_WRITEPROTECT_PIN_PORT		GPIOB
#define FATFS_USE_WRITEPROTECT_PIN_PIN		GPIO_Pin_7
#endif
#endif


#endif

