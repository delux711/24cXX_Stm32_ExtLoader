#include "Dev_Inf.h"

/* This structure containes information used by ST-LINK Utility to program and erase the device */
#if defined (__ICCARM__)
__root struct StorageInfo const StorageInfo  =  {
#else
struct StorageInfo const StorageInfo  =  {
#endif
#if defined(I2C_24C16)
   "24c16_STM32F103",//"M25P64_STM3210E-EVAL", 	 	 // Device Name + version number
   I2C_EEPROM,// SPI_FLASH         					 // Device Type
   0x00000000,                						 // Device Start Address
   0x800,// 0x00800000,                 						 // Device Size in Bytes (2kBytes)
   0x00000001,                 						 // Programming Page Size 16Bytes
   0xFF,                       						 // Initial Content of Erased Memory
// Specify Size and Address of Sectors (view example below)
   0x00000010, 0x00000080,     				 // Sector Num : 16 ,Sector Size: 128Bytes 
   0x00000000, 0x00000000
}; 
#elif defined(I2C_24C512)
   "24c512_STM32F103",                      	 	 // Device Name + version number
   I2C_EEPROM,                   					 // Device Type
   0x00000000,                						 // Device Start Address
   0x80000,                 						 // Device Size in Bytes (512kBytes)
   0x0000080,                 						 // Programming Page Size 16Bytes
   0xFF,                       						 // Initial Content of Erased Memory
// Specify Size and Address of Sectors (view example below)
   0x00001000, 0x00000080,     				 // Sector Num : 16 ,Sector Size: 128Bytes 
   0x00000000, 0x00000000
};
#endif
/*  								Sector coding example
	A device with succives 16 Sectors of 1KBytes, 128 Sectors of 16 KBytes, 
	8 Sectors of 2KBytes and 16384 Sectors of 8KBytes
	
	0x00000010, 0x00000400,     							// 16 Sectors of 1KBytes
	0x00000080, 0x00004000,     							// 128 Sectors of 16 KBytes
	0x00000008, 0x00000800,     							// 8 Sectors of 2KBytes
	0x00004000, 0x00002000,     							// 16384 Sectors of 8KBytes
	0x00000000, 0x00000000,								// end
  */
