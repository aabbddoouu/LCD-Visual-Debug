/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "ff.h"

/* Not USB in use */
/* Define it in defines.h project file if you want to use USB */
#ifndef FATFS_USE_USB
	#define FATFS_USE_USB				0
#endif

/* Not SDRAM in use */
/* Define it in defines.h project file if you want to use SDRAM */
#ifndef FATFS_USE_SDRAM
	#define FATFS_USE_SDRAM				0
#endif

/* Not SPI FLASH in use */
/* Define it in defines.h project file if you want to use SPI FLASH */
#ifndef FATFS_USE_SPI_FLASH
	#define FATFS_USE_SPI_FLASH			0
#endif

/* Set in defines.h file if you want it */
#ifndef TM_FATFS_CUSTOM_FATTIME
	#define TM_FATFS_CUSTOM_FATTIME		0
#endif

/* Defined in defines.h */
/* We are using FATFS with SPI */
#if FATFS_USE_USB == 1 || FATFS_USE_SDRAM == 1 || FATFS_USE_SPI_FLASH == 1
	/* If SDIO is not defined, set to 2, to disable SD card */
	/* You can set FATFS_USE_SDIO in defines.h file */
	/* This is for error fixes */
	#ifndef FATFS_USE_SDIO
		#define FATFS_USE_SDIO			2
	#endif
#else
	/* If SPI is not used, set default settings back */
	/* By default, SDIO is used */
	#ifndef FATFS_USE_SDIO
		#define FATFS_USE_SDIO			1
	#endif
#endif


/* USB with FATFS */
#if FATFS_USE_USB == 1
	#include "fatfs_usb.h"
#endif /* FATFS_USE_USB */
/* SDRAM with FATFS */
#if FATFS_USE_SDRAM == 1
	#include "fatfs_sdram.h"
#endif /* FATFS_USE_SDRAM */
/* SPI FLASH with FATFS */
#if FATFS_USE_SPI_FLASH == 1
	#include "fatfs_spi_flash.h"
#endif /* FATFS_USE_SPI_FLASH */

/* Include SD card files if is enabled */
#if FATFS_USE_SDIO == 1
	#include "fatfs_sd_sdio.h"
#elif FATFS_USE_SDIO == 0
	#include "fatfs_sd.h"
#endif


/* Definitions of physical drive number for each media */
#define ATA		   0
#define USB		   1
#define SDRAM      2
#define SPI_FLASH  3

/* Make driver structure */
DISKIO_LowLevelDriver_t FATFS_LowLevelDrivers[FF_VOLUMES] = {
	{
#if FATFS_USE_SDIO == 1
		SD_SDIO_disk_initialize,
		SD_SDIO_disk_status,
		SD_SDIO_disk_ioctl,
		SD_SDIO_disk_write,
		SD_SDIO_disk_read
#else
		SD_disk_initialize,
		SD_disk_status,
		SD_disk_ioctl,
		SD_disk_write,
		SD_disk_read
#endif
	},
	{
		USB_disk_initialize,
		USB_disk_status,
		USB_disk_ioctl,
		USB_disk_write,
		USB_disk_read
	},
	{
		SDRAM_disk_initialize,
		SDRAM_disk_status,
		SDRAM_disk_ioctl,
		SDRAM_disk_write,
		SDRAM_disk_read
	},
	{
		SPI_FLASH_disk_initialize,
		SPI_FLASH_disk_status,
		SPI_FLASH_disk_ioctl,
		SPI_FLASH_disk_write,
		SPI_FLASH_disk_read
	}
};


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	/* Return low level status */
	if (FATFS_LowLevelDrivers[pdrv].disk_initialize) {
		return FATFS_LowLevelDrivers[pdrv].disk_initialize();
	}
	
	/* Return parameter error */
	return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{
	/* Return low level status */
	if (FATFS_LowLevelDrivers[pdrv].disk_status) {
		return FATFS_LowLevelDrivers[pdrv].disk_status();
	}
	
	/* Return parameter error */
	return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	/* Check count */
	if (!count) {
		return RES_PARERR;
	}
	
	/* Return low level status */
	if (FATFS_LowLevelDrivers[pdrv].disk_read) {
		return FATFS_LowLevelDrivers[pdrv].disk_read(buff, sector, count);
	}
	
	/* Return parameter error */
	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	/* Check count */
	if (!count) {
		return RES_PARERR;
	}
	
	/* Return low level status */
	if (FATFS_LowLevelDrivers[pdrv].disk_write) {
		return FATFS_LowLevelDrivers[pdrv].disk_write(buff, sector, count);
	}
	
	/* Return parameter error */
	return RES_PARERR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	/* Return low level status */
	if (FATFS_LowLevelDrivers[pdrv].disk_ioctl) {
		return FATFS_LowLevelDrivers[pdrv].disk_ioctl(cmd, buff);
	}
	
	/* Return parameter error */
	return RES_PARERR;
}
#endif

/*-----------------------------------------------------------------------*/
/* Get time for fatfs for files                                          */
/*-----------------------------------------------------------------------*/
__weak DWORD get_fattime(void) {
	/* Returns current time packed into a DWORD variable */
	return	  ((DWORD)(2013 - 1980) << 25)	/* Year 2013 */
			| ((DWORD)7 << 21)				/* Month 7 */
			| ((DWORD)28 << 16)				/* Mday 28 */
			| ((DWORD)0 << 11)				/* Hour 0 */
			| ((DWORD)0 << 5)				/* Min 0 */
			| ((DWORD)0 >> 1);				/* Sec 0 */
}

/* Function declarations to prevent link errors if functions are not found */
__weak DSTATUS SD_SDIO_disk_initialize(void) {return RES_ERROR;}
__weak DSTATUS SD_disk_initialize(void) {return RES_ERROR;}
__weak DSTATUS USB_disk_initialize(void) {return RES_ERROR;}
__weak DSTATUS SDRAM_disk_initialize(void) {return RES_ERROR;}
__weak DSTATUS SPI_FLASH_disk_initialize(void) {return RES_ERROR;}

__weak DSTATUS SD_SDIO_disk_status(void) {return RES_ERROR;}
__weak DSTATUS SD_disk_status(void) {return RES_ERROR;}
__weak DSTATUS USB_disk_status(void) {return RES_ERROR;}
__weak DSTATUS SDRAM_disk_status(void) {return RES_ERROR;}
__weak DSTATUS SPI_FLASH_disk_status(void) {return RES_ERROR;}

__weak DRESULT SD_SDIO_disk_ioctl(BYTE cmd, void *buff) {return (DRESULT)STA_NOINIT;}
__weak DRESULT SD_disk_ioctl(BYTE cmd, void *buff) {return (DRESULT)STA_NOINIT;}
__weak DRESULT USB_disk_ioctl(BYTE cmd, void *buff) {return (DRESULT)STA_NOINIT;}
__weak DRESULT SDRAM_disk_ioctl(BYTE cmd, void *buff) {return (DRESULT)STA_NOINIT;}
__weak DRESULT SPI_FLASH_disk_ioctl(BYTE cmd, void *buff) {return (DRESULT)STA_NOINIT;}

__weak DRESULT SD_SDIO_disk_read(BYTE *buff, DWORD sector, UINT count) {return (DRESULT)STA_NOINIT;}
__weak DRESULT SD_disk_read(BYTE *buff, DWORD sector, UINT count) {return (DRESULT)STA_NOINIT;}
__weak DRESULT USB_disk_read(BYTE *buff, DWORD sector, UINT count) {return (DRESULT)STA_NOINIT;}
__weak DRESULT SDRAM_disk_read(BYTE *buff, DWORD sector, UINT count) {return (DRESULT)STA_NOINIT;}
__weak DRESULT SPI_FLASH_disk_read(BYTE *buff, DWORD sector, UINT count) {return (DRESULT)STA_NOINIT;}

__weak DRESULT SD_SDIO_disk_write(const BYTE *buff, DWORD sector, UINT count) {return (DRESULT)STA_NOINIT;}
__weak DRESULT SD_disk_write(const BYTE *buff, DWORD sector, UINT count) {return (DRESULT)STA_NOINIT;}
__weak DRESULT USB_disk_write(const BYTE *buff, DWORD sector, UINT count) {return (DRESULT)STA_NOINIT;}
__weak DRESULT SDRAM_disk_write(const BYTE *buff, DWORD sector, UINT count) {return (DRESULT)STA_NOINIT;}
__weak DRESULT SPI_FLASH_disk_write(const BYTE *buff, DWORD sector, UINT count) {return (DRESULT)STA_NOINIT;}
