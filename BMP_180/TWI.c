#include "TWI.h"

void TWIInit(void)
{
	//set SCL to 400kHz
	TWSR = 0x00;
	TWBR = ((F_CPU/TWI_FREQ) - 16)/2;
	//enable TWI
	TWCR = (1<<TWEN);
}

void TWIWrite(uint8_t u8data)
{
	TWDR = u8data;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
}

void TWIWrite16(uint16_t u16data)
{
	TWDR = u16data & 0xF0; //Send the first byte
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
	
	TWDR = u16data & 0x0F; //Send the second byte
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
}

void TWIStart(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
}

void TWIStop(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}

uint8_t TWIReadACK(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while ((TWCR & (1<<TWINT)) == 0);
	return TWDR;
}

uint8_t TWIReadNACK(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(0<<TWEA);
	while ((TWCR & (1<<TWINT)) == 0);
	return TWDR;
}

uint8_t TWIGetStatus(void)
{
	uint8_t status;
	status = TWSR & 0xF8; //Mask status
	return status;
}