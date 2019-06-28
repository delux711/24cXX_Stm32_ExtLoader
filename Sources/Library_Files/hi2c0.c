#include "hi2c0.h"
#include "stm32f10x.h"

#define GPIO_CRH_MODE12_Pos               ((12 - 8) * 4)
#define GPIO_CRH_MODE13_Pos               ((13 - 8) * 4)
#define HI2C_OUTPUT_OPEN_DRAIN_ENABLE     (0x3u)    ///< 50MHz output with open drain. See GPIOx_CRL register
#define HI2C_INPUT_PULL_UP_ENABLE         (0x4u)    ///< Input float. See GPIOx_CRL register
#define HI2C_OUTPUT                       HI2C_OUTPUT_OPEN_DRAIN_ENABLE
#define HI2C_INPUT                        HI2C_INPUT_PULL_UP_ENABLE

/*
bool HI2C0_bEventEnabled;
bool bIsChippresent;
uint8_t ucChipAddr;
uint8_t HI2C0_ucError;
uint8_t HI2C0_ucLastRx;
*/

/* maximum waitstate delay */
const uint8_t HI2C0_ucMaxWaitState = 0xFF;

/* FUNCTIONS */
void HI2C0_vWaitForSlave(HI2C_Struct *s);


/*************************************************************************************/

void HI2C0_vInitPort(void) {
    uint32_t tempCR;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;      /* enable clock for GPIOx */
    /*
    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT13);
    GPIOB->OTYPER |= (1u << GPIO_OTYPER_OT13_Pos);
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD13);
    GPIOB->PUPDR |= (1u << GPIO_PUPDR_PUPD13_Pos); // pull-up
    */
    tempCR = GPIOB->CRH & ~(uint32_t)(GPIO_CRH_CNF12 | GPIO_CRH_MODE12 | GPIO_CRH_CNF13 | GPIO_CRH_MODE13);
    tempCR |= (uint32_t)((uint32_t)HI2C_INPUT << GPIO_CRH_MODE12_Pos);
    tempCR |= (uint32_t)((uint32_t)HI2C_INPUT << GPIO_CRH_MODE13_Pos);
    GPIOB->CRH = tempCR;
}


void HI2C0_vOutputSCL(void) {
    uint32_t tempCR;
    tempCR = GPIOB->CRH & ~(uint32_t)(GPIO_CRH_CNF12 | GPIO_CRH_MODE12);
    tempCR |= (uint32_t)((uint32_t)HI2C_OUTPUT << GPIO_CRH_MODE12_Pos);
    GPIOB->CRH = tempCR;/*
    GPIOB->MODER &= ~(GPIO_MODER_MODE12);
    GPIOB->MODER |= (1U << GPIO_MODER_MODE12_Pos);*/
}

void HI2C0_vInputSCL(void) {
    uint32_t tempCR;
    tempCR = GPIOB->CRH & ~(uint32_t)(GPIO_CRH_CNF12 | GPIO_CRH_MODE12);
    tempCR |= (uint32_t)((uint32_t)HI2C_INPUT << GPIO_CRH_MODE12_Pos);
    GPIOB->CRH = tempCR;/*
    GPIOB->MODER &= ~(GPIO_MODER_MODE12);
    GPIOB->MODER |= (0U << GPIO_MODER_MODE12_Pos);*/
}

void HI2C0_vClrSCL(void) {
    GPIOB->BSRR |= GPIO_BSRR_BR12;
}

void HI2C0_vSetSCL(void) {
    GPIOB->BSRR |= GPIO_BSRR_BS12;
}

bool HI2C0_bGetSCL(void) {
    return GPIOB->IDR & GPIO_IDR_IDR12;
}

void HI2C0_vOutputSDA(void) {
    uint32_t tempCR;
    tempCR = GPIOB->CRH & ~(uint32_t)(GPIO_CRH_CNF13 | GPIO_CRH_MODE13);
    tempCR |= (uint32_t)((uint32_t)HI2C_OUTPUT << GPIO_CRH_MODE13_Pos);
    GPIOB->CRH = tempCR;/*
    GPIOB->MODER &= ~(GPIO_MODER_MODE13);
    GPIOB->MODER |= (1U << GPIO_MODER_MODE13_Pos);*/
}

void HI2C0_vInputSDA(void) {
    uint32_t tempCR;
    tempCR = GPIOB->CRH & ~(uint32_t)(GPIO_CRH_CNF13 | GPIO_CRH_MODE13);
    tempCR |= (uint32_t)((uint32_t)HI2C_INPUT << GPIO_CRH_MODE13_Pos);
    GPIOB->CRH = tempCR;/*
    GPIOB->MODER &= ~(GPIO_MODER_MODE13);
    GPIOB->MODER |= (0U << GPIO_MODER_MODE13_Pos);*/
}

void HI2C0_vClrSDA(void) {
   GPIOB->BSRR |= GPIO_BSRR_BR13;
}

void HI2C0_vSetSDA(void) {
   GPIOB->BSRR |= GPIO_BSRR_BS13;
}

bool HI2C0_bGetSDA(void) {
    return GPIOB->IDR & GPIO_IDR_IDR13;
}


void HI2C0_vBitDelayH(void) {
    /** \todo Delay must be adjusted to get not more than 400Khz */
    uint8_t i;
    for(i = 0; i < 8u; i++) {
        __asm("NOP"); 
        __asm("NOP"); 
        __asm("NOP"); 
        __asm("NOP"); 
        __asm("NOP"); 
        __asm("NOP"); 
    }
}

void HI2C0_vBitDelayL(void) {
    /** \todo Delay must be adjusted to get not more than 400Khz */
    uint8_t i;
    for(i = 0; i < 8u; i++) {
        __asm("NOP"); 
        __asm("NOP"); 
        __asm("NOP"); 
        __asm("NOP"); 
        __asm("NOP"); 
        __asm("NOP"); 
    }
}

/* for 100kHz mode this function has to wait at least 5 µsec - overhead for call/return  */
/* for 400kHz mode this function has to wait at least 1.10 µsec - overhead for call/return */
void HI2C0_vBitDly(void) {
    /* use HSUP_vDelay(10); for 100 kHz mode */
    /* use nothing for 400 kHz mode */
}

/* event handler */
void HI2C0_vHandleEvent(void) {

}

/*************************************************************************************/

/* set SCL to a high state and wait until the slave releases it too */
static void HI2C0_vWaitForSlave(HI2C_Struct *s) {
    uint8_t ucWait = HI2C0_ucMaxWaitState;

    HI2C0_vInputSCL();
    HI2C0_vSetSCL();

    while(HI2C0_bGetSCL() == 0u) {
        ucWait--;
        if(ucWait == 0u) {
            /* SCL stuck? ... */
            s->HI2C0_ucError |= HI2C0_TIMEOUT;
            return;
        }
    }
    HI2C0_vOutputSCL();
    HI2C0_vBitDelayH();
}

void HI2C0_vMakeStopCondition(HI2C_Struct *s) {
    /* generate stop condition */
    HI2C0_vOutputSDA();
    HI2C0_vClrSDA();
    HI2C0_vBitDelayL();
    HI2C0_vWaitForSlave(s);
    HI2C0_vSetSDA();
    HI2C0_vBitDelayH();
    /* release bus */
    HI2C0_vInputSCL();
    HI2C0_vInputSDA();
}

void HI2C0_vInit(uint8_t chipAddress, HI2C_Struct *s) {
    s->ucChipAddr = 0x12u;
    s->bIsChippresent = false;
    s->HI2C0_ucError = 0;
    s->HI2C0_ucLastRx = 0;
    
    HI2C0_setChipAddress(chipAddress, s);
#ifdef HI2C_SPECIAL_INIT 
    HI2C0_vInitPort(); /* Initialize the special features of the periphery. */
#endif
    /* preset for port latch */
    HI2C0_vInputSCL();
    HI2C0_vInputSDA();
}

bool HI2C0_bSetAddr(uint8_t ucAddress, HI2C_Struct *s) {
    /* no errors so far */
    s->HI2C0_ucError = 0u;

    HI2C0_vSetSDA();
    HI2C0_vSetSCL();

    /* generate start condition */
    HI2C0_vOutputSDA();
    HI2C0_vOutputSCL();
    HI2C0_vBitDelayH();
    HI2C0_vClrSDA()   ;
    HI2C0_vBitDelayH();
    HI2C0_vClrSCL()   ;
    HI2C0_vBitDelayL();

    /* transmit address */
    return HI2C0_bSetTxData(ucAddress, 0u, s);
}

/* HI2C0_bSetTxData() */
bool HI2C0_bSetTxData(uint8_t ucDataByte, bool bStop, HI2C_Struct *s) {
    uint8_t ucCounter;

    /* transmit all 8 data bits */
    ucCounter = 8u;
    do {
        /* send each bit, MSB first */
        if((ucDataByte & 0x80u) != 0u) {
            HI2C0_vSetSDA();
        }
        else {
            HI2C0_vClrSDA();
        }
        ucDataByte = (uint8_t)(ucDataByte << 1u);

        /* generate clock */
        HI2C0_vSetSCL();
        HI2C0_vBitDelayH();
        HI2C0_vClrSCL();
        HI2C0_vBitDelayL();
        --ucCounter;
   } while(ucCounter > 0u);

    /* listen for ACK */
    HI2C0_vSetSDA();
    HI2C0_vInputSDA();
    HI2C0_vWaitForSlave(s);

    if(HI2C0_bGetSDA() != 0u) {
        /* ack didn't happen, may be nothing out there */
        s->HI2C0_ucError |= HI2C0_NACK;
    }
    HI2C0_vClrSCL();
    HI2C0_vBitDelayL();
    HI2C0_vSetSDA();
    HI2C0_vOutputSDA();

    if((bStop != 0u) || (s->HI2C0_ucError != 0u)) {
        /* generate stop condition */
        HI2C0_vMakeStopCondition(s);
    }
    /* call event handler */

    return (bool)(s->HI2C0_ucError == 0u);
}

/* HI2C0_vTriggerReceive() */
uint8_t HI2C0_vTriggerReceive(bool bStop, HI2C_Struct *s) {
    uint8_t ucCounter;

    /* switch to input since we want to receive data */
    HI2C0_vInputSDA();

    /* receive the bits -- starting with the MSB */
    ucCounter = 8u;

    do {
        HI2C0_vSetSCL();
        HI2C0_vBitDelayH();

        s->HI2C0_ucLastRx <<= 1u;

        if(HI2C0_bGetSDA() != 0u) {
            s->HI2C0_ucLastRx |= 1u;
        }
        HI2C0_vClrSCL();
        HI2C0_vBitDelayL();
        --ucCounter;
   } while(ucCounter > 0u);

    /* send ACK according to the stop flag */
    HI2C0_vOutputSDA();

    if(bStop != 0u) {
        /* no acknowledge */
        HI2C0_vSetSDA();
    } else {
        /* acknowledge */
        HI2C0_vClrSDA();
    }
    HI2C0_vWaitForSlave(s);
    HI2C0_vClrSCL();
    HI2C0_vSetSDA();
    HI2C0_vBitDelayL();

    if(bStop != 0u) {
        HI2C0_vMakeStopCondition(s);
    }
    /* call event handler */
    if(s->HI2C0_bEventEnabled != 0u) {
        HI2C0_vHandleEvent();
    }
    return s->HI2C0_ucLastRx;
}

void HI2C0_vSendStop(void) {
    /* Send stop-condition.  Changing of SDA during SCL = OUTPUT_HIGH is */
    /* only allowed while sending start- or stop-condition.  For generating */
    /* stop-condition both SDA and SCL must be LOW */
    HI2C0_vClrSCL();
    HI2C0_vClrSDA();
    HI2C0_vOutputSCL();
    HI2C0_vOutputSDA();

    HI2C0_vBitDelayL();           /* clock low period */
    HI2C0_vSetSCL();              /* change SCL edge to OUTPUT_HIGH level */
    HI2C0_vBitDelayH();           /* Stop condition setup time */
    HI2C0_vSetSDA();;             /* change SDA edge to OUTPUT_HIGH level */

    HI2C0_vInputSCL();
    HI2C0_vInputSDA();
}

bool HI2C0_bForceBusRelease(void) {
    uint16_t uiCounter;
    bool bSdaStatus;

    HI2C0_vOutputSCL();
    HI2C0_vInputSDA();

    for(uiCounter = 0u; uiCounter < 15u; uiCounter++) {
        HI2C0_vClrSCL();          /* security: send some clocks to free the bus-lines */
        HI2C0_vBitDelayL();          /* clock low period */
        HI2C0_vSetSCL();
        HI2C0_vBitDelayH();
    }
    HI2C0_vSendStop();

    /* check BUS-lines */
    for(uiCounter = 0u; uiCounter < 5u; uiCounter++) {
        bSdaStatus = HI2C0_bGetSDA();
        if(bSdaStatus != 0u) {
            break;
        }
    }
    return (bSdaStatus);
}

uint8_t HI2C0_readByte(uint8_t addr, bool stop, HI2C_Struct *s) {
    uint8_t ret;
    ret = 0u;
    if(true == HI2C0_writeAddr(addr, true, s)) {
        if(true == HI2C0_bSetAddr((s->ucChipAddr | 0x01u), s)) { // read
            ret = HI2C0_vTriggerReceive(stop, s);
        }
    }
    return ret;
}
    
bool HI2C0_writeByte(uint8_t addr, bool stop, uint8_t data, HI2C_Struct *s) {
    bool ret;
    ret = false;
    if(true == HI2C0_writeAddr(addr, false, s)) { // write
        if(true == HI2C0_bSetTxData(data, stop, s)) { // write address
            ret = true;
        }
    }
    return ret;
}

bool HI2C0_writeAddr(uint8_t addr, bool stop, HI2C_Struct *s) {
    bool ret;
    ret = false;
    //if(true == HI2C0_bSetAddr(ucChipAddr)) { // write
    if(true == HI2C0_bSetAddr(HI2C0_getChipAddress(s), s)) { // write
        s->bIsChippresent = true;
        if(true == HI2C0_bSetTxData(addr, stop, s)) { // write address
            ret = true;
        } else {
            s->bIsChippresent = false;
        }
    } else {
        s->bIsChippresent = false;
    }
    return ret;
}

void HI2C0_setChipAddress(uint8_t chipAddress, HI2C_Struct *s) {
    s->ucChipAddr = (uint8_t)(0xFEu & chipAddress);
}

uint8_t HI2C0_getChipAddress(HI2C_Struct *s) {
    return s->ucChipAddr;
}

bool HI2C0_isChipPresent(HI2C_Struct *s) {
    return s->bIsChippresent;
}
