/*
 * BMP180 sensor library, created using Atmel Studio for the AVR MCUs.
 *
 * Created by Dimitrios Stoupis at 09 December 2016.
 * Feel free to change and add/remove things from the library to make to suit your needs.
 * In order for the library to work perfectly, the custom made I2C library, TWI.h, is needed, otherwise change the I2C communication commands...
 * ...to suit your favorite I2C library that you are gonna be using.
 * 
 * For the sensor connection, connect the two interface wires, SCL and SDA to the corresponding pins of your Atmel AVR MCU.
 * The communication between the sensor and the MCU, is possible only by connecting the I2C wires of the sensor to the correct I2C interface ports at the MCU.
 * 
 * The sensor data sheet can be found on the page: https://cdn-shop.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf, link from December 2016.
 */ 

#ifndef BMP180_H_ //Check if the name is already defined to avoid duplicates
#define BMP180_H_ //If the name is not defined insert all the following things

#ifndef F_CPU //In case you forget to define the clock speed, define it here
#define F_CPU 8000000UL //Change the clock speed according to yours
#endif

#include <avr/io.h> //A needed library in all projects, containing crucial things for the MCUs
#include <util/delay.h> //Library that is used for the delays in functions
#include <math.h> //Include the library for math operations
#include "TWI.h" //The custom I2C communication library, change it if you use other and keep in mind to also change the functions accordingly

//Calibration parameter registers
#define FIRST_CALIB_REG_ADDR 0xAA //The memory address of the first register of the calibration values

//Calibration parameter register index values for the calibration_values array
#define AC1 0
#define AC2 1
#define AC3 2
#define AC4 3
#define AC5 4
#define AC6 5
#define B1 6
#define B2 7
#define MB 8
#define MC 9
#define MD 10

//Data write and read registers and commands
#define TEMP_READ_UNCL_MSB 0xF6
#define PRESS_READ_UNCL_MSB 0xF6
#define RAW_VALUE_READ_REGISTER 0xF4
#define TEMP_READ_COMMAND 0x2E
#define PRESS_READ_COMMAND 0x34 //Basic pressure read command

//Pressure resolution and condition parameters
#define PRESS_RESOLUTION 3 //Set the pressure resolution with 3 the highest possible, from 0 to 3, incrementing by 1
#define PRESS_AVERAGING_ENABLE 0 //Enable (1) or disable (0) the pressure averaging
#define PRESS_AVERAGING_SAMPLES 50 //The number of pressure samples to be averaged, if averaging is enabled

//General functional parameters selection
#define BMP180_TWI_INIT 0 //Set to (0) if you want to explicitly initialize the I2C interface, otherwise set to (1)
#define BMP180_AUTOUPDATETEMP 1 //If you want a temperature auto update for the pressure calibration parameter set to (1)

//Device address and calibrated addresses
#define BMP180_ADDR 0x77 //Address of the BMP sensor
#define BMP180_READ 0xEF //Calibrated address of the sensor to include the read bit
#define BMP180_WRITE 0xEE //Calibrated address of the sensor to include the write bit

//Global variables and arrays used in functions and calculations
int16_t calibration_values[11]; //Array to store the initial calibration values read from the memory
int32_t _B5; //This variable is used both in temp and press calibration
uint32_t _ac4_reg; //Save the AC4 register calibration value

/*
* extern functions are the ones for the end user
* The functions that are used internally to make things simpler, are not included in the header file
* If you want to use an internal function, just declare it here
*/
extern void BMP180_Init(void); //BMP180 Initialization function
extern int16_t BMP180_Get_Temp(void); //Get the temperature as an integer multiplied by 10
extern double BMP180_Get_Celcius_Temp(void); //Get the decimal temperature in Celsius
extern int32_t BMP180_Get_Pressure(void); //Get the pressure in Pascal
extern double BMP180_Get_hPa_Press(void); //Get the hPa value of the pressure
extern double BMP180_Absolute_Altitude(double sea_level_press); //Calculate the altitude in meters providing the sea level pressure in hPa
extern double BMP180_Sea_Level_Press(double altitude); //Calculate the sea level pressure in hPa providing the altitude in meters

#endif
