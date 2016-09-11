/*****************************************************************************
* Copyright (c) Future Technology Devices International 2014
* propriety of Future Technology devices International.
*
* Software License Agreement
*
* This code is provided as an example only and is not guaranteed by FTDI. 
* FTDI accept no responsibility for any issues resulting from its use. 
* The developer of the final application incorporating any parts of this 
* sample project is responsible for ensuring its safe and correct operation 
* and for any consequences resulting from its use.
*****************************************************************************/
/**
* @file                           FT_RTC.h
* @brief                          Contains RTC library interface of MCP7940M hardware for arduino platform.
								  Tested platform version: Arduino 1.0.4 and later. This library is more specific to MCP7940M RTC.
								  Dependencies: I2C wire library, Serial library
* @version                        0.1.0
* @date                           2014/02/05
*
*/

#ifndef _FT_RTC_H_
#define _FT_RTC_H_

//standard driver and hardware specific includes
#include "Wire.h"
#include "hardware/MCP7940M/MCP7940M.h"

/* Definitions used for debug. Uncomment the below to enable debug from this library */
#define FT_RTCDEBUGOFF 				255 //switch off debug
#define FT_RTCDEBUG1 				1 //most critical debug information
#define FT_RTCDEBUG2 				2 //mid critical debug information
#define FT_RTCDEBUG3 				3 //least critical debug information

/* Change the below statement wrt debug criticality */
#define FT_RTCDEBUG 				FT_RTCDEBUG1

/* Macros for RTC library version */
#define FT_RTCMAJOR 				(0)
#define FT_RTCMINOR 				(1)
#define FT_RTCBUILD 				(1)

/* Macros for time out */
#define FT_RTCMAXTIMEOUTCOUNTER		(1500)

/* Design for FT_RTC library */ 
//assuming wire library is working fine in arduino platform

/* Enums for RTC status */
typedef enum FT_RTCStatus 
{
	FT_RTC_OK = 0,
	FT_RTC_ERROR = 1,
	FT_RTC_RUNNING = 2,
	FT_RTC_STOPPED = 3,
	FT_RTC_ERROR_TIMEOUT = 4,	
}FT_RTCStatus;

/* Date time class for reading and writing date/time parameters to RTC */
class DateTime
{
public:
	uint8_t		hour:5;	//hours 0-11 or 0-23
	uint8_t		fampm:1;	//am pm format, 0 - am, 1 - pm
	uint8_t		hformat:2;	//hours, 0 for 24 hours format, 1 for 12 hours format
	uint8_t		min;	//minutes 0-59
	uint8_t		sec;	//seconds 0-59
	uint8_t		day;	//day of month 1-31
	uint8_t		mon;	//month 1-12
	uint16_t	year;	//year 0-99
	uint8_t		dow;	//day of week 1-7
	DateTime(void)
	{
	hour = 0;
	fampm = 0;	
	hformat = 0;
	min = 0;
	sec = 0;	
	day = 0;	
	mon = 0;	
	year = 0;
	dow = 0;
	};
};
class FT_RTC
{
	public:
	FT_RTC(void);//default i2c address id assigned
	FT_RTC(uint8_t i2caddr);//address of i2c rtc device
	~FT_RTC(void);
	void GetVersion(uint8_t &Major,uint8_t &Minor,uint8_t &Build);
	FT_RTCStatus Init(void);//open i2c driver
	FT_RTCStatus IsRunning();//check whether rtc is running
	FT_RTCStatus SetFormat(uint8_t HFormat);//set the date and time formats wrt rtc ic
	FT_RTCStatus GetDateTime(DateTime &DT);//get the date & time from rtc	
	FT_RTCStatus SetDateTime(const DateTime &DT);//set the date & time of rtc and starts the rtc
	FT_RTCStatus GetTime(uint8_t &HFormat,uint8_t &Hour,uint8_t &Minutes,uint8_t &Seconds);//get the time from rtc	
	//FT_RTCStatus SetTime(uint8_t HFormat,uint8_t Hour,uint8_t Minutes,uint8_t Seconds);//set the time of rtc
	
	//read, write memory
	FT_RTCStatus Read(uint32_t Addr,uint8_t &Value);//read one byte from Addr
	FT_RTCStatus Read(uint32_t Addr, uint8_t *Dst,uint8_t NBytes);//read n bytes
	FT_RTCStatus Write(uint32_t Addr,uint8_t Value);//write 1 byte of data
	FT_RTCStatus Write(uint32_t Addr,uint8_t *Src,uint8_t NBytes);//write n bytes of data - 1 to 255 bytes
	
	FT_RTCStatus Exit(void);//close i2c driver
	
	//utility apis
	uint8_t DecToBcd(uint8_t Value);
	uint8_t BcdToDec(uint8_t Value);
	
	uint8_t I2CAddr;//store the address of i2c
};


/**
 * @name    FT_RTC
 * @brief   Constructor of FT_RTC.
 * @ingroup FT_RTC
 *
 * Constructor of FT_RTC. Initializes few global parameters. Assigns default I2C address to the global
 *
 *
 * @retval None   
 *
 */
FT_RTC::FT_RTC(void)
{
	/* Assign the I2C address to the global */
	I2CAddr = FT_RTC_I2C_ADDRESS;	
}

/**
 * @name    FT_RTC
 * @brief   Constructor of FT_RTC.
 * @ingroup FT_RTC
 *
 * Constructor of FT_RTC. Initializes few global parameters. Assigns default I2C address to the global
 *
 *
 * @retval None   
 *
 */
FT_RTC::FT_RTC(uint8_t i2caddr)
{
	I2CAddr = i2caddr;
}

FT_RTC::~FT_RTC(void)
{
	/* do nothing */
}
/* Api to initialize wire library */
FT_RTCStatus FT_RTC::Init(void)
{
	/* Initialize i2c driver */
	Wire.begin();
	delay(100);//delay a bit to make sure the wire library is running fine
	return FT_RTC_OK;
}
/**
 * @name    IsRunning
 * @brief   Check the state of RTC
 * @ingroup FT_RTC
 *
 * Api to check the state of RTC, running or stopped.
 *
 *
 * @retval FT_RTC_RUNNING		Running state
 * @retval FT_RTC_STOPPED		Stopped state
 *
 */
FT_RTCStatus FT_RTC::IsRunning(void)
{
	/* Check the RTCC address 0 */
	uint8_t ReadByte = FT_RTC_OK;
	FT_RTCStatus Status;
	Status = Read(FT_RTCADDR_SECONDS,ReadByte);
	
	if(FT_RTC_OK != Status)
	{
		return FT_RTC_ERROR;
	}
	if((ReadByte&FT_RTC_STARTSTOPBIT)==0)
	{
		/* RTC is stopped */
		return FT_RTC_STOPPED;
	}

	/* RTC is running */
	return FT_RTC_RUNNING;
}

/* api to read 1 byte of value from Addr */
FT_RTCStatus FT_RTC::Read(uint32_t Addr,uint8_t &Value)
{
	int16_t Counter = 0;
	uint8_t writeResult;
	Wire.beginTransmission(I2CAddr);//i2c protocol
	/* address bytes for rtc are from 00 to 0xff */
	Wire.write(Addr);
	/* end the transmission but do not release the bus - usage is random data read use case from rtc */
	writeResult = Wire.endTransmission(false);//hold the bus to read the next data
	if (0 != writeResult)
	{
		return FT_RTC_ERROR;//error case
	}
	Wire.requestFrom((uint8_t)I2CAddr, (uint16_t)1);// request length bytes from slave device and end the transmission after this

	/* blocking call - at least one byte must be available */
	while(0 == Wire.available())
	{
		/* timeout logic - check for FT_RTCMAXTIMEOUTCOUNTER number of itterations and return error */
		if(Counter > FT_RTCMAXTIMEOUTCOUNTER)
		{
			return FT_RTC_ERROR_TIMEOUT;//return timout error
		}
		//delay(1);
		Counter++;
	}
	
	Value = Wire.read();
	return FT_RTC_OK;
}

/* Api to read n bytes of data from rtc */
#if 0
FT_RTCStatus FT_RTC::Read(uint32_t Addr, uint8_t *Dst,uint8_t NBytes)
{
  int16_t i,BlockCount,Counter = 0;
  short count = 0;
  uint8_t writeResult = 0;
  
	//nested - can lead to stack overflow
	while (NBytes > 0)
	{

		/* calculate the number of bytes to be read */
		BlockCount = NBytes>FT_RTC_I2C_BURST_SIZE?FT_RTC_I2C_BURST_SIZE:NBytes;

		Wire.beginTransmission(I2CAddr); // transmit to device (0x23)

		/* address bytes for rtc are from 00 to 0xff */
		Wire.write((uint8_t)Addr);        // sends value byte  

		/* end the transmission but do not release the bus - usage is random data read use case from rtc */
		writeResult = Wire.endTransmission(false);//hold the bus to read the next data

		if (0 != writeResult)
		{
			return FT_RTC_ERROR;//error case
		}

		Wire.requestFrom((uint8_t)I2CAddr, (uint16_t)BlockCount);// request NBytes bytes from slave device and end the transmission after this
		for(i=0;i<BlockCount;i++)
		{
			/* blocking call - at least one byte must be available */
			/* need to consider time out here */
			while(0 == Wire.available())
			{
				/* timeout logic - check for FT_RTCMAXTIMEOUTCOUNTER number of itterations and return error */
				if(Counter > FT_RTCMAXTIMEOUTCOUNTER)
				{
					return FT_RTC_ERROR_TIMEOUT;//return timout error
				}		
				Counter++;
				//delay(1);
			}
			*Dst++ = Wire.read();
		}
		/* Increment according to read size */
		Addr += BlockCount;
		NBytes -= BlockCount;

	}
  return FT_RTC_OK;
}
#else
FT_RTCStatus FT_RTC::Read(uint32_t addr, uint8_t *buffer,uint8_t length)
{

  uint16_t i,Counter = 0;
  uint8_t writeResult = 0;
   while (length > FT_RTC_I2C_BURST_SIZE)
  {
  //recursive function
    Read(addr,buffer,FT_RTC_I2C_BURST_SIZE);
    buffer += FT_RTC_I2C_BURST_SIZE;
    addr += FT_RTC_I2C_BURST_SIZE;
    length -= FT_RTC_I2C_BURST_SIZE;
  }
  
  Wire.beginTransmission(I2CAddr); // transmit to device (0x23)
  
 /* address bytes for rtc are from 00 to 0xff */
  Wire.write(addr);        // sends value byte  
  
  /* end the transmission but do not release the bus - usage is random data read use case from rtc */
  writeResult = Wire.endTransmission(false);//hold the bus to read the next data

  if (0 != writeResult)
  {
    return FT_RTC_ERROR_TIMEOUT;//error case
  }

  Wire.requestFrom((int16_t)I2CAddr, (int16_t)length);// request length bytes from slave device and end the transmission after this
  for(i=0;i<length;i++)
  {
	/* blocking call - at least one byte must be available */
	while(0 == Wire.available())
	{
		/* timeout logic - check for FT_RTCMAXTIMEOUTCOUNTER number of itterations and return error */
		if(Counter > FT_RTCMAXTIMEOUTCOUNTER)
		{
			return FT_RTC_ERROR_TIMEOUT;//return timout error
		}		
		Counter++;
	}    
	buffer[i] = Wire.read();
  }
  
  return FT_RTC_OK;
}
#endif
/* API to write data into particular location */
FT_RTCStatus FT_RTC::Write(uint32_t Addr, uint8_t Value)
{  
  /* for read the lower bit must be set to 1 and for write set to 0 */
  Wire.beginTransmission(I2CAddr);

  /* address bytes for rtc are from 00 to 0xff */
  Wire.write(Addr);        // sends value byte    
  Wire.write(Value);//send the data to slave


  /* end the transmission by stop bit */  
  if(0 != Wire.endTransmission())
  {
    return FT_RTC_ERROR;
  }
  return FT_RTC_OK;
}

/* API to write data N bytes into particular location */
FT_RTCStatus FT_RTC::Write(uint32_t Addr,uint8_t *Src,uint8_t NBytes)
{
  uint16_t i;

  if(0 == NBytes)
  {
    return FT_RTC_ERROR;
  }
  
  /* for read the lower bit must be set to 1 and for write set to 0 */
  Wire.beginTransmission(I2CAddr);

  /* address bytes for rtc are from 00 to 0xff */
  Wire.write(Addr);        // sends value byte  
  
  /* check for each byte */
  for(i=0;i<NBytes;i++)
  {
    Wire.write(*Src++);//send the data to slave
  }

  /* end the transmission by stop bit */  
  if(0 != Wire.endTransmission())
  {
    return FT_RTC_ERROR;
  }
  return FT_RTC_OK;
}

//utility api to convert decimal number into bcd
uint8_t FT_RTC::DecToBcd(uint8_t Value)
{
  return ( (Value/10)*16 + (Value%10) );
}
//utility api to convert bcd to decimal
uint8_t FT_RTC::BcdToDec(uint8_t Value)
{
  return ( (Value>>4)*10 + (Value&0xf) );
}

/* get the date and time */
FT_RTCStatus FT_RTC::GetDateTime(DateTime &DT)
{
	uint8_t TempBuff[7];//temp buffer
	FT_RTCStatus Status;
	/* Read all the registers from RTC */
	Status = Read(FT_RTCADDR_SECONDS,TempBuff,7);
	
	if(Status)
	{
		return Status;
	}
	DT.sec 	= BcdToDec((TempBuff[0] & 0x7f));
	DT.min 	= BcdToDec((TempBuff[1] & 0x7f));
	//wrt 12/24 hours format calculate the time into 24 hours format
	if(TempBuff[2] & FT_RTC_HFORMAT_BIT)
	{
		//12 hour format
		DT.hour = BcdToDec((TempBuff[2] & 0x1f));
		DT.fampm = ((TempBuff[2])>>5)&0x01;
		DT.hformat = FT_RTC_HFORMAT_12H;
	}
	else
	{
		//24 hour format
		DT.hour = BcdToDec((TempBuff[2] & 0x3f));
		DT.fampm = 0;
		DT.hformat = FT_RTC_HFORMAT_24H;
	}
	DT.dow 		= BcdToDec((TempBuff[3]&0x7));
	DT.day 		= BcdToDec((TempBuff[4]&0x3f));
	DT.mon	 	= BcdToDec((TempBuff[5]&0x1f));
	DT.year 	= BcdToDec((TempBuff[6]&0xff));
	
	return 	Status;
}
/* api to set the date time and start rtc */
FT_RTCStatus FT_RTC::SetDateTime(const DateTime &DT)
{
	uint8_t TempBuff[7];
	FT_RTCStatus Status;
	
	//check for the hour format and make modifications accordingly
	TempBuff[0] = 0x80 | DecToBcd(DT.sec);//make sure the rtc is running
	TempBuff[1] = DecToBcd(DT.min);
	if(FT_RTC_HFORMAT_12H == DT.hformat)
	{
		TempBuff[2] = DecToBcd(DT.hour) | (DT.fampm<<5) | (DT.hformat<<6);
	}
	else
	{
		//24 hours format
		TempBuff[2] = DecToBcd(DT.hour)&0x3f;
	}
	TempBuff[3] = DecToBcd(DT.dow);
	TempBuff[4] = DecToBcd(DT.day);
	TempBuff[5] = DecToBcd(DT.mon);
	TempBuff[6] = DecToBcd(DT.year);
	Status = Write(FT_RTCADDR_SECONDS,TempBuff,7);	
	
	return Status;
}

/* api to set only the hour format */
/* read the time from rtc and convert according to HFormat */
FT_RTCStatus FT_RTC::SetFormat(uint8_t HFormat)
{
	uint8_t hour,fampm,temp;
	FT_RTCStatus Status;
	
	Status = Read(FT_RTCADDR_HOURS,hour);
	if(Status)return Status;
	
	/* Depending on the format, adjust the time  */
	if(HFormat != ((hour >> 6)&0x01))
	{
		if(FT_RTC_HFORMAT_12H == HFormat)
		{
			temp = BcdToDec(hour&0x3f);
			fampm = temp>11?1:0;
				if(fampm) temp -= 12;
			hour = temp | (fampm << 5) | 0x40;
		}
		else
		{
			temp = BcdToDec(hour&0x1f);
			fampm = (hour>>5)&0x01;
			if(fampm)temp+=12;
			
			hour = temp;			
		}		
		
		//write the time wrt new format
		Status = Write(FT_RTCADDR_HOURS,hour);	
	}	
	//else do nothing
	
	return Status;
}
/* get the time from rtc. Output hours is always in terms of 24 hours */
FT_RTCStatus FT_RTC::GetTime(uint8_t &HFormat,uint8_t &Hour,uint8_t &Minutes,uint8_t &Seconds)
{
	uint8_t TempBuff[3];
	FT_RTCStatus Status;
	
	//read the three registers to get hformat, hour, minutes & seconds
	Status = Read(FT_RTCADDR_SECONDS,TempBuff,3);
	if(Status)return Status;

	/* update the parameters wrt rtc */
	Seconds	= BcdToDec((TempBuff[0] & 0x7f));
	Minutes	= BcdToDec((TempBuff[1] & 0x7f));	
	HFormat = (TempBuff[2]>>6)&0x01;
	if(HFormat)
		Hour = BcdToDec(TempBuff[2]&0x1f);
	else
		Hour = BcdToDec(TempBuff[2]&0x3f);
		
	return FT_RTC_OK;
	
}

/* do nothing as of now. free up any context which was allocated during Init api */
FT_RTCStatus FT_RTC::Exit(void)
{	
	return FT_RTC_OK;
}


#endif /* _FT_RTC_H_ */

