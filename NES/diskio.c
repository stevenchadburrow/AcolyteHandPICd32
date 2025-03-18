/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{	
	//return STA_NOINIT;
	return 0;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{	
	int test = 0;

	for (int i=0; i<5; i++)
	{
		test = sdcard_initialize();

		if (test == 1) break;
	}
	
	if (test == 1) return 0;
	else return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	int test = 0;
	
	for (unsigned int i=0; i<count; i++)
	{
		test = sdcard_readblock((unsigned int)((sector>>15)&0xFFFF), (unsigned int)((sector<<1)&0xFFFF));

		if (test == 0) break;

		for (unsigned int j=0; j<512; j++)
		{
			buff[i * 512 + j] = sdcard_block[j];
		}
	}

	if (test == 1) return 0;
	else return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	int test = 0;
	
	for (unsigned int i=0; i<count; i++)
	{
		for (unsigned int j=0; j<512; j++)
		{
			sdcard_block[j] = buff[i * 512 + j];
		}
		
		test = sdcard_writeblock((unsigned int)((sector>>15)&0xFFFF), (unsigned int)((sector<<1)&0xFFFF));

		if (test == 0) break;
	}
	
	if (test == 1) return 0;
	else return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{	
	if (cmd == CTRL_SYNC)
	{
		buff = 0;
		return 0;
	}
	else if (cmd == GET_SECTOR_COUNT)
	{
		buff = 0;
		return 0;
	}
	else if (cmd == GET_SECTOR_SIZE)
	{
		buff = 0;
		return 0;
	}
	else if (cmd == GET_BLOCK_SIZE)
	{
		buff = 0;
		return 0;
	}
	else if (cmd == CTRL_TRIM)
	{
		buff = 0;
		return 0;
	}
	
	//return RES_PARERR;
	return 0;
}

