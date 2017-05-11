# BMP180_Library_Guide
In order to be able and use the BMP180 sensor library, the I2C or TWI interface library is needed, that is the reason is included with the pressure sensor library. There is no need for explicit inclusion of the TWI library, because it is included in the BMP180.

You have some options to enable or disable and some values to set ih the header file, and the options are:
1. Setting the automatic initizlization of the TWI interface, by setting **BMP180_TWI_INIT** as **1**
2. Autoupdating the temperature, before a pressure reading, by stetting the **BMP180_AUTOUPDATETEMP** to **1**
3. Enabling sample averaging by setting the **PRESS_AVERAGING_ENABLE** to **1** and then defining how many samples to average in the **PRESS_AVERAGING_SAMPLES**. 

You can also choose the resolution in the pressure reading by setting the **PRESS_RESOLUTION** to **0,1,2 or 3** with **3** being the highest resolution available by the sensor. Also note that increasing resolution, the sampling time in the sensor will increase (refer to the datasheet for detailed information).

The available functions along with a small description of their functionality are:
1. **void BMP180_Init(void);**
   
   This function initializes the BMP180 sensor throught the TWI interface, making it ready for measurments.
2. **int16_t BMP180_Get_Temp(void);**
   
   The function returns the temperature as read by the sensor, but the temperature format is the actual temperature in Celcius, multiplied by 10.
3. **double BMP180_Get_Celcius_Temp(void);**
   
   The job of this function is to read and return the temperature as floating point number, with a 0.1C precision as provided by the sensor.
4. **int32_t BMP180_Get_Pressure(void);**
   
   The function returns the read pressure from the sensor in Pascal.
5. **double BMP180_Get_hPa_Press(void);**
   
   This function reads, converts and returns the pressure from the sensor in hPa.
6. **double BMP180_Absolute_Altitude(double sea_level_press);**
   
   Using this function you can calculate the altitude, from the current pressure reading, by providing the current sea level pressure at the location in that moment.
7. **double BMP180_Sea_Level_Press(double altitude);**
   
   This function provides a calculation of the local sea level compensated pressure, or *QNH*, providing the altitude from the sea level of the current location.

* ***Note:*** Using functions 6 and/or 7 makes the program more memory intensive, meaning it requires more flash and ram, because of the math functions called in these function. If there are memory constraints in the project, the use of these functions should be avoided.

* ***One final note:*** The TWI library was writen for the ATmega644p AVR and the registers used are for that AVR, if your AVR is a different one, it is recomended to first look at its datasheet in the TWI or I2C section and check if the resigters match. If they do match you can use it as is, otherwise you need to modify the coreponding areas.

You can find the sensor datasheet at: https://cdn-shop.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf
