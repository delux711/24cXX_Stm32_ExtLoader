#ifndef STM32_I2C_24C02_H
#define STM32_I2C_24C02_H

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

#define HI2C_USED

#if defined(HI2C_USED)
#include "hi2c0.h"

extern bool sFLASH_Init(void);
extern bool sFLASH_ReadBuffer(uint8_t* buffer, uint32_t Address, uint32_t Size);
extern bool sFLASH_WriteBuffer(uint8_t* buffer, uint32_t Address, uint32_t Size);
extern bool sFLASH_EraseBulk(void);
extern bool sFLASH_EraseSector(uint32_t EraseStartAddress ,uint32_t EraseEndAddress);

#else
// ***********************************************************************
// STM32F103_24C02  - derived from UART Menu with GPIO & SysTick support

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_i2c.h"

#define LED_PIN  GPIO_Pin_13
#define LED_PORT GPIOC

#define I2C_EE             I2C1
#define I2C_EE_CLK         RCC_APB1Periph_I2C1
#define I2C_EE_GPIO        GPIOB
#define I2C_EE_GPIO_CLK    RCC_APB2Periph_GPIOB
#define I2C_EE_SCL         GPIO_Pin_8
#define I2C_EE_SDA         GPIO_Pin_9


// ** demo used a ST 24C02 2k(256x8) EEPROM ***
#define I2C_Speed              200000
#define I2C_SLAVE_ADDRESS7     0xA0
#define I2C_FLASH_PAGESIZE     8
#define EEPROM_HW_ADDRESS      0xA2   /* E0=1  E1=0 E2=0 (shift 1 pos. left)*/

#endif

#ifdef __cplusplus
}
#endif
#endif
