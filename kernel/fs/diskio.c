/* This file use for NCTU OSDI course */
#include <fs.h>
#include <fat/diskio.h>
#include <fat/ff.h>
#include <kernel/drv/disk.h>

/*  Lab7, low level file operator.
 *  You have to provide some device control interface for 
 *  FAT File System Module to communicate with the disk.
 *
 *  Use the function under kernel/drv/disk.c to finish
 *  this part. You can also add some functions there if you
 *  need.
 *
 *  FAT File System Module reference document is under the
 *  doc directory (doc/00index_e.html)
 *
 *  Note:
 *  Since we only use primary slave disk as our file system,
 *  please ignore the pdrv parameter in below function and
 *  just manipulate the hdb disk.
 *
 *  Call flow example:
 *        ┌──────────────┐
 *        │     open     │
 *        └──────────────┘
 *               ↓
 *        ┌──────────────┐
 *        │   sys_open   │  file I/O system call interface
 *        └──────────────┘
 *               ↓
 *        ┌──────────────┐
 *        │  file_open   │  VFS level file API
 *        └──────────────┘
 *               ↓
 *        ┌──────────────┐
 *        │   fat_open   │  fat level file operator
 *        └──────────────┘
 *               ↓
 *        ┌──────────────┐
 *        │    f_open    │  FAT File System Module
 *        └──────────────┘
 *               ↓
 *        ╔══════════════╗
 *   ==>  ║    diskio    ║  low level file operator
 *        ╚══════════════╝
 *               ↓
 *        ┌──────────────┐
 *        │     disk     │  simple ATA disk dirver
 *        └──────────────┘
 */

#define DISK_ID 1

/**
  * @brief  Initial IDE disk
  * @param  pdrv: Physical drive number
  * @retval disk error status
  *         - 0: Initial success
  *         - STA_NOINIT: Intial failed
  *         - STA_PROTECT: Medium is write protected
  */
DSTATUS disk_initialize (BYTE pdrv)
{
  /* TODO */
  /* Note: You can create a function under disk.c  
   *       to help you get the disk status.
   */
    disk_init();
    return 0;
}

/**
  * @brief  Get disk current status
  * @param  pdrv: Physical drive number
  * @retval disk status
  *         - 0: Normal status
  *         - STA_NOINIT: Device is not initialized and not ready to work
  *         - STA_PROTECT: Medium is write protected
  */
DSTATUS disk_status (BYTE pdrv)
{
/* TODO */
/* Note: You can create a function under disk.c  
 *       to help you get the disk status.
 */
    return 0;
}

/**
  * @brief  Read serval sector form a IDE disk
  * @param  pdrv: Physical drive number
  * @param  buff: destination memory start address
  * @param  sector: start sector number
  * @param  count: number of sector
  * @retval Results of Disk Functions (See diskio.h)
  *         - RES_OK: success
  *         - < 0: failed
  */
DRESULT disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
    int err = 0;
    int i = count;
    BYTE *ptr = buff;
    UINT cur_sector = sector;

    for (; i > 0; i -= 256) {
        int cur_count = (i % 256) ? i % 256 : 256;
        int ret = ide_read_sectors(DISK_ID, i % 256, cur_sector, (unsigned int)buff);
        if (ret < 0) return ret;

        cur_sector += cur_count;
        buff += cur_count * 512;
    }
    return RES_OK;
}

/**
  * @brief  Write serval sector to a IDE disk
  * @param  pdrv: Physical drive number
  * @param  buff: memory start address
  * @param  sector: destination start sector number
  * @param  count: number of sector
  * @retval Results of Disk Functions (See diskio.h)
  *         - RES_OK: success
  *         - < 0: failed
  */
DRESULT disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count)
{
    int err = 0;
    int i = count;
    BYTE *ptr = buff;
    UINT cur_sector = sector;

    for (; i > 0; i -= 256) {
        int cur_count = (i % 256) ? i % 256 : 256;
        int ret = ide_write_sectors(DISK_ID, i % 256, cur_sector, (unsigned int)buff);
        if (ret < 0) return ret;

        cur_sector += cur_count;
        buff += cur_count * 512;
    }
    return RES_OK;
}

/**
  * @brief  Get disk information form disk
  * @param  pdrv: Physical drive number
  * @param  cmd: disk control command (See diskio.h)
  *         - GET_SECTOR_COUNT
  *         - GET_BLOCK_SIZE (Same as sector size)
  * @param  buff: return memory space
  * @retval Results of Disk Functions (See diskio.h)
  *         - RES_OK: success
  *         - < 0: failed
  */
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff)
{
    uint32_t *retVal = (uint32_t *)buff;

    switch (cmd) {
    case GET_SECTOR_COUNT:
        *retVal = ide_devices[DISK_ID].Size;
        break;
    case GET_BLOCK_SIZE:
        *retVal = 512;
        break;
    }
    return RES_OK;
}

/**
  * @brief  Get OS timestamp
  * @retval tick of CPU
  */
DWORD get_fattime (void)
{
    return sys_get_ticks();
}
