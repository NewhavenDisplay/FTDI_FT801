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
* @file                           ReadMe.txt
* @brief                          Contains information regarding the release, version significance and testing done.
				  Tested platform version: Arduino 1.0.4 and later
* @version                        1.4.0
* @date                           2015/03/27
*
*/

Objective:
------------
This document captures version significance, folder structure, testing information etc of FTDI specific graphics controllers library and respective examples.


Version Significance:
-------------------------
Version 1.4.0 - Mar/27/2015:
	-Added demo application implements swipe lists, as found on tablets and mobile phones. Located in "examples\FT_VM800P43_50\Intermediate\Swipe" folder.

Version 1.3.0 - Jun/24/2014:
	-Added clocktrimming to all platforms.
	-Overloaded init functions to be able to specify clock source.
	-New FT_GC options type, FT_GCOptions, to specify internal clock source.
	-Modified the internal DisplayConfigExternalClock() function, clock source and PCLK lines are now in the init function.
	-Added multitouch capable sketchs in 'intermediate' section for FT801 platforms:
		BouncingSquares
		BouncingCircles
		BouncingPoints
		MovingPoints
		MainMenu
	-Minor modifications to the following sketches to improve FT801 funtionalities:
		FT_App_Meter
		FT_App_Clocks
		FT_App_Gauges
	-Updated FT_App_MainMenu with multiple animated backgrounds and improved FT801 functionalites.

Version 1.2.0 - Apr/28/2014:
	- Added few more applications into Intermediate section.
	- Example applications to demonatrate graphics controller interrupts, BARGRAPH bitmap usage, and Streetmap.
	- Fixed a bug in the WriteCmdfromflash() function so it can now be used within the StartTransferCmd() and EndTransferCmd() continuous transfer block.
	- Fixed a bug in EnableInterrupts() function to utilize the correct register to enable interrupts.
	- Added missing template type definition in some platform header file.
	- Enhanced the library to initialize the interrupt pin.
	- Renamed ROM and RAM font table structure in the library from FT_Gpu_Fonts_t to FT_Fonts_t.


Version 1.1.0 
 	- Added few more applications into respective categories
 	- Example application to demonstrate standard sdlibrary instead of FT_SD.
 	- General optimizations wrt performance, code size, ram size etc.
 	- Restructuring of classes using templates instead of static classes. Code clean up of all the applications.
 	- Few of the remaining applications are ftclocks.
 	- Renamed FT_VM801P43.h to FT_VM801P43_50.h. Renamed .\FTDI\examples\FT_VM801P43 to .\FTDI\examples\FT_VM801P43_50
 	- Hardware pin assignment (interrupt pin) of VM80xP35/43/50 platform was updated.
	- bug fix in FT_GC - Vertex2ii api


Version 1.0.0 
 	- Added few more applications into respective categories
 	- bug fixs and code cleanup
 	- This release is before testing of 1.0.0
 	- Few of the remaining applications are restaurant, ftclocks.


Version 0.2.0 
 	- Added few more applications into respective categories
 	- This release is before testing of 1.0.0
 	- Few of the remaining applications are restaurant, ftclocks.


Version 0.1.1 
 	- version that contains most of the platforms - VM800P43_50, ADAM_4DLCD_FT843, Breakout_4DLCD_FT843/VM800B43_50, VM800B35, VM800P35, VM801B43, VM801P43
 	- Few of the applications porting is remaining.
 

Version Draft
 	- draft version that contains only few platforms


Known Issues and Limitations:
------------------------------
 - Finalizing folder structure and file naming convention. Please check whether FTDI, FT_GC needs to be changed to some other names such as FT_GPU etc.
 - Optimization wrt stack usage and statistics wrt performance, memory usage etc.
 - Code comment in doxygen format, file headers etc are yet to be done.
 - Version signification to be captured in respective ReadMe.txt files for library, examples, platforms etc.
 - Renaming of demo applications is to be done.


Supported platforms:
---------------------
 - NHD-4.3CTP-SHIELD - Newhaven Display 4.3" TFT Shield for Arduino w/ Capacitive Touch (FT801)
 - VM800P35 - FTDI 3.4" Plus board.
 - VM800P43_50 - FTDI 4.3" and 5.0" Plus board.
 - ADAM_4DLCD_FT843 – The 4DLCD_FT843 board can used with almost all Arduino boards that have ICSP pins.
 - BREAKOUTBOARD_4DLCD_FT843 – The 4DLCD_FT843 breakout board can be used with almost all Arduino boards that have ICSP pins.  This platform does not have SD card slot.
 - VM800B43_50 - This board can be used with almost all Arduino boards.  This platform does not have SD card slot.
 - VM800B35 - FTDI 3.5" Basic board – This board can be used with almost all Arduino boards. This platform does not have SD card slot.
 - VM800B43_50 - FTDI 4.3" and 5" Plus boards. This platform does not have SD card slot.
 - VM801B43 - FTDI 4.3" Basic board with FT801  – This board can be used with almost all Arduino boards.  This platform does not have SD card slot.
 - VM801P43_50 - FTDI 4.3" and 5" plus board with FT801.


Installation instructions:
---------------------------
 - Download and install the appropriate Arduino IDE for the targeting board by following the steps mentioned in the "AN813 FT800_Series_Arduino_Library" or at http://arduino.cc/en/guide/windows#.UzAxxqiSxHU 
 - Follow the steps in the "AN813 FT800_Series_Arduino_Library" or Unzip the FTDI.zip file and copy the FTDI folder into C:\Users\<user login name>\Documents\Arduino\libraries\ 
 - Open any of the sketches from C:\Users\<user login name>\Documents\Arduino\libraries\FTDI\examples\*. As of now most of the applications are implemented, we are in the process of implementing the rest.
 - Select the correct port number and board type in the IDE and sketches are ready to upload to the Arduino board. Also, make sure relevant data files are copied from SDCard folder,if the sample sketch folder contains one, onto the root level of the SD card storage (ex: \FTDI\examples\FT_VM800P43_50\Demos\FT_App_Jackpot\SDCard).


Setup instructions:
--------------------
For all the below, use arduino IDE 1.0.3 or later versions were used for development/testing.
 - For VM800P35/VM800P43_50/VM801P43 - use "Arduino pro or mini (5v,16mz) atmega 328" as board type in arduino IDE. 
 - For ADAM_4DLCD_FT843 - follow the instructions mentioned in ADAM datasheet - use the default jumper settings. 
 - For BREAKOUTBOARD_4DLCD_FT843, VM800B43_50,VM800B35,VM801B43 - follow the pin assignments mentioned in respective platform header file: FT_Breakout_4DLCD_FT843.h, FT_VM800B43_50, FT_VM800B35.h, FT_VM801B43.h. These platforms can be connected to almost all Arduino boards.  Make sure to choose the correct port number and board type in the IDE.


Folder Structure:
------------------
 - Root level (\FTDI) contains include file wrt platforms, libraries, hardware and examples. 
 - Libraries (\FTDI\libraries) contain source for SD card, RTC, FT800 implementation, FT801 implementation etc. FT_GC is the base class for FT800Impl and FT801Impl.
 - Hardware (\FTDI\hardware) contains hardware specific macros/instructions/registers etc
 - Each of the platforms shall contain respective examples (\FTDI\examples). Examples are split into (a)Basic (b)Intermediate (c)Advanced (d)Demos. NOTE – Sample application based on HAL has been splitted up and they were divided into a/b/c and demo applications are divided into c/d.


Testing done:
----------------
 - Arduino IDE versions used for the development and verification were: 1.0.4, 1.0.5, 1.5.5 on Windows 7 32bits and Windows 7 64bits.
 - All sample sketches were tested on all platforms in the "Supported platforms" section.  NOTE: The actual MCU board used for the sketch verification process is noted in the version checklist.

