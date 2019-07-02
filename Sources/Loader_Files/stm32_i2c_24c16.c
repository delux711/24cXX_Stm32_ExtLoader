#include "stm32_i2c_24c16.h"
#include "stm32f10x.h"
#include "hi2c0.h"
//#include "stm32f10x_i2c.h"

#define EE24C_CHIP_ADDRESS    (0xA0u)

#if defined(I2C_24C16)
#define EE24C_PROG_PAGE_SIZE  (16u)
#define EE24C_FULL_EREASE     (0x800u / 256u)   // 8
#elif defined(I2C_24C512)
#define EE24C_PROG_PAGE_SIZE  (128u)
#define EE24C_FULL_EREASE     (0x80000u / 256u) // 2048
#endif

#define GPIO_CRH_MODE14_Pos ((14 - 8) * 4)
#define PIN_OUTPUT          (2u)

bool sFlash_isReady(HI2C_Struct *s);
void sFlash_setAddressUpper(uint32_t address, HI2C_Struct *s);
bool sFlash_searchFirstChip(HI2C_Struct *s);
/*
static inline void sFlash_WriteEnable(void)  { GPIOB->BSRR |= GPIO_BSRR_BR14; }
static inline void sFlash_WriteDisable(void) { GPIOB->BSRR |= GPIO_BSRR_BS14; }
*/
bool sFLASH_Init(void) {
    uint8_t i;
    uint32_t config;
    HI2C_Struct s = { false, false, EE24C_CHIP_ADDRESS, 0u, 0u};
    
    
    // pin WP
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;      // enable clock for GPIOx
    config = (GPIOB->CRH & (~(GPIO_CRH_MODE14 | GPIO_CRH_CNF14)));
    config |= (uint32_t)((uint32_t)PIN_OUTPUT << GPIO_CRH_MODE14_Pos);
    GPIOB->BSRR |= GPIO_BSRR_BS14; //sFlash_WriteDisable();
    GPIOB->CRH = config;

    HI2C0_vInit(EE24C_CHIP_ADDRESS, &s);

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

    return sFlash_searchFirstChip(&s);
}

bool sFLASH_ReadBuffer(uint8_t* buffer, uint32_t Address, uint32_t Size) {
    bool ret = true;
    bool i2cStop = false;
    uint8_t temp;
    HI2C_Struct s = { false, false, EE24C_CHIP_ADDRESS, 0u, 0u};
    if(sFlash_searchFirstChip(&s)) {
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
#if defined(I2C_24C16)
            sFlash_setAddressUpper(Address, &s);
#endif
            temp = HI2C0_readByte((HI2C_ADDRESS_LENGTH) Address, i2cStop, &s);
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
    }
    return ret;
}

bool sFLASH_WriteBuffer(uint8_t* buffer, uint32_t Address, uint32_t Size) {
    bool ret = false;
    bool i2cStop;
    uint8_t i;
    HI2C_Struct s = { false, false, EE24C_CHIP_ADDRESS, 0u, 0u};
    if(sFlash_searchFirstChip(&s)) {
        GPIOB->BSRR |= GPIO_BSRR_BR14; //sFlash_WriteEnable();
        while(0 < Size) {
#if defined(I2C_24C16)
            sFlash_setAddressUpper(Address, &s);
#endif
            if(((EE24C_PROG_PAGE_SIZE - 1u) < Size) && (0u == (Address & (EE24C_PROG_PAGE_SIZE - 1u)))) {   // adresa musi byt zarovnana na 16. Inak napaluj po bajte
                ret = HI2C0_writeByte((HI2C_ADDRESS_LENGTH) Address, false, *buffer++, &s);  // HI2C_ADDRESS_LENGTH  Address = uint8/16_t Address
                if(ret) {
                    i2cStop = false;
                    for(i = 0u; ((i < (EE24C_PROG_PAGE_SIZE - 1u)) && (true == ret)); i++) {
                        ret = HI2C0_bSetTxData(*buffer++, i2cStop, &s);
                        if((EE24C_PROG_PAGE_SIZE - 3u) == i) {
                            i2cStop = true;
                        }
                    }
                    while(!sFlash_isReady(&s));
                    Size -= EE24C_PROG_PAGE_SIZE;
                    Address += EE24C_PROG_PAGE_SIZE;
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
    }
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
    for(i = 0u; i < EE24C_FULL_EREASE; i++) {   // EE24C_FULL_EREASE = (24c16 = 8), (24c512 = 2048)
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

#if defined(I2C_24C16)
void sFlash_setAddressUpper(uint32_t address, HI2C_Struct *s) {
    address &= 0x700u;
    address >>= 7u;
    HI2C0_setChipAddress((uint8_t)((HI2C0_getChipAddress(s) & 0xF0u) | address), s);
}
#endif

bool sFlash_searchFirstChip(HI2C_Struct *s) {
    bool ret = false;
    uint8_t i, address;
    for(i = 0u; i < 16u; i+=2u) {
        address = (HI2C0_getChipAddress(s) | i);
        ret = HI2C0_bSetAddr(address, s);
        HI2C0_vMakeStopCondition(s);
        if(ret) {
            HI2C0_setChipAddress(address, s);
            break;
        }
    }
    return ret;
}
