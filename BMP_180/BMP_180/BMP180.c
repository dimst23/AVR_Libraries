#include "BMP180.h" //Include the definitions of functions and many other necessary things

/*
* Write the desired command to the sensor via I2C
* Provide the address of the command register and then provide the command to be sent via I2C
*/
inline void BMP180_Send_Command(uint8_t command_register, uint8_t command)
{
	while ((TWCR & (1 << TWINT))); //Wait if the bus is busy
	TWIStart(); //Send a start condition to initialize communication
	TWIWrite(BMP180_WRITE); //Send the write command to the sensor memory
	TWIWrite(command_register); //Send the register address for the command to be sent
	TWIWrite(command); //The wanted command is sent
	TWIStop(); //Stop the I2C communication
}

/*
* Read the bytes sent from the sensor via I2C
* Provide the address of the read register and also provide the number of the expected bytes and the array for the read bytes to be saved
*/
void BMP180_Read_Bytes(uint8_t registe, uint8_t *byte_read, uint8_t byte_count)
{
	while ((TWCR & (1 << TWINT))); //Wait if the bus is busy
	TWIStart(); //Send a start condition to initialize communication
	TWIWrite(BMP180_WRITE); //Send the write command to the sensor memory
	TWIWrite(registe); //Send the register you want
	
	//TWI Restart condition
	TWIStop();
	TWIStart();
	
	TWIWrite(BMP180_READ); //Send the read command to the sensor
	for(uint8_t i = 0; i < byte_count; i++) //and read the returned bytes
	{
		if (i == byte_count - 1) {byte_read[i] = TWIReadNACK();} //When the last byte is to be received, don't acknowledge to the slave
		else {byte_read[i] = TWIReadACK();} //If the bytes that are expected are more than one, send an acknowledgment to the slave
	}
	TWIStop(); //Stop the I2C communication
}

/*
* Read the calibration parameters from the BMP memory
*/
void BMP180_Get_Calibration_Params(void)
{
	uint8_t reg_read = FIRST_CALIB_REG_ADDR; //Set the address of the first parameter register
	uint8_t temp_value[2]; //Store the read MSB at index (0) and LSB at index (1) temporarily
		
	for(uint8_t i = 0; i < 11; i++)
	{
		if(reg_read == 0xB0) //Store the AC4 value in a separate variable
		{
			BMP180_Read_Bytes(reg_read, temp_value, 2); //Expect to read two bytes form the corresponding register, first MSB and second LSB
			_ac4_reg = ((uint32_t)temp_value[0] << 8) | ((uint32_t)temp_value[1]); //Save the value of AC4 to the variable
			reg_read += 2; //Increment for the next register
			continue; //Continue the loop from here and on
		}
		BMP180_Read_Bytes(reg_read, temp_value, 2); //Expect to read two bytes form the corresponding register, first MSB and second LSB
		calibration_values[i] = ((int16_t)temp_value[0] << 8) | ((int16_t)temp_value[1]); //Save the read value to the corresponding place at the array
		reg_read += 2; //Increment to go to the next register
	}
}

/*
* Read the raw temperature value as the sensor has calculated and has it saved at its memory registers
*/
uint16_t BMP180_Read_Temp_Raw(void)
{
	uint8_t bytes[2]; //Array to store the returned bytes from the BMP180_Read_Bytes() function
	
	BMP180_Send_Command(RAW_VALUE_READ_REGISTER, TEMP_READ_COMMAND); //Send the command to tell the sensor to calculate the raw temperature value
	_delay_ms(5); //Delay 5ms, because the sensor takes a maximum of 4.5ms to make the measurement of the temperature
	BMP180_Read_Bytes(TEMP_READ_UNCL_MSB, bytes, 2); //Once measured, read the raw temperature value from the sensor's registers
	
	return (((uint16_t)bytes[0] << 8) | ((uint16_t)bytes[1])); //Return the raw value which is a 16-bit integer
}

/*
* Read the raw pressure value as the sensor has calculated and has it saved at its memory registers
*/
int32_t BMP180_Read_Press_Raw(void)
{
	//Array to store the bits read from the registers. In sequence, at index (0) is the MSB, at index (1) the LSB and at index (2) is the XLSB
	uint8_t bytes[3];
	
	/*
	* Send the command to tell the sensor to calculate the raw pressure value
	* and also calibrate the read command according to the selected value resolution (this part is the shifting, which is (PRESS_RESOLUTION << 6))
	*/
	BMP180_Send_Command(RAW_VALUE_READ_REGISTER, PRESS_READ_COMMAND + (PRESS_RESOLUTION << 6));
	_delay_ms(2 + (3 << PRESS_RESOLUTION)); //Delay a set amount of time, according to the selected value resolution
	BMP180_Read_Bytes(PRESS_READ_UNCL_MSB, bytes, 3); //And read the measured raw pressure value
	
	return ((((int32_t)bytes[0] << 16) | ((int32_t)bytes[1] << 8) | ((int32_t)bytes[2])) >> (8 - PRESS_RESOLUTION)); //Return the read value
}

/*
* If pressure averaging is enabled, make a function for that reason
*/
#if PRESS_AVERAGING_ENABLE
int32_t BMP180_Averaging_Filter(void)
{
	int32_t sum_UP = 0, sum_B5 = 0; //Variables to hold the sum for averaging
	
	for (uint8_t i = 0; i < PRESS_AVERAGING_SAMPLES; i++)
	{
		sum_UP += BMP180_Read_Press_Raw(); //Sum of the raw pressure value read from the sensor register
		BMP180_Get_Temp(); //Call the function in order to calculate B5
		sum_B5 += _B5; //Sum the B5 parameter for averaging
	}
	_B5 = sum_B5/PRESS_AVERAGING_SAMPLES; //Find the average of B5 and save it at B5 global variable
	return (sum_UP/PRESS_AVERAGING_SAMPLES); //Return the averaged raw pressure value
}
#endif

/*
* Initialize the needed parameters and read the calibration parameters from the sensor
*/
void BMP180_Init(void)
{
	#if BMP180_TWI_INIT //If the automatic initialization of the I2C interface is enabled do the following
		TWIInit(); //Initialize the I2C interface of the MCU
		_delay_us(10); //Delay 10 micro seconds to give some time to the MCU for the I2C interface startup
	#endif
	
	BMP180_Get_Calibration_Params(); //Get the calibration parameters of the sensor
	BMP180_Get_Temp(); //Get a temperature measurement to initialize B5
}

/*
* Function that returns the true temperature read from the sensor
* The value returned is an integer with an accuracy of 0.1C, multiplied by 10, so if you want to obtain the decimal temperature divide by 10
*/
int16_t BMP180_Get_Temp(void)
{
	//Variables for the following calculations
	uint16_t UT = 0;
	int32_t X1 = 0;
	int32_t X2 = 0;
	
	//Calculate the true temperature value, according to the data sheet
	UT = BMP180_Read_Temp_Raw(); //Get the raw temperature value from the sensor
	X1 = ((((int32_t)UT - (int32_t)calibration_values[AC6]))*(int32_t)calibration_values[AC5]) >> 15; //Shifting right (n) times, is the same as dividing by (2^n)
	X2 = ((int32_t)calibration_values[MC] << 11)/(X1 + (int32_t)calibration_values[MD]); //And shifting left (n) times, is the same as multiplying by (2^n)
	_B5 = X1 + X2;
	
	return ((int16_t)((_B5 + 8) >> 4)); //Return the calculated true temperature value
}

/*
* Function that returns the true pressure value read from the sensor
* The value returned is an integer with an accuracy of 0.01Pa, multiplied by 100, so if you want to obtain the decimal pressure in hPa divide by 100
*/
int32_t BMP180_Get_Pressure(void)
{
	//Variables for the following calculations
	int32_t B6 = 0, X1 = 0, X2 = 0, X3 = 0, B3 = 0, pressure = 0, UP = 0;
	uint32_t B4 = 0, B7 = 0;
	
	#if BMP180_AUTOUPDATETEMP //If temperature auto update enabled...
		BMP180_Get_Temp(); //Get the temperature first to calculate variable B5 needed for the pressure calculation
	#endif
	
	#if PRESS_AVERAGING_ENABLE //If averaging enabled...
		UP = BMP180_Averaging_Filter(); //Get the averaged raw pressure value for further calculation
	#else //If averaging is not enabled...
		UP = BMP180_Read_Press_Raw(); //Just get the raw pressure value from the sensor one time
	#endif
	
	/*
	* Calculate the true pressure value, according to the data sheet
	* Shifting left and right (n) times is the same as multiplying and dividing by (2^n) accordingly
	*/
	B6 = _B5 - 4000;
	X1 = (calibration_values[B2]*(B6*B6) >> 12) >> 11;
	X2 = (calibration_values[AC2]*B6) >> 11;
	X3 = X1 + X2;
	B3 = (((((int32_t)calibration_values[AC1])*4 + X3) << PRESS_RESOLUTION) + 2) >> 2;
	X1 = (calibration_values[AC3]*B6) >> 13;
	X2 = (calibration_values[B1]*((B6*B6) >> 12)) >> 16;
	X3 = ((X1 + X2) + 2) >> 2;
	B4 = (_ac4_reg*((X3 + 32768))) >> 15;
	B7 = ((uint32_t)UP - B3)*(50000 >> PRESS_RESOLUTION);
	pressure = (B7 < 0x80000000) ? ((B7 << 1)/B4) : ((B7/B4) << 1);
	X1 = (pressure >> 8)*(pressure >> 8);
	X1 = (X1*3038) >> 16;
	X2 = (-7357*pressure) >> 16;
	pressure += ((X1 + X2 + 3791) >> 4);
	
	return (pressure);
}

/*
* Get the temperature value at its correct decimal form
*/
inline double BMP180_Get_Celcius_Temp(void)
{
	return ((double)BMP180_Get_Temp()/10.0);
}

/*
* Get the pressure value at hPa
*/
inline double BMP180_Get_hPa_Press(void)
{
	return ((double)BMP180_Get_Pressure()/100.0);
}

/*
* Get the altitude in meters, by providing the sea level pressure, which you may take from your local airport METAR
*/
inline double BMP180_Absolute_Altitude(double sea_level_press)
{
	return (44330.0*(1.0 - pow((BMP180_Get_hPa_Press()/sea_level_press), (1.0/5.255)))); //Calculate the altitude according to the data sheet
}

/*
* Get the sea level pressure in hPa, by providing the altitude in meters, which you may take from Google maps
*/
inline double BMP180_Sea_Level_Press(double altitude)
{
	return (BMP180_Get_hPa_Press()*pow((1.0 - altitude/44330.0), -5.255)); //Calculate the sea level pressure according to the data sheet
}