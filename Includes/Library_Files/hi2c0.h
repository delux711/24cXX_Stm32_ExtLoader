#ifndef _HI2C0_H
#define _HI2C0_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#if defined(STM32F10X_MD)
#include "stm32f10x.h"

//SCL_PORT=B,SCL_PIN=12,SDA_PORT=B, SDA_PIN=13, RW_PORT=B, RW_PIN=14, LED_PORT=C, LED_PIN=13
// #define SCL_PIN  12
// #define SCL_PORT B
// #define SDA_PIN  13
// #define SDA_PORT B
// #define RW_PIN   14
// #define RW_PORT  B
// #define LED_PIN  13
// #define LED_PORT C

#elif defined(STM32L476xx)
#include "stm32l4xx.h"

//SCL_PORT=E,SCL_PIN=13,SDA_PORT=E,SDA_PIN=14,RW_PORT=E,RW_PIN=15,LED_PORT=B,LED_PIN=2
// #define SCL_PIN  13
// #define SCL_PORT E
// #define SDA_PIN  14
// #define SDA_PORT E
// #define RW_PIN   15
// #define RW_PORT  E
// #define LED_PIN  2
// #define LED_PORT B

#else
#error "Please select first the target STM32F10X_MD or STM32L476xx!"
#endif


#if defined(EE24C_ONE_BYTE_ADDRESS)
#define HI2C_ADDRESS_LENGTH   uint8_t
#elif defined(EE24C_TWO_BYTES_ADDRESS)
#define HI2C_ADDRESS_LENGTH   uint16_t
#endif

/************** STM32L103 ********************/
#if defined(STM32F10X_MD)
#define GPIO_CRL_Pos                      (0u)
#define GPIO_CRH_Pos                      (8u)
#if SCL_PIN < 8
#define SCL_CRLH    L
#else
#define SCL_CRLH    H
#endif
#if SDA_PIN < 8
#define SDA_CRLH    L
#else
#define SDA_CRLH    H
#endif
#if RW_PIN < 8
#define RW_CRLH    L
#else
#define RW_CRLH    H
#endif
#if LED_PIN < 8
#define LED_CRLH    L
#else
#define LED_CRLH    H
#endif
#define HI2C_OUTPUT_OPEN_DRAIN_ENABLE        (0x7u)    ///< 50MHz output with open drain. See GPIOx_CRL register
#define HI2C_INPUT_PULL_UP_ENABLE            (0x8u)    ///< Input float. See GPIOx_CRL register
#define HI2C_OUTPUT                          (0x3u)    //< 50MHz output push pull
#define HI2C_OUTPUT_OPEN_DRAIN               HI2C_OUTPUT_OPEN_DRAIN_ENABLE
#define HI2C_INPUT                           HI2C_INPUT_PULL_UP_ENABLE

#define _HI2C0_vEnablePort(port)             RCC->APB2ENR |= RCC_APB2ENR_IOP##port##EN;

#define _HI2C0_vSetDir(inOut,port,pin,crlh)  do {                                                                                                     \
                                                 uint32_t tempCR;                                                                                     \
                                                 tempCR = GPIO##port##->CR##crlh & ~(uint32_t)(GPIO_CR##crlh##_CNF##pin | GPIO_CR##crlh##_MODE##pin); \
                                                 tempCR |= (uint32_t)((uint32_t)inOut << ((##pin - GPIO_CR##crlh##_Pos) * 4u));                       \
                                                 GPIO##port##->CR##crlh = tempCR;                                                                     \
                                             } while(0)
#define HI2C0_vSetDir(inOut,port,pin,crlh)   _HI2C0_vSetDir(inOut,port,pin,crlh)

#define _HI2C0_bGetPin(port,pin)             ((bool)(GPIO##port##->IDR & GPIO_IDR_IDR##pin))
    
#define HI2C0_vSetDirSCL(inOut)              HI2C0_vSetDir(inOut, SCL_PORT, SCL_PIN, SCL_CRLH)
#define HI2C0_vSetDirSDA(inOut)              HI2C0_vSetDir(inOut, SDA_PORT, SDA_PIN, SDA_CRLH)
#define HI2C0_vSetDirRW(inOut)               HI2C0_vSetDir(inOut, RW_PORT , RW_PIN , LED_CRLH)
#define HI2C0_vSetDirLED(inOut)              HI2C0_vSetDir(inOut, LED_PORT, LED_PIN ,LED_CRLH)
#define HI2C0_vInitOpenDrainPullUp(port,pin)

/************** STM32L476 ********************/
#elif defined(STM32L476xx)
#define HI2C_OUTPUT_OPEN_DRAIN               (1u)
#define HI2C_OUTPUT                          (1u)
#define HI2C_INPUT                           (0u)

#define _HI2C0_vEnablePort(port)             RCC->AHB2ENR |= RCC_AHB2ENR_GPIO##port##EN;

#define _HI2C0_vSetDir(inOut,port,pin)       do {                                                                       \
                                                 uint32_t tempCR = GPIO##port##->MODER & (~(GPIO_MODER_MODE##pin));     \
                                                 tempCR |= (inOut << GPIO_MODER_MODE##pin##_Pos);                       \
                                                 GPIO##port##->MODER = tempCR;                                          \
                                             } while(0)
#define HI2C0_vSetDir(inOut,port,pin)        _HI2C0_vSetDir(inOut,port,pin)

#define _HI2C0_bGetPin(port,pin)             ((bool)(GPIO##port##->IDR & GPIO_IDR_ID##pin))

#define HI2C0_vSetDirSCL(inOut)              HI2C0_vSetDir(inOut, SCL_PORT, SCL_PIN)
#define HI2C0_vSetDirSDA(inOut)              HI2C0_vSetDir(inOut, SDA_PORT, SDA_PIN)
#define HI2C0_vSetDirRW(inOut)               HI2C0_vSetDir(inOut, RW_PORT , RW_PIN)
#define HI2C0_vSetDirLED(inOut)              HI2C0_vSetDir(inOut, LED_PORT, LED_PIN)

#define _HI2C0_vInitOpenDrainPullUp(port,pin) do { GPIO##port##->OTYPER |= GPIO_OTYPER_OT##pin##_Msk; GPIO##port##->PUPDR |= (1u << GPIO_PUPDR_PUPD##pin##_Pos); } while(0)
#define HI2C0_vInitOpenDrainPullUp(port,pin) _HI2C0_vInitOpenDrainPullUp(port,pin)

#endif
#define _HI2C0_vSetPin(log,port,pin)         do { if(log) { GPIO##port##->BSRR |= GPIO_BSRR_BS##pin; } else { GPIO##port##->BSRR |= GPIO_BSRR_BR##pin; } }while(0)

#define HI2C0_vEnablePort(port)              _HI2C0_vEnablePort(port)
#define HI2C0_bGetPin(port,pin)              _HI2C0_bGetPin(port,pin)
#define HI2C0_vSetPin(log,port,pin)          _HI2C0_vSetPin(log,port,pin)

typedef struct _HI2C_Struct {
    bool HI2C0_bEventEnabled;
    bool bIsChippresent;
    uint8_t ucChipAddr;
    uint8_t HI2C0_ucError;
    uint8_t HI2C0_ucLastRx;
} HI2C_Struct;

/**
    How to use:
    Zmenit vsetky vyskyty 0 za jedinecke cislo, resp. identifikator i2c zbernice (0 to 0)
    Zmenit 13 za cislo pinu na ktorom su pripojene data (13 to 10, pre pin PB10) 
    Zmenit B za pismeno portu (B to B, pre pin PB10)
    Zmenit 12 za cislo pinu na ktorom su master hodinovy clock to 10, (12 to 9 pre pin PA9) 
    Zmenit B za pismeno portu (B to A, pre pin PA9)
    Vsetky tieto zmeny spravit v tomto *.h aj *.c subore
*/

/**
\b description: \n
 This is the define that represents the error indication of a message not acknowledged by the slave.
 The error is  returned by HI2C<unit>_ucGetError().
*/
#define HI2C0_NACK    (1u<<0u)

/**
\b description: \n
 This is the define that represents the error indication of a message timeout by the slave.
 The error is  returned by HI2C<unit>_ucGetError().
*/
#define HI2C0_TIMEOUT (1u<<1u)

/**
\b description: \n
 This is the external reference to the maximum waitstate delay. This delay controls
 the number of cycles the master shall wait for a slave acknowledge.
<B>HC08 implementation:</B>
 cycles on the HC08 implementation is a loop with takes at least 20 processor cycles
*/
extern const uint8_t HI2C0_ucMaxWaitState;

extern bool HI2C0_writeByte(HI2C_ADDRESS_LENGTH addr, bool stop, uint8_t data, HI2C_Struct *s);
extern bool HI2C0_writeAddr(HI2C_ADDRESS_LENGTH addr, bool stop, HI2C_Struct *s);
extern bool HI2C0_isChipPresent(HI2C_Struct *s);
extern uint8_t HI2C0_readByte(HI2C_ADDRESS_LENGTH addr, bool stop, HI2C_Struct *s);
extern uint8_t HI2C0_getChipAddress(HI2C_Struct *s);
extern void HI2C0_setChipAddress(uint8_t chipAddress, HI2C_Struct *s);

/************************************************************************************/
extern void LED_vInit(void);
extern void LED_vSet(bool onOFF);
extern bool LED_bGet(void);

/** Initializes the port of the HI2C.
\b package: \n
 HI2CPackage1\n
 HI2CPackage2\n
\b \<unit\>: 0 \n
\b description: \n
 This function initializes the special features for the port of the HI2C<unit>. For instance in case
 of the Stm32 the clock for the port is enabled in this function.
\note
\param none
\return none
*/
#define HI2C_SPECIAL_INIT
extern void HI2C0_vInitPort(void);


/***************************************************************************************************$FHB
$name: HI2C<unit>_vOutputSCL
package:
 HI2CPackage1
 HI2CPackage2
prototype:
 void HI2C<unit>_vOutputSCL(void);
 <unit>: 0
description:
 User defined function that shall set the SCL pin to output
parameters:
 none
return:
 type:
           void
$HE****************************************************************************************************/
extern void HI2C0_vOutputSCL(void);


/***************************************************************************************************$FHB
$name: HI2C<unit>_vOutputSDA
package:
 HI2CPackage1
 HI2CPackage2
prototype:
 void HI2C<unit>_vOutputSDA(void);
 <unit>: 0
description:
 User defined function that shall set the SDA pin to output
parameters:
 none
return:
 type:
           void
$HE****************************************************************************************************/
extern void HI2C0_vOutputSDA(void);


/***************************************************************************************************$FHB
$name: HI2C<unit>_vInputSCL
package:
 HI2CPackage1
 HI2CPackage2
prototype:
 void HI2C<unit>_vInputSCL(void);
 <unit>: 0
description:
 User defined function that shall set the SCL pin to input
parameters:
 none
return:
 type:
            void
$HE****************************************************************************************************/
extern void HI2C0_vInputSCL(void);


/***************************************************************************************************$FHB
$name: HI2C<unit>_vInputSDA
package:
 HI2CPackage1
 HI2CPackage2
prototype:
 void HI2C<unit>_vInputSDA(void);
 <unit>: 0
description:
 User defined function that shall set the SDA pin to input
parameters:
 none
return:
 type:
           void
$HE****************************************************************************************************/
extern void HI2C0_vInputSDA(void);


/***************************************************************************************************$FHB
$name: HI2C<unit>_vClrSCL
package:
 HI2CPackage1
 HI2CPackage2
prototype:
 void HI2C<unit>_vClrSCL(void);
 <unit>: 0
description:
 User defined function that shall set the SCL pin to a low level
parameters:
 none
return:
 type:
           void
$HE****************************************************************************************************/
extern void HI2C0_vClrSCL(void);


/***************************************************************************************************$FHB
$name: HI2C<unit>_vSetSCL
package:
 HI2CPackage1
 HI2CPackage2
prototype:
 void HI2C<unit>_vSetSCL(void);
 <unit>: 0
description:
 User defined function that shall set the SCL pin to a high level
parameters:
 none
return:
 type:
           void
$HE****************************************************************************************************/
extern void HI2C0_vSetSCL(void);


/***************************************************************************************************$FHB
$name: HI2C<unit>_vClrSDA
package:
 HI2CPackage1
 HI2CPackage2
prototype:
 void HI2C<unit>_vClrSDA(void);
 <unit>: 0
description:
 User defined function that shall set the SDA pin to a low level
parameters:
 none
return:
 type:
           void
$HE****************************************************************************************************/
extern void HI2C0_vClrSDA(void);


/***************************************************************************************************$FHB
$name: HI2C<unit>_vSetSDA
package:
 HI2CPackage1
 HI2CPackage2
prototype:
 void HI2C<unit>_vSetSDA(void);
 <unit>: 0
description:
 User defined function that shall set the SDA pin to a high level
parameters:
 none
return:
 type:
           void
$HE****************************************************************************************************/
extern void HI2C0_vSetSDA(void);


/***************************************************************************************************$FHB
$name: HI2C<unit>_bGetSCL
package:
 HI2CPackage1
 HI2CPackage2
prototype:
 void HI2C<unit>_bGetSCL(void);
 <unit>: 0
description:
 User defined function that shall return the current pin level of SCL
parameters:
 none
return:
 type:
           void
$HE****************************************************************************************************/
extern bool HI2C0_bGetSCL(void);

/***************************************************************************************************$FHB
$name: HI2C<unit>_bGetSDA
package:
 HI2CPackage1
 HI2CPackage2
prototype:
 void HI2C<unit>_bGetSDA(void);
 <unit>: 0
description:
 User defined function that shall return the current pin level of SDA
parameters:
 none
return:
 type:
            void
$HE****************************************************************************************************/
extern bool HI2C0_bGetSDA(void);


/************************************************************************************/


/** Initializes the operation of the HI2C.

\b package: \n
 HI2CPackage1\n
 HI2CPackage2\n

\b \<unit\>: 0 \n

\b description: \n
 This function initializes the operation of the HI2C<unit>. It uses the respective
 configuration parameters defined in HI2C<unit>_Config. The call of the handler
 function HI2C<unit>_vHandleEvent is disabled after initialization. After initialization the
 HI2C<unit> is ready to accept further requests

\note This function is not reentrant.
      A configuration HI2C<unit>_Config is not required for initialization.

\param ucIndex: Indexes the configuration set to be used for initialization.\n
   \b type: uint8_t\n
   \b range: 0x00..0xFF

\return none
*/

extern void HI2C0_vInit(uint8_t chipAddress, HI2C_Struct *s);

/** Sets the address of the slave to communicate to.

\b package: \n
 HI2CPackage1\n
 HI2CPackage2\n

\b \<unit\>: 0\n

\b description: \n
 This interface sets the address of the slave to communicate to. The read/write information is
 contained in the address information. After the call of this interface the transmission of the
 address starts. Depending on the read/write bit of the address further calls of HI2C<unit>_bSetTxData
 or HI2C<unit>_ucGetRxData are possible until the stop information is passed to the interface.\n
 \b HI2CPackage1: \m
  This function completes the transmission and returns if
  the address is sent.\n
 \b HI2CPackage2: \n
  Once the transmission is completed, the respective handler
  HI2C<unit>_vHandleEvent is called.

\param uiAddress: This parameter is the address (including direction) to communicate to.\n
   \b type: uint16_t\n
   \b range: 0x00 .. 0xFF for 8 bit addressses

\return: The function returns true, if the setting of the address was successful,
         false otherwise.

 \b HI2CPackage1: \n
  The interface returns false, if the partner that shall be addressed is
  not responding (no acknowledge) or the partner inserts too many
  waitstates (by holding SCL low).
  If the event handler is enabled, the event handler is immediately
  called by this function once the reception is completed.

 \b HI2CPackage2: \n
  If the bus is currently busy due to another transmission/reception, this
  interface returns false.\n

 \b type: bool\n
 \b range: true, false
*/
extern bool HI2C0_bSetAddr(uint8_t ucAddress, HI2C_Struct *s);

/** Sends a single data byte over the I2C bus.

\b package: \n
 HI2CPackage1\n
 HI2CPackage2\n

\b \<unit\>: 0\n

\b description: \n
 This interface sends a single data byte over the I2C bus. After the call of this
 interface the transmission process starts.

  \b HI2CPackage1: \n
   This function completes the transmission and returns, if the date has been
   sent. If the event handler is enabled the event handler is called immediately
   once the reception is completed.\n

  \b HI2CPackage2: \n
   Once the transmission has been finished the respective handler HI2C<unit>_vHandleEvent
   is be called.

\param ucDataByte: This parameter contains the data byte to be transmitted.\n
   \b type: uint8_t\n
   \b range: 0x00..0xFF

\param bStop: This parameter defines if the stop condition has to be
              transmitted after the data byte.\n
   \b type: bool\n
   \b range: true, transmit stop condition

\return The function returns true, if the setting of the data was successful, false otherwise.\n

 \b HI2CPackage1: \n
  The interface returns false if the partner is not responding (no
  acknowledge) or the partner inserts too many waitstates (by holding SCL low).\n

 \b HI2CPackage2: \n
  This interface shall return false if any previous transmissions
  (addressing) has been failed or a previous transmissions (addressing) is
  still in progress.\n

 \b type: bool\n
 \b range: true, false
*/
extern bool HI2C0_bSetTxData(uint8_t ucDataByte, bool bStop, HI2C_Struct *s);

/** Returns the most recent received data byte.

\b package: \n
 HI2CPackage1\n
 HI2CPackage2\n

\b \<unit\>: 0\n

\b description: \n
 This interface returns the most recent received data byte from the addressed
 slave.

\param none

\return This interface returns the received data byte.\n
  \b type: uint8_t\n
  \b range: 0x00..0xFF
*/
#define HI2C0_ucGetRxData()   (HI2C0_ucLastRx)

/** Triggers a reception.

\b package: \n
 HI2CPackage1\n
 HI2CPackage2\n

\b \<unit\>: 0\n

\b description: \n
 \b HI2CPackage1: \m
  The interface triggers a reception and saves the received data byte once
  the reception has been finished. The stop information passed via the interface
  is used to generate the stop condition if requested.
  If the event handler is enabled the event handler will be immediately called by
  this function once the reception is completed.

 \b HI2CPackage2: \n
  This interface triggers the reception of the next data byte. The stop
  information passed via the interface shall is used to generate the stop
  condition if requested.

\param bStop: This information is used to signal the slave whether the received byte
              shall be the last one.\n
   \b type: bool\n
   \b range:
           - true, generate STOP condition
           - false, generate no STOP condition

\return none
*/
extern uint8_t HI2C0_vTriggerReceive(bool bStop, HI2C_Struct *s);

/** A handler called, if the transmission/reception process to/from the slave has been finished.

\b package: \n
 HI2CPackage1\n
 HI2CPackage2\n

\b \<unit\>: 0\n

\b description: \n
 This handler is called, if the transmission/reception process to/from the slave
 has been finished or upon occurrence of an error condition.

 \b HI2CPackage1: \n
  The implementation calls this function on the same interrupt level as the
  caller of the HI2C<unit>_vTriggerReceive or HI2C<unit>_bSetTxData
  function.\n

\note This function must be implemented by the user.

\param none

\return none
*/
extern void HI2C0_vHandleEvent(void);

/** Enables the call of the event handler.

\b package: \n
 HI2CPackage1\n
 HI2CPackage2\n

\b \<unit\>: 0\n

\b description: \n
 This function enables the call of the event handler HI2C<unit>_vHandleEvent.

\note This function must be implemented by the user.

\param none

\return none
*/
#define HI2C0_vEnableEvent() { HI2C0_bEventEnabled = 1u; }

/** Disables the call of the event handler.

\b package: \n
 HI2CPackage1\n
 HI2CPackage2\n

\b \<unit\>: 0 \n

\b description: \n
 This function disables the call of the event handler HI2C<unit>_vHandleEvent.

\param none

\return none
*/
#define HI2C0_vDisableEvent() { HI2C0_bEventEnabled = 0u; }

/** Returns the collected error information.

\b package: \n
 HI2CPackage1\n
 HI2CPackage2\n

\b \<unit\>: 0 \n

\b description: \n
 This interface returns the collected error information. The error information
 is cleared on read. In case an error is detected that has terminated the
 communication, the handler HI2C<unit>_vHandleEvent is be called.

\param none

\return
 \b type:  uint8_t\n
 \b range:
           - HI2C0_NACK    This information is set whenever the acknowledge of the addressed partner was
                          not detected.

           - HI2C0_TIMEOUT This information is set whenever the partner has inserted to many waitstates
                          (stuck SCL).
*/
#define HI2C0_ucGetError()  (HI2C0_ucError)

/** Generates a stop condition on the I2C bus.

\b package: \n
 HI2CPackage1\n
 HI2CPackage2\n

\b \<unit\>: 0\n

\b description: \n
 This interface generates a stop condition on the I2C bus.

\param none

\return none
 
*/
extern void HI2C0_vSendStop(void);

/** Generates clock pulses and a stop condition to force a slave to release the bus.

\b package: \n
 HI2CPackage1\n
 HI2CPackage2\n

\b \<unit\>: 0 \n

\b description: \n
 This interface generates clock pulses and a stop condition to force a slave to release the
 bus.

\param none

\return
       - 0: bus release has been failed
       - 1: bus release was successfull
 
*/
extern bool HI2C0_bForceBusRelease(void);

/** Performs the delay for H level of signal.

\b package: \n
 HI2CPackage1\n
 HI2CPackage2\n

\b \<unit\>: 0\n

\b description: \n
 User defined function that perform the delay for high phase of SCL
 (5 µsec for 100 kHz mode or 1.3 µsec for 400 kHz mode)
 To achieve the best performance the call/return overhead can be taken
 into acount.

\param none

\return none
*/
extern void HI2C0_vBitDelayH(void);

/** Performs the delay for L level of signal.

\b package: \n
 HI2CPackage1\n
 HI2CPackage2\n

\b \<unit\>: 0\n

\b description: \n
 User defined function that perform the delay for low phase of SCL
 (5 µsec for 100 kHz mode or 1.3 µsec for 400 kHz mode)
 To achieve the best performance the call/return overhead can be taken
 into acount.

\param none

\return none
*/
extern void HI2C0_vBitDelayL(void);
extern void HI2C0_vMakeStopCondition(HI2C_Struct *s);
extern void HI2C0_vMakeStartCondition(void);
extern void HI2C0_vWaitForSlave(HI2C_Struct *s);
extern void HI2C0_vWriteDisable(void);
extern void HI2C0_vWriteEnable(void);
#endif // _HI2C0_H
