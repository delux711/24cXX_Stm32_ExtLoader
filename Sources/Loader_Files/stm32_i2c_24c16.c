#include "stm32_i2c_24c16.h"
#include "stm32f10x.h"
#include "hi2c0.h"
//#include "stm32f10x_i2c.h"

#define GPIO_CRH_MODE14_Pos ((14 - 8) * 4)
#define PIN_OUTPUT          (2u)

bool sFlash_isReady(HI2C_Struct *s);
void sFlash_setAddressUpper(uint32_t address, HI2C_Struct *s);
/*
static inline void sFlash_WriteEnable(void)  { GPIOB->BSRR |= GPIO_BSRR_BR14; }
static inline void sFlash_WriteDisable(void) { GPIOB->BSRR |= GPIO_BSRR_BS14; }
*/
bool sFLASH_Init(void) {
    bool ret;
    uint8_t i;
    uint32_t config;
    HI2C_Struct s = { false, false, 0xA0u, 0u, 0u};
    
    
    // pin WP
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;      // enable clock for GPIOx
    config = (GPIOB->CRH & (~(GPIO_CRH_MODE14 | GPIO_CRH_CNF14)));
    config |= (uint32_t)((uint32_t)PIN_OUTPUT << GPIO_CRH_MODE14_Pos);
    GPIOB->BSRR |= GPIO_BSRR_BS14; //sFlash_WriteDisable();
    GPIOB->CRH = config;

    HI2C0_vInit(0xA0u, &s);

    HI2C0_vSetSDA();
    HI2C0_vSetSCL();
    HI2C0_vOutputSDA();
    HI2C0_vOutputSCL();
    for(i = 0u; i < 9u; i++) {
        /* generate clock */
        HI2C0_vSetSCL();
        HI2C0_vBitDelayH();
        HI2C0_vClrSCL();
        HI2C0_vBitDelayL();
    }
    HI2C0_vInputSCL();
    HI2C0_vInputSDA();
    HI2C0_vMakeStartCondition();
    HI2C0_vMakeStopCondition(&s);   // RESET of chip

    for(i = 0u; i < 16u; i+=2u) {
        ret = HI2C0_bSetAddr(HI2C0_getChipAddress(&s) | i, &s);
        HI2C0_vMakeStopCondition(&s);
        if(ret) {
            break;
        }
    }
    return ret;
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
        sFlash_setAddressUpper(Address, &s);
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
    return ret;
    //return true;
}

bool sFLASH_WriteBuffer(uint8_t* buffer, uint32_t Address, uint32_t Size) {
    bool ret = false;
    bool i2cStop;
    uint8_t i;
    HI2C_Struct s = { false, false, 0xA0u, 0u, 0u};
    GPIOB->BSRR |= GPIO_BSRR_BR14; //sFlash_WriteEnable();
    while(0 < Size) {
        sFlash_setAddressUpper(Address, &s);
        if((15u < Size) && (0u == (Address & 0xF))) {   // adresa musi byt zarovnana na 16. Inak napaluj po bajte
            ret = HI2C0_writeByte((uint8_t) Address, false, *buffer++, &s);
            if(ret) {
                i2cStop = false;
                for(i = 0u; ((i < 15u) && (true == ret)); i++) {
                    ret = HI2C0_bSetTxData(*buffer++, i2cStop, &s);
                    if(13 == i) {
                        i2cStop = true;
                    }
                }
                while(!sFlash_isReady(&s));
                Size -= 16;
                Address += 16;
            } else {
                ret = false;
                break;
            }
        } else {
            ret = HI2C0_writeByte((uint8_t) Address, true, *buffer++, &s);
            if(ret) {
                while(!sFlash_isReady(&s));
                Size--;
                Address++;
            } else {
                ret = false;
                break;
            }
        }
    }
    GPIOB->BSRR |= GPIO_BSRR_BS14; //sFlash_WriteDisable();
    return ret;
}

bool sFLASH_EraseBulk(void) {
    bool ret = false;
    uint8_t buff[256u];
    uint16_t i, address;
    for(i = 0u; i < 256u; i++) {
        buff[i] = 0xFF;
    }
    address = 0u;
    for(i = 0u; i < 8u; i++) {
        ret = sFLASH_WriteBuffer(buff, address, 256u);
        address += 256u;
        if(!ret) {
            break;
        }
    }
    return ret;
    
}

bool sFLASH_EraseSector(uint32_t EraseStartAddress ,uint32_t EraseEndAddress) {
    bool ret = false;
    uint8_t i;
    uint8_t buff[128];
    (void)EraseEndAddress;
    for(i = 0u; i < 128u; i++) {
        buff[i] = 0xFFu;
    }
    ret = sFLASH_WriteBuffer(buff, EraseStartAddress, 128u);
    return ret;
}

bool sFlash_isReady(HI2C_Struct *s) {
    bool ret = false;
    HI2C0_vWaitForSlave(s);
    ret = HI2C0_bSetAddr(HI2C0_getChipAddress(s), s);
    HI2C0_vMakeStopCondition(s);
    return ret;
}

void sFlash_setAddressUpper(uint32_t address, HI2C_Struct *s) {
    address &= 0x700u;
    address >>= 7u;
    HI2C0_setChipAddress((uint8_t)((HI2C0_getChipAddress(s) & 0xF0u) | address), s);
}
