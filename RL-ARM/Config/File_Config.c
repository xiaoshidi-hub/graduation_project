/*----------------------------------------------------------------------------
 *      RL-ARM - FlashFS
 *----------------------------------------------------------------------------
 *      Name:    FILE_CONFIG.C
 *      Purpose: Configuration of RL FlashFS by user
 *      Rev.:    V4.70
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2013 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <File_Config.h>


#define N_FILES     8


#define FAT_NAME_CACNT   50


#define FAT_NAME_RELOC   0


#define FAT_NAME_CADR    0x60000000


#define CPU_CLK     168000000


#define FL0_EN      0


#define FL0_BADR    0x80000000


#define FL0_SIZE    0x0200000


#define FL0_INITV   0xFF


#define FL0_HFILE   "FS_FlashDev.h"


#define FL0_DEF     0


#define SF0_EN      0


#define SF0_SIZE    0x0200000


#define SF0_INITV   0xFF


#define SF0_HFILE   "FS_SPI_FlashDev.h"


#define SF0_DEF     0


#define RAM0_EN     0


#define RAM0_SIZE   0x004000


#define RAM0_NSECT  64


#define RAM0_RELOC  1


#define RAM0_BADR   0x81010000


#define RAM0_DEF    0


#define MC0_EN      1


#define MC0_SPI     0
          

#define MC0_CASZ    8

//   <e>Relocate Cache Buffer
//   <i>Locate Cache Buffer at a specific address.
//   <i>Some devices like NXP LPC23xx require a Cache buffer
//   <i>for DMA transfer located at specific address.
#define MC0_RELOC   0

//   <o>Base address <0x0000-0xFFFFFE00:0x200>
//   <i>Define the Cache buffer base address.
//   <i>For LPC23xx/24xx devices this is USB RAM
//   <i>starting at 0x7FD00000.
#define MC0_CADR    0x7FD00000

//   </e>
//   <q>FAT Journal
//   <i>Enable FAT Journal in order to guarantee
//   <i>fail-safe FAT file system operation.
#define MC0_FSJ     0

//   <q>Default Drive [M0:]
//   <i>Used when Drive letter not specified
#define MC0_DEF     1

// </e>
// <e>Memory Card Drive 1
// ======================
// <i>Enable Memory Card Drive  [M1:]
#define MC1_EN      0

//   <o>Bus Mode <0=>SD-Native <1=>SPI
//   <i>Define Memory Card bus interface mode.
//   <i>SD-Native mode needs MCI peripheral.
//   <i>SPI mode uses SD Card in SPI mode.
#define MC1_SPI     1
          
//   <o>File System Cache <0=>OFF <1=>1 KB <2=>2 KB <4=>4 KB
//                        <8=>8 KB <16=>16 KB <32=>32 KB
//   <i>Define System Cache buffer size for file IO. 
//   <i>Increase this number for faster r/w access.
//   <i>Default: 4 kB
#define MC1_CASZ    0

//   <e>Relocate Cache Buffer
//   <i>Locate Cache Buffer at a specific address.
//   <i>Some devices like NXP LPC23xx require a Cache buffer
//   <i>for DMA transfer located at specific address.
#define MC1_RELOC   0

//   <o>Base address <0x0000-0xFFFFFE00:0x200>
//   <i>Define the Cache buffer base address.
//   <i>For LPC23xx/24xx devices this is USB RAM
//   <i>starting at 0x7FD00000.
#define MC1_CADR    0x7FD00000

//   </e>
//   <q>FAT Journal
//   <i>Enable FAT Journal in order to guarantee
//   <i>fail-safe FAT file system operation.
#define MC1_FSJ     0

//   <q>Default Drive [M1:]
//   <i>Used when Drive letter not specified
#define MC1_DEF     0

// </e>
// <e>USB Flash Drive 0
// ====================
// <i>Enable USB Flash Drive  [U0:]
#define USB0_EN     0

//   <o>File System Cache <0=>OFF <1=>1 KB <2=>2 KB <4=>4 KB
//                        <8=>8 KB <16=>16 KB <32=>32 KB
//   <i>Define System Cache buffer size for file IO. 
//   <i>Increase this number for faster r/w access.
//   <i>Default: 4 kB
#define USB0_CASZ   8

//   <q>FAT Journal
//   <i>Enable FAT Journal in order to guarantee
//   <i>fail-safe FAT file system operation.
#define USB0_FSJ    0

//   <q>Default Drive [U0:]
//   <i>Used when Drive letter not specified
#define USB0_DEF    0

// </e>
// <e>USB Flash Drive 1
// ====================
// <i>Enable USB Flash Drive  [U1:]
#define USB1_EN     0

//   <o>File System Cache <0=>OFF <1=>1 KB <2=>2 KB <4=>4 KB
//                        <8=>8 KB <16=>16 KB <32=>32 KB
//   <i>Define System Cache buffer size for file IO. 
//   <i>Increase this number for faster r/w access.
//   <i>Default: 4 kB
#define USB1_CASZ   8

//   <q>FAT Journal
//   <i>Enable FAT Journal in order to guarantee
//   <i>fail-safe FAT file system operation.
#define USB1_FSJ    0

//   <q>Default Drive [U1:]
//   <i>Used when Drive letter not specified
#define USB1_DEF    0

// </e>
// <e>NAND Flash Drive 0
// ===================
// <i>Enable NAND Flash Drive  [N0:]
#define NAND0_EN    0

//   <o>Page size  <528=> 512 + 16 bytes
//                 <2112=>2048 + 64 bytes
//                 <4224=>4096 + 128 bytes
//                 <8448=>8192 + 256 bytes
//   <i>Define program Page size in bytes (User + Spare area).
#define NAND0_PGSZ  2112

//   <o>Block Size <8=>8 pages <16=>16 pages <32=>32 pages
//                 <64=>64 pages <128=>128 pages <256=>256 pages
//   <i>Define number of pages in a block.
#define NAND0_PGCNT 64

//   <o>Device Size [blocks] <512-32768>
//   <i>Define number of blocks in NAND Flash device.
#define NAND0_BLCNT 4096

//   <o>Page Caching <0=>OFF <1=>1 page <2=>2 pages <4=>4 pages
//                   <8=>8 pages <16=>16 pages <32=>32 pages
//   <i>Define number of cached Pages.
//   <i>Default: 4 pages
#define NAND0_CAPG  2

//   <o>Block Indexing <0=>OFF <1=>1 block <2=>2 blocks <4=>4 blocks
//                     <8=>8 blocks <16=>16 blocks <32=>32 blocks
//                     <64=>64 blocks <128=>128 blocks <256=>256 blocks
//   <i>Define number of indexed Flash Blocks.
//   <i>Increase this number for better performance.
//   <i>Default: 16 blocks
#define NAND0_CABL  16

//   <o>Software ECC <0=>None <1=>Hamming (SLC)
//   <i>Enable software ECC calculation only,
//   <i>if not supported by hardware.
#define NAND0_SWECC 1

//   <o>File System Cache <0=>OFF <1=>1 KB <2=>2 KB <4=>4 KB
//                        <8=>8 KB <16=>16 KB <32=>32 KB
//   <i>Define System Cache buffer size for file IO. 
//   <i>Increase this number for faster r/w access.
//   <i>Default: 4 kB
#define NAND0_CASZ  4

//   <e>Relocate Cache Buffers
//   <i>Use this option to locate Cache buffers 
//   <i>at specific address in RAM or SDRAM.
#define NAND0_RELOC 0

//     <o>Base address <0x0000-0xFFFFFE00:0x200>
//     <i>Define base address for Cache Buffers.
#define NAND0_CADR  0x80000000

//   </e>
//   <q>FAT Journal
//   <i>Enable FAT Journal in order to guarantee
//   <i>fail-safe FAT file system operation.
#define NAND0_FSJ     0

//   <q>Default Drive [N0:]
//   <i>Used when Drive letter not specified
#define NAND0_DEF   0

// </e>
// <e>NAND Flash Drive 1
// ===================
// <i>Enable NAND Flash Drive  [N1:]
#define NAND1_EN    0

//   <o>Page size  <528=> 512 + 16 bytes
//                 <2112=>2048 + 64 bytes
//                 <4224=>4096 + 128 bytes
//                 <8448=>8192 + 256 bytes
//   <i>Define program Page size in bytes (User + Spare area).
#define NAND1_PGSZ  2112

//   <o>Block Size <8=>8 pages <16=>16 pages <32=>32 pages
//                 <64=>64 pages <128=>128 pages <256=>256 pages
//   <i>Define number of pages in a block.
#define NAND1_PGCNT 32

//   <o>Device Size [blocks] <512-32768>
//   <i>Define number of blocks in NAND Flash device.
#define NAND1_BLCNT 512

//   <o>Page Caching <0=>OFF <1=>1 page <2=>2 pages <4=>4 pages
//                   <8=>8 pages <16=>16 pages <32=>32 pages
//   <i>Define number of cached Pages.
//   <i>Default: 4 pages
#define NAND1_CAPG  4

//   <o>Block Indexing <0=>OFF <1=>1 block <2=>2 blocks <4=>4 blocks
//                     <8=>8 blocks <16=>16 blocks <32=>32 blocks
//                     <64=>64 blocks <128=>128 blocks <256=>256 blocks
//   <i>Define number of indexed Flash Blocks.
//   <i>Increase this number for better performance.
//   <i>Default: 16 blocks
#define NAND1_CABL  16

//   <o>Software ECC <0=>None <1=>Hamming (SLC)
//   <i>Enable software ECC calculation only,
//   <i>if not supported by hardware.
#define NAND1_SWECC 0

//   <o>File System Cache <0=>OFF <1=>1 KB <2=>2 KB <4=>4 KB
//                        <8=>8 KB <16=>16 KB <32=>32 KB
//   <i>Define System Cache buffer size for file IO.
//   <i>Increase this number for faster r/w access.
//   <i>Default: 4 kB
#define NAND1_CASZ  4

//   <e>Relocate Cache Buffers
//   <i>Use this option to locate Cache buffers
//   <i>at specific address in RAM or SDRAM.
#define NAND1_RELOC 0

//     <o>Base address <0x0000-0xFFFFFE00:0x200>
//     <i>Define base address for Cache Buffers.
#define NAND1_CADR  0x80000000

//   </e>
//   <q>FAT Journal
//   <i>Enable FAT Journal in order to guarantee
//   <i>fail-safe FAT file system operation.
#define NAND1_FSJ     0

//   <q>Default Drive [N1:]
//   <i>Used when Drive letter not specified
#define NAND1_DEF   0

// </e>

//------------- <<< end of configuration section >>> -----------------------

#ifndef  __NO_FILE_LIB_C
#include <File_lib.c>
#endif

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
