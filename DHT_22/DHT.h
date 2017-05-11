#ifndef DHT_H_
#define DHT_H_

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

#define DHT_DDR DDRC //Set the DDR that the DHT is on the specific PORT
#define DHT_PORT PORTC //Set the PORT that the sensor is on
#define DHT_PIN PINC //Set the PIN that the DHT is on
#define DHT_PORTNU 2 //Set the PORT number
#define DHT_PIN_NUM 2 //And the PIN number

extern void DHT_Init(void); //Initialize the sensor
extern void DHT_Humidity(uint16_t *Hum); //Save the humidity to a pointer
extern void DHT_Temperature(int16_t *Temp); //Save the temperature to a pointer
extern void DHT_GetMeteoData(int16_t *Temper, uint16_t *Humd); //Save both humidity and temp to the provided pointers

#endif