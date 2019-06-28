#include "stm32_i2c_24c02.h"
#include "stm32f10x.h"
#include "hi2c0.h"
//#include "stm32f10x_i2c.h"

#define GPIO_CRH_MODE14_Pos ((14 - 8) * 4)
#define PIN_OUTPUT          (2u)

/*
static inline void sFlash_WriteEnable(void)  { GPIOB->BSRR |= GPIO_BSRR_BR14; }
static inline void sFlash_WriteDisable(void) { GPIOB->BSRR |= GPIO_BSRR_BS14; }
*/
bool sFLASH_Init(void) {
    bool ret;
    uint32_t config;
    HI2C_Struct s = { false, false, 0xA0u, 0u, 0u};
    
    
    // pin WP
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;      // enable clock for GPIOx
    config = (GPIOB->CRH & (~(GPIO_CRH_MODE14 | GPIO_CRH_CNF14)));
    config |= (uint32_t)((uint32_t)PIN_OUTPUT << GPIO_CRH_MODE14_Pos);
    GPIOB->BSRR |= GPIO_BSRR_BS14; //sFlash_WriteDisable();
    GPIOB->CRH = config;

    HI2C0_vInit(0xA0u, &s);
    ret = HI2C0_bSetAddr(HI2C0_getChipAddress(&s), &s);
    (void)ret;
    //ret = HI2C0_bSetAddr(0xa0);
    HI2C0_vMakeStopCondition(&s);
    return true;
}

bool sFLASH_ReadBuffer(uint8_t* buffer, uint32_t Address, uint32_t Size) {
    bool ret = true;
    bool i2cStop = false;
    uint8_t temp;
    HI2C_Struct s = { false, false, 0xA0u, 0u, 0u};
    if(GPIOC->ODR & GPIO_ODR_ODR13)
        GPIOC->BSRR |= GPIO_BSRR_BR13;
    else
        GPIOC->BSRR |= GPIO_BSRR_BS13;
    if(0u < Size) {
        if(Size <= 1) {
            i2cStop = true;
        } else {
            i2cStop = false;
        }
        temp = HI2C0_readByte((uint8_t) Address, i2cStop, &s);
        if(HI2C0_isChipPresent(&s)) {
            *buffer++ = temp;
            while(--Size) {
                if(Size <= 1) {
                    i2cStop = true;
                }
                *buffer++ = HI2C0_vTriggerReceive(i2cStop, &s);
            }
        }
    }
    (void)ret;
    //return ret;
    return true;
}

bool sFLASH_WriteBuffer(uint8_t* buffer, uint32_t Address, uint32_t Size) {
    bool ret = false;
    bool i2cStop = false;
    //sFLASH_Init();
    HI2C_Struct s = { false, false, 0xA0u, 0u, 0u};
    GPIOB->BSRR |= GPIO_BSRR_BR14; //sFlash_WriteEnable();
    if(0 < Size) {
        if(Size <= 1) {
            i2cStop = true;
        } else {
            i2cStop = false;
        }
        ret = HI2C0_writeByte((uint8_t) Address, i2cStop, *buffer++, &s);
        if(ret) {
            while(--Size) {
                if(Size <= 1) {
                    i2cStop = true;
                }
                ret = HI2C0_bSetTxData(*buffer++, i2cStop, &s);
            }
        }
    }
    return ret;
}

bool sFLASH_EraseBulk(void) {
    HI2C_Struct s = { false, false, 0xA0u, 0u, 0u};
    if(true == HI2C0_bSetAddr((0x40 | 0x01u), &s)) { // read
        (void)HI2C0_vTriggerReceive(true, &s);
    }
    return true;
}

bool sFLASH_EraseSector(uint32_t EraseStartAddress ,uint32_t EraseEndAddress) {
    HI2C_Struct s = { false, false, 0xA0u, 0u, 0u};
    if(true == HI2C0_bSetAddr((0x50 | 0x01u), &s)) { // read
        (void)HI2C0_vTriggerReceive(true, &s);
    }
    return true;
}

/*
void I2C_Configuration(void)
{
    I2C_InitTypeDef  I2C_InitStructure;

    // I2C configuration
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = I2C_SLAVE_ADDRESS7;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;

    // I2C Peripheral Enable
    I2C_Cmd(I2C_EE, ENABLE);
    // Apply I2C configuration after enabling it
    I2C_Init(I2C_EE, &I2C_InitStructure);
}
*/
