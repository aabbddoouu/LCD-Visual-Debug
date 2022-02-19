//#include "stm32f4xx.h"
#include "defines.h"
#include "diskio.h"

/**
 * @defgroup TM_FATFS_SPI_FLASH_Macros
 * @brief    Library defines
 * @{
 */

/* Sector size on flash memory */
#ifndef FATFS_SPI_FLASH_SECTOR_SIZE
#define FATFS_SPI_FLASH_SECTOR_SIZE    512
#endif

/* Memory size on flash */
#ifndef FATFS_SPI_FLASH_MEMORY_SIZE
#define FATFS_SPI_FLASH_MEMORY_SIZE    512
#endif

DSTATUS TM_FATFS_SPI_FLASH_disk_initialize(void);
DSTATUS TM_FATFS_SPI_FLASH_disk_status(void);
DRESULT TM_FATFS_SPI_FLASH_disk_ioctl(BYTE cmd, void *buff);
DRESULT TM_FATFS_SPI_FLASH_disk_read(BYTE *buff, DWORD sector, UINT count);
DRESULT TM_FATFS_SPI_FLASH_disk_write(const BYTE *buff, DWORD sector, UINT count);


/* C++ detection */
#ifdef __cplusplus
}
#endif