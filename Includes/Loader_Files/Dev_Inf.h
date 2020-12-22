#define    MCU_FLASH   1
#define    NAND_FLASH  2
#define    NOR_FLASH   3
#define    SRAM        4
#define    PSRAM       5
#define    PC_CARD     6
#define    SPI_FLASH   7
#define    I2C_FLASH   8
#define    SDRAM       9
#define    I2C_EEPROM  10

#define SECTOR_NUM 2                  // Max Number of Sector types

// #if defined(STM32F10X_MD)
// #include "stm32f10x.h"
// #define SCL_PIN  12
// #define SCL_PORT B
// #define SDA_PIN  13
// #define SDA_PORT B
// #define RW_PIN   14
// #define RW_PORT  B
// #define LED_PIN  13
// #define LED_PORT C

// #elif defined(STM32L476xx)
// #include "stm32l4xx.h"
// #define SCL_PIN  13
// #define SCL_PORT E
// #define SDA_PIN  14
// #define SDA_PORT E
// #define RW_PIN   15
// #define RW_PORT  E
// #define LED_PIN  2
// #define LED_PORT B
// #else
// #error "Please select first the target STM32F10X_MD or STM32L476xx!"
// #endif


// #if defined(I2C_24C16)
// // Velkost 2048 bytov, 128 stranok po 16 bytov
// #define EE24C_ONE_BYTE_ADDRESS
// #define EE24C_DEVICE_SIZE     (2048u)
// #define EE24C_PROG_PAGE_SIZE  (16u)
// #elif defined(I2C_24C128)
// // Velkost 16384 bytov, 256 stranok po 64 bytov
// #define EE24C_TWO_BYTES_ADDRESS
// #define EE24C_DEVICE_SIZE     (16384)
// #define EE24C_PROG_PAGE_SIZE  (64u)
// #elif defined(I2C_24C256)
// // Velkost 32768 bytov, 512 stranok po 64 bytov
// #define EE24C_TWO_BYTES_ADDRESS
// #define EE24C_DEVICE_SIZE     (32768u)
// #define EE24C_PROG_PAGE_SIZE  (64u)
// #elif defined(I2C_24C512)
// // Velkost 65536 bytov, 512 stranok po 128 bytov
// #define EE24C_TWO_BYTES_ADDRESS
// #define EE24C_DEVICE_SIZE     (65536u)
// #define EE24C_PROG_PAGE_SIZE  (128u)
// #endif


struct DeviceSectors  
{
  unsigned long        SectorNum;     // Number of Sectors
  unsigned long        SectorSize;    // Sector Size in Bytes
};

struct StorageInfo  
{
   char           DeviceName[100];    // Device Name and Description
   unsigned short DeviceType;         // Device Type: ONCHIP, EXT8BIT, EXT16BIT, ...
   unsigned long  DeviceStartAddress; // Default Device Start Address
   unsigned long  DeviceSize;         // Total Size of Device
   unsigned long  PageSize;           // Programming Page Size
   unsigned char  EraseValue;         // Content of Erased Memory
   struct         DeviceSectors sectors[SECTOR_NUM];
};
