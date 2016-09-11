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
* @file                           MCP7940M.h
* @brief                          Contains definitions and macros related to MCP7940M hardware for arduino platform.
								  Tested platform version: Arduino 1.0.4 and later. This library is more specific to MCP7940M RTC.
								  
* @version                        0.1.0
* @date                           2014/02/05
*
*/

#ifndef _MCP7940M_H_
#define _MCP7940M_H_

/* Design for FT_RTC library */ 
//assuming wire library is working fine in arduino platform

/* RTC specific macros */
#define FT_RTC_I2C_ADDRESS     	(0x6F)		//7 bit i2c address and last bit is for read/write information
#define FT_RTC_I2C_BURST_SIZE  	(28)		//maximum burst size depending on the platform
#define FT_RTC_MFP  			(2)			//multi function pin

/* Macros wrt RTC address */
#define FT_RTCADDR_SECONDS		0x00
#define FT_RTCADDR_MINUTES		0x01
#define FT_RTCADDR_HOURS		0x02
#define FT_RTCADDR_DAY			0x03
#define FT_RTCADDR_DATE			0x04
#define FT_RTCADDR_MONTH		0x05
#define FT_RTCADDR_YEAR			0x06

/* Macro for bit location */
#define FT_RTC_HFORMAT_BIT 		0x40
#define FT_RTC_STARTSTOPBIT 	0x80

/* Macros for Hour format */
#define FT_RTC_HFORMAT_12H		0x01
#define FT_RTC_HFORMAT_24H		0x00

#endif /* _MCP7940M_ */

