#include "Dev_Inf.h"

#define __addVariantPin(addedPins)                              #addedPins
#define _addVariantPin(cPort, cPin, dPort, dPin, rPort, rPin)   __addVariantPin(_SCL_##cPort##cPin##_SDA_##dPort##dPin##_RW_##rPort##rPin)
#define addVariantPin(cPort, cPin, dPort, dPin, rPort, rPin)    _addVariantPin(cPort,cPin,dPort,dPin,rPort,rPin)

/*
Parameters for compiler:
STM32F10X_MD, I2C_24C16,  EE24C_ONE_BYTE_ADDRESS,  EE24C_DEVICE_SIZE=2048,  EE24C_PROG_PAGE_SIZE=16, SCL_PORT=B, SCL_PIN=12, SDA_PORT=B, SDA_PIN=13, RW_PORT=B, RW_PIN=14, LED_PORT=C, LED_PIN=13
STM32F10X_MD, I2C_24C128, EE24C_TWO_BYTES_ADDRESS, EE24C_DEVICE_SIZE=16384, EE24C_PROG_PAGE_SIZE=64, SCL_PORT=B, SCL_PIN=12, SDA_PORT=B, SDA_PIN=13, RW_PORT=B, RW_PIN=14, LED_PORT=C, LED_PIN=13
STM32F10X_MD, I2C_24C256, EE24C_TWO_BYTES_ADDRESS, EE24C_DEVICE_SIZE=32768, EE24C_PROG_PAGE_SIZE=64, SCL_PORT=B, SCL_PIN=12, SDA_PORT=B, SDA_PIN=13, RW_PORT=B, RW_PIN=14, LED_PORT=C, LED_PIN=13
STM32F10X_MD, I2C_24C512, EE24C_TWO_BYTES_ADDRESS, EE24C_DEVICE_SIZE=65536, EE24C_PROG_PAGE_SIZE=128,SCL_PORT=B, SCL_PIN=12, SDA_PORT=B, SDA_PIN=13, RW_PORT=B, RW_PIN=14, LED_PORT=C, LED_PIN=13

STM32L476xx,  I2C_24C16,  EE24C_ONE_BYTE_ADDRESS,  EE24C_DEVICE_SIZE=2048,  EE24C_PROG_PAGE_SIZE=16, SCL_PORT=E, SCL_PIN=13, SDA_PORT=E, SDA_PIN=14, RW_PORT=E, RW_PIN=15, LED_PORT=B, LED_PIN=2
STM32L476xx,  I2C_24C128, EE24C_TWO_BYTES_ADDRESS, EE24C_DEVICE_SIZE=16384, EE24C_PROG_PAGE_SIZE=64, SCL_PORT=E, SCL_PIN=13, SDA_PORT=E, SDA_PIN=14, RW_PORT=E, RW_PIN=15, LED_PORT=B, LED_PIN=2
STM32L476xx,  I2C_24C256, EE24C_TWO_BYTES_ADDRESS, EE24C_DEVICE_SIZE=32768, EE24C_PROG_PAGE_SIZE=64, SCL_PORT=E, SCL_PIN=13, SDA_PORT=E, SDA_PIN=14, RW_PORT=E, RW_PIN=15, LED_PORT=B, LED_PIN=2
STM32L476xx,  I2C_24C512, EE24C_TWO_BYTES_ADDRESS, EE24C_DEVICE_SIZE=65536, EE24C_PROG_PAGE_SIZE=128,SCL_PORT=E, SCL_PIN=13, SDA_PORT=E, SDA_PIN=14, RW_PORT=E, RW_PIN=15, LED_PORT=B, LED_PIN=2
*/

/* This structure containes information used by ST-LINK Utility to program and erase the device */
#if defined (__ICCARM__)
__root struct StorageInfo const StorageInfo  =  {
#else
struct StorageInfo const StorageInfo  =  {
#endif
    #if defined(I2C_24C16)  //"24c16_STM32F103", // Device Name + version number
        "24c16_"
    #elif defined(I2C_24C128)
        "24c128_"
    #elif defined(I2C_24C256)
        "24c256_"
    #elif defined(I2C_24C512)
        "24c512_"
	
    #else
    #error "Please select first the target I2C_24C16, I2C_24C128, I2C_24C256 or I2C_24C512!"
    #endif
    #if defined(STM32F10X_MD)
       "STM32F103"
    #elif defined(STM32L476xx)
       "STM32L476"
    #else
    #error "Please select first the target STM32F10X_MD or STM32L476xx!"
    #endif
    addVariantPin(SCL_PORT, SCL_PIN, SDA_PORT, SDA_PIN, RW_PORT, RW_PIN),   
    I2C_EEPROM,                  // Device Type
    0x00000000,                  // Device Start Address
    EE24C_DEVICE_SIZE,           // Device Size in Bytes
                                 // 24c16  =  2kBbyte
                                 // 24c128 = 16kB
                                 // 24c256 = 32kB
                                 // 24c512 = 65kB
    EE24C_PROG_PAGE_SIZE,        // Programming Page Size
                                 // 24c16  = 16 byte
                                 // 24c128 = 64 byte
                                 // 24c256 = 64 byte
                                 // 24c512 = 128 byte
    0xFF,                        // Initial Content of Erased Memory
    // Specify Size and Address of Sectors (view example below)
    // 24C16  = 128, 16,         // 128 sektorov po 16 bytov = 2kB
    // 24C128 = 256, 64,         // 256 sektorov po 64 bytov = 16kB
    // 24C256 = 512, 64,         // 512 sektorov po 64 bytov = 32kB
    // 24C512 = 512, 128,        // 512 sektorov po 128 bytov= 65kB
    (EE24C_DEVICE_SIZE / EE24C_PROG_PAGE_SIZE), EE24C_PROG_PAGE_SIZE,
    0x00000000, 0x00000000
}; 

/*                                  Sector coding example
    A device with succives 16 Sectors of 1KBytes, 128 Sectors of 16 KBytes, 
    8 Sectors of 2KBytes and 16384 Sectors of 8KBytes
    
    0x00000010, 0x00000400,                                 // 16 Sectors of 1KBytes
    0x00000080, 0x00004000,                                 // 128 Sectors of 16 KBytes
    0x00000008, 0x00000800,                                 // 8 Sectors of 2KBytes
    0x00004000, 0x00002000,                                 // 16384 Sectors of 8KBytes
    0x00000000, 0x00000000,                                // end
  */
