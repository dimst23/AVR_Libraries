
#ifndef TWI_H_
#define TWI_H_

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

#define TWI_FREQ 200000UL //Set the clock communication frequency to 200kHz

extern void TWIInit(void); //Initialize the I2C interface
extern void TWIStart(void); //Send a start signal
extern void TWIStop(void); //Send a stop signal
extern void TWIWrite(uint8_t u8data); //Send 8 bits of data
extern void TWIWrite16(uint16_t u16data); //Send 16 bits of data
extern uint8_t TWIReadACK(void); //Check if you received the acknowledgment from the other device
extern uint8_t TWIReadNACK(void); //Expect no acknowledgment
extern uint8_t TWIGetStatus(void); //Get the I2C status (Read the bits)

#endif