#include "stm32_i2c_24cXX.h"
#include "hi2c0.h"

bool sFlash_isReady(HI2C_Struct *s);
void sFlash_setAddressUpper(uint32_t address, HI2C_Struct *s);
bool sFlash_searchFirstChip(HI2C_Struct *s);

bool sFLASH_Init(void) {
    uint8_t i;
    HI2C_Struct s = { false, false, EE24C_CHIP_ADDRESS, 0u, 0u};
    
    
    LED_vInit();
    LED_vSet(false);
    HI2C0_vInit(EE24C_CHIP_ADDRESS, &s);
    HI2C0_vWriteDisable();

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
    HI2C0_vWriteDisable();
    if(sFlash_searchFirstChip(&s)) {
        LED_vSet(!LED_bGet());
        if(0u < Size) {
            if(Size <= 1) {
                i2cStop = true;
            } else {
                i2cStop = false;
            }
#if defined(EE24C_ONE_BYTE_ADDRESS)
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
    LED_vSet(false);
    return ret;
}

bool sFLASH_WriteBuffer(uint8_t* buffer, uint32_t Address, uint32_t Size) {
    bool ret = false;
    bool i2cStop;
    uint8_t i;
    HI2C_Struct s = { false, false, EE24C_CHIP_ADDRESS, 0u, 0u};
    if(sFlash_searchFirstChip(&s)) {
        HI2C0_vWriteEnable();
        while(0 < Size) {
            LED_vSet(!LED_bGet());
#if defined(EE24C_ONE_BYTE_ADDRESS)
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
        HI2C0_vWriteDisable();
    }
    LED_vSet(false);
    return ret;
}

bool sFLASH_EraseBulk(void) {
    bool ret = false;
    uint8_t buff[EE24C_PROG_PAGE_SIZE];
    uint16_t i, address;
    for(i = 0u; i < EE24C_PROG_PAGE_SIZE; i++) {
        buff[i] = 0xFF;
    }
    address = 0u;
    for(i = 0u; i < (EE24C_DEVICE_SIZE / EE24C_PROG_PAGE_SIZE); i++) {
        ret = sFLASH_WriteBuffer(buff, address, EE24C_PROG_PAGE_SIZE);
        address += EE24C_PROG_PAGE_SIZE;
        if(!ret) {
            break;
        }
    }
    return ret;
    
}

bool sFLASH_EraseSector(uint32_t EraseStartAddress ,uint32_t EraseEndAddress) {
    bool ret = false;
    uint8_t i;
    uint8_t buff[EE24C_PROG_PAGE_SIZE];
    (void)EraseEndAddress;
    for(i = 0u; i < EE24C_PROG_PAGE_SIZE; i++) {
        buff[i] = 0xFFu;
    }
    ret = sFLASH_WriteBuffer(buff, EraseStartAddress, EE24C_PROG_PAGE_SIZE);
    return ret;
}

bool sFlash_isReady(HI2C_Struct *s) {
    bool ret = false;
    HI2C0_vWaitForSlave(s);
    ret = HI2C0_bSetAddr(HI2C0_getChipAddress(s), s);
    HI2C0_vMakeStopCondition(s);
    return ret;
}

#if defined(EE24C_ONE_BYTE_ADDRESS)
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
