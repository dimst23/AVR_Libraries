#include "DHT.h"

int16_t data[2] = {0}; //Array to store the received values

void DHT_Init(void)
{
	DHT_DDR &= ~(1 << DHT_PIN_NUM); //Set PORT to INPUT
	DHT_PORT |= 1 << DHT_PORTNU; //And pull it HIGH
}

int8_t DHT_Read_Data(void)
{
	uint8_t counter = 0; //Counter variable
	
	uint8_t bit = 0; //Save each byte received to this variable
	uint8_t calc_crc = 0; //Save the calculated CRC
	uint8_t rcvd_crc = 0; //Save the received CRC
	uint8_t temp_crc = 0; //A temporary variable for CRC operations
	
	DHT_PORT |= 1 << DHT_PORTNU; //And pull it HIGH
	_delay_ms(250); //Delay to give the sensor some time to stabilize
	
	//Start the communication procedure
	DHT_DDR |= (1 << DHT_PIN_NUM); //Set the Pin of the DHT to output
	DHT_PORT &= ~(1 << DHT_PORTNU); //Pull the DHT pin LOW
	_delay_ms(20); //Delay at least 1ms
	DHT_PORT |= 1 << DHT_PORTNU; //Pull the DHT pin HIGH
	_delay_us(40); //Delay 20 - 40us according to the data sheet
	DHT_DDR &= ~(1 << DHT_PIN_NUM); //Set PORT to input to start listening
	DHT_PORT |= 1 << DHT_PORTNU; //Enable the pull-up resistor at the PIN
	_delay_us(20); //Delay before checking the PIN to make sure we are in the phase
	
	while(!(DHT_PIN & (1 << DHT_PIN_NUM)) && (counter < 255)) //Wait until the PIN is HIGH or timeout
		counter++; //Increase the counter for timeout
	if (counter >= 255) //If timeout limit reached, exit from the function
		return 0;
	_delay_us(100); //Delay more than 80us in order to capture the bit that is sent
	
	//Get the humidity reading
	for(uint8_t i = 0; i < 16; i++)
	{
		counter = 0; //Reset the counter in each iteration
		while(!(DHT_PIN & (1 << DHT_PIN_NUM)) && (counter < 255)) //Wait until the PIN is HIGH or timeout
			counter++; 
		_delay_us(40); //Delay 40us to check...
		
		if((DHT_PIN & (1 << DHT_PIN_NUM)) && (counter < 255)) //...if PIN is still HIGH that means we have a bit of one (1)
		{
			bit = 1; //Save that PIN and...
			while((DHT_PIN & (1 << DHT_PIN_NUM)) && (counter < 255)) //...Wait until the PIN gets to LOW
				counter++;
		}
		else //If the pin got to LOW after the 40us delay the bit is for sure a zero bit
			bit = 0; //Save the received bit
		data[0] <<= 1; //Shift to save the received bit
		data[0] |= bit; //Save the received bit
		
		if (counter >= 255)
			break;
		
		//CRC checking sector
		temp_crc <<= 1;
		temp_crc |= bit; //Copy the received bit in the CRC variable
		
		if ((i + 1)%8 == 0) //Every 8-bits save the value of the received bits from the sensor....
		{
			calc_crc += temp_crc; //....and save and sum to CRC checksum
			temp_crc = 0; //Reset the temporary CRC value
		}
	}
	
	//Get the temperature reading
	for(uint8_t i = 0; i < 16; i++)
	{
		counter = 0; //Reset the counter in each iteration
		//Same procedure as above
		while(!(DHT_PIN & (1 << DHT_PIN_NUM)) && (counter < 255))
			counter++;
		_delay_us(40);
		
		if((DHT_PIN & (1 << DHT_PIN_NUM)) && (counter < 255))
		{
			bit = 1;
			while((DHT_PIN & (1 << DHT_PIN_NUM)) && (counter < 255))
				counter++;
		}
		else
			bit = 0;
		data[1] <<= 1;
		data[1] |= bit;
		
		if (counter >= 255)
			break;
		
		//CRC checking sector
		temp_crc <<= 1; 
		temp_crc |= bit; //Copy the received bit in the CRC variable
		
		if ((i + 1)%8 == 0) //Every 8-bits save the value of the received bits from the sensor....
		{
			calc_crc += temp_crc; //....and save and sum to CRC checksum
			temp_crc = 0; //Reset the temporary CRC value
		}
	}
	
	//Get the CRC checksum
	for(uint8_t i = 0; i < 8; i++)
	{
		counter = 0;
		while(!(DHT_PIN & (1 << DHT_PIN_NUM)) && (counter < 255))
			counter++;
		_delay_us(40);
		
		if((DHT_PIN & (1 << DHT_PIN_NUM)) && (counter < 255))
		{
			bit = 1;
			while((DHT_PIN & (1 << DHT_PIN_NUM)) && (counter < 255))
				counter++;
		}
		else
			bit = 0;
		rcvd_crc <<= 1;
		rcvd_crc |= bit;
		
		if (counter >= 255)
			break;
	}
	
	if (counter >= 255)
		return 0;
	
	if (calc_crc == rcvd_crc) //If CRC succeeds...
	{
		
		if(data[1] & 0x8000)
		{
			data[1] &= ~(0x8000);
			data[1] *= (-1);
		}
		return 1; //...return 1 to indicate it	
	}
		
	else
		return 0; //Otherwise send a zero to indicate failure
	_delay_ms(100);
}

void DHT_Humidity(uint16_t *Hum)
{
	if(DHT_Read_Data())
		*Hum = data[0];
	else
		*Hum = 1250; //Return a strange value to let know that the CRC failed
}

void DHT_Temperature(int16_t *Temp)
{
	if(DHT_Read_Data())
		*Temp = data[1];
	else
		*Temp = 1100; //Return a strange value to let know that the CRC failed

}

void DHT_GetMeteoData(int16_t *Temper, uint16_t *Humd)
{
	if(DHT_Read_Data())
	{
		*Humd = data[0];
		*Temper = data[1];
	}
	else
	{
		*Humd = 1250; //Return a strange value to let know that the CRC failed
		*Temper = 1100; //Return a strange value to let know that the CRC failed
	}
}