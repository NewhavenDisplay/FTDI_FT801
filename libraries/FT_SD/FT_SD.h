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
* @file                           FT_SD.h
* @brief                          Contains SD library for arduino platform.
								  Tested platform version: Arduino 1.0.4 and later
								  Dependencies: SPI library, Serial library
* @version                        0.1.0
* @date                           2014/02/05
*
*/

#ifndef _FT_SD_H_
#define _FT_SD_H_

/* Definitions used for debug. Uncomment the below to enable debug from this library */
#define FT_SDDEBUGOFF	 	255 //switchoff debug
#define FT_SDDEBUG1 		1 //most critical debug information
#define FT_SDDEBUG2 		2 //mid critical debug information
#define FT_SDDEBUG3 		3 //least critical debug information

/* Change the below statement wrt debug criticality */
#define FT_SDDEBUG FT_SDDEBUGOFF

/* Macros used for timeout delay */
#define FT_SD_TIMEOUTCOUNT			1500


#define FT_SD_FAT16 0
#define FT_SD_FAT32 1



/* design for sdcard */
/* Assuming that SPI library is working fine */
/* Default SPI pins are used which are mentioned in arduino website */
/* Limitations being, fat16/32, only 8.3 string name, no support for file create or file write api, sector based reading */


/* FT_SD library status enums */
typedef enum FT_SDStatus
{
	FT_SD_OK = 0,
	FT_SD_ERROR = 1,
	FT_SD_ERROR_FAT = 2,
	FT_SD_INVALIDFILE = 3,
	FT_SD_EOF = 4,
}FT_SDStatus;


typedef struct FT_DirEnt {
  int8_t Name[8];
  int8_t Ext[3];
  uint8_t Attribute;
  uint8_t Reserved[8];
  uint16_t Cluster_hi;  // FAT32 only
  uint16_t Time;
  uint16_t Date;
  uint16_t Cluster;
  uint32_t Size;
}FT_DirEnt;

/* Class for file based operations */
/* Limitations - only 1 file at a time */
//forward declaration
class FT_SD;

class FT_SDFile
{

	public:
	FT_SDFile()	{};//do nothing
	~FT_SDFile(){};
	/* File system related apis */
	//FT_SDStatus OpenFile(const char *FileName);
	void FileStart(FT_DirEnt &de);//Search for the file name
	FT_SDStatus Init(FT_DirEnt &de);
	void ReadSector(uint8_t *dst);//Read 512 bytes of data into destination
	void ReadSector();//Skip 512 bytes of data 
	void SeekSector(uint32_t Off);//Seek file to particular offset, offset should be multiple of 512 bytes
	void SkipSector();//Skip a sector
	void SkipCluster();//Skip the current cluster
	void NextCluster();//Goto the next cluster
	
	/* Global parameters maintaining file related information */
	uint32_t Cluster;
	uint32_t Offset;
	uint32_t Size;
	uint8_t Sector;	
	FT_SD *FTSD;
	
};

/* This library supports error reporting into the sdcard library */
/* Please note, error reporting is supported in constructor/Init/Exit. Read doesnt  */
class FT_SD
{
public:
	/* sdcard implementation related apis - internal apis wrt sd spec */
	FT_SD(void);
	FT_SD(uint8_t Pin);
	~FT_SD(void);
	FT_SDStatus Init();//initialize sdcard 
	FT_SDStatus Exit();//initialize sdcard 
	
	/* Read Apis */
	uint8_t Read(uint32_t Off);
	uint16_t Read16(uint32_t Off);
	uint32_t Read32(uint32_t Off);
	void ReadN(uint8_t *dst, uint32_t Off, uint16_t NBytes);	
	
	/* SD spec related apis */
	void Sel(void);//spi cs enable
	void DeSel(void);//spi cs disable
	void SDDelay(uint8_t N);//delay of N*8 clocks
	void Cmd(uint8_t Cmd, uint32_t Lba, uint8_t Crc);//sd card specific command	
	FT_SDStatus R1(uint8_t &r);//response 1 from sdcard controller
	FT_SDStatus SDR7(uint8_t &r);//response 7 from sdcard controller
	FT_SDStatus SDR3(uint32_t &OCR,uint8_t &r);//response 3 from sdcard controller
	void Cmd17(uint32_t Off);//command 17 to sdcard controller	
	void AppCmd(uint8_t CC, uint32_t LBA);//sd spec appcmd
	
	/* File operations */
	FT_SDStatus OpenFile(FT_SDFile &SdFile,const char *FileName);//open a file and update SDFile handle
	uint8_t CSPin;
	uint8_t CCS;

	uint8_t Type;
	uint16_t SectorsPerCluster;
	uint16_t ReservedSectors;
	uint16_t MaxRootDirEntries;
	uint16_t SectorsPerFat;
	uint16_t ClusterSize;
	uint32_t RootDirFirstCluster;

	// global variables to maintain fat
	uint32_t OPartition;
	uint32_t OFat;
	uint32_t ORoot;
	uint32_t OData;	
};




FT_SD::FT_SD(void)
{
	/* assign default pin assignment */
	CSPin = FT_SD_CSPIN;
}
FT_SD::FT_SD(uint8_t Pin)
{
	/* assign CS pin */
	CSPin = Pin;
}

FT_SD::~FT_SD(void)
{
	/* Close the SPI channel */
	/* Make sure the other modules are not using the SPI channel */
	SPI.end();
}

/* API for CS */
void FT_SD::Sel(void)
{ 
	digitalWrite(CSPin, LOW); 
}

/* API for CS disable */
void FT_SD::DeSel(void)
{ 
	digitalWrite(CSPin, HIGH); 
	SPI.transfer(0xff); //extra clock for sd card controller
}
/* API to insert n dummy clocks */
void FT_SD::SDDelay(uint8_t N) {
	while (N--)
		SPI.transfer(0xff);
}

/* SD spec related apis */
void FT_SD::Cmd(uint8_t Cmd, uint32_t Lba = 0, uint8_t Crc = 0x95)
{
	Sel(); 
	SPI.transfer(0xff);//extra clock cycles
	SPI.transfer(0x40 | Cmd);
	SPI.transfer(0xff & (Lba >> 24));
	SPI.transfer(0xff & (Lba >> 16));
	SPI.transfer(0xff & (Lba >> 8));
	SPI.transfer(0xff & (Lba));
	SPI.transfer(Crc); 
	SPI.transfer(0xff);//extra clock cycles
}

// read response R1
FT_SDStatus FT_SD::R1(uint8_t &r) 
{
	int16_t i = FT_SD_TIMEOUTCOUNT;
	while ((r = SPI.transfer(0xff)) & 0x80)
	{
	  i--;
	  if(i <= 0)
	  {
		/* Timeout error */
		DeSel();
		SPI.transfer(0xff);   // trailing byte
		return FT_SD_ERROR;
	  }
	}

	DeSel();
	SPI.transfer(0xff);   // trailing byte
	return FT_SD_OK;
}
  // read response R3
FT_SDStatus FT_SD::SDR3(uint32_t &ocr,uint8_t &r) 
{
	int16_t i = FT_SD_TIMEOUTCOUNT;
	while ((r = SPI.transfer(0xff)) & 0x80)
	{
	  i--;
	  if(i <= 0)
	  {
		DeSel();
		SPI.transfer(0xff);   // trailing byte
		return FT_SD_ERROR;
	  }
	}
	for (uint8_t i = 4; i; i--)
	  ocr = (ocr << 8) | SPI.transfer(0xff);
	SPI.transfer(0xff);   // trailing byte

	DeSel();
	return FT_SD_OK;
}

FT_SDStatus FT_SD::SDR7(uint8_t &r) {  // read response R7

	int16_t i = FT_SD_TIMEOUTCOUNT;

	while ((r = SPI.transfer(0xff)) & 0x80)
	{
	  i--;
	  if(i <= 0)
	  {
		DeSel();
		SPI.transfer(0xff);   // trailing byte
		return FT_SD_ERROR;
	  }
	}
	for (uint8_t i = 4; i; i--)
	{
	  SPI.transfer(0xff);
	}
	DeSel();

return FT_SD_OK;
}

//application command
void FT_SD::AppCmd(uint8_t CC, uint32_t LBA = 0)
{
	uint8_t tmpr;
	Cmd(55); R1(tmpr);
	Cmd(CC, LBA);
}

FT_SDStatus FT_SD::Init(void) 
{
	/* Initialize SPI library */
	pinMode(CSPin, OUTPUT);
	digitalWrite(CSPin, HIGH);    
	delay(100);
	SPI.begin();
	
#if (FT_SDDEBUG <= FT_SDDEBUG1)	
	Serial.print("CSPin no ");
	Serial.println(CSPin,DEC);
#endif
	
#if defined(__SAM3X8E__)
	SPI.setClockDivider(255);//hardcoding for due board
#else	
	/* handle due, yue and other hardware */
	SPI.setClockDivider(SPI_CLOCK_DIV64);
#endif	

	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);

	DeSel();
	//Sel();//need to cross check this

	delay(10);      // wait for boot
	SDDelay(10);   // deselected, 80 pulses

	  
	uint8_t r1,rlres;
	int16_t loopidx = FT_SD_TIMEOUTCOUNT;
	do {       // reset, enter idle
	  Cmd(0);
	  rlres = R1(r1);
	  if(rlres)
	  {
		Serial.println("error in R1");
		/* error case timeout */
		return FT_SD_ERROR;
	  }
	  loopidx--;
	  if(loopidx <= 0)
	  {
		Serial.println("timeout cmd 0");
		return FT_SD_ERROR;
	  }
	} while (r1 != 1); 

#if (FT_SDDEBUG <= FT_SDDEBUG1)
	Serial.println("cmd 0 success");
#endif
	uint8_t sdhc = 0,Status;
	//SDDelay(1);//cross check this
	Cmd(8, 0x1aa, 0x87);
	//delay(100);
	Status = SDR7(r1);
	if(0 != Status)
	{
	  return FT_SD_ERROR;
	}
	sdhc = (r1 == 1);
#if (FT_SDDEBUG <= FT_SDDEBUG1)
	Serial.println("cmd 8 success");
#endif

	{
	//check whether redundant
	  Cmd(58);
	  uint32_t OCR;
	  uint8_t r,Status;
	  Status = SDR3(OCR,r);
	  if(Status)
	  {
		return FT_SD_ERROR;
	  }	 
	}
#if (FT_SDDEBUG <= FT_SDDEBUG1)
	Serial.println("cmd 58 success");
#endif
	
	while (1) {
	  uint8_t StatusR1;
	  AppCmd(41, sdhc ? (1UL << 30) : 0); // card init
	  StatusR1 = R1(r1);
	  if ((r1 & 1) == 0)
		break;
		if(StatusR1)
		{
		  /* return error */
		  return FT_SD_ERROR;
		}
	  delay(100);
	}


	if (sdhc) {
	  Cmd(58);
	  uint32_t OCR;
	  uint8_t r,Status;
	  Status = SDR3(OCR,r);
	  if(0!= Status)
	  {
		return FT_SD_ERROR;
	  }
	  CCS = 1UL & (OCR >> 30);
	} else {
	  CCS = 0;
	}	 
#if (FT_SDDEBUG <= FT_SDDEBUG1)
	Serial.println("sdhc success");
#endif

	uint8_t type_code = Read(0x1be + 0x4);
	switch (type_code) {
	  default:
		Type = FT_SD_FAT16;
		break;
	  case 0x0b:
	  case 0x0c:
		Type = FT_SD_FAT32;
		break;
	}
	//Printf("Type code %#02x means FAT%d\n", type_code, (Type == FT_SD_FAT16) ? 16 : 32);
#if (FT_SDDEBUG <= FT_SDDEBUG1)
	Serial.println("read fat success");
#endif

	OPartition = 512L * Read32(0x1be + 0x8);
	SectorsPerCluster = Read(OPartition + 0xd);
	ReservedSectors = Read16(OPartition + 0xe);
	ClusterSize = 512L * SectorsPerCluster;

	//Printf("Bytes per Sector:    %d\n", Read16(OPartition + 0xb));
	//Printf("Sectors per Cluster: %d\n", SectorsPerCluster);

	if (Type == FT_SD_FAT16) {
	  MaxRootDirEntries = Read16(OPartition + 0x11);
	  SectorsPerFat = Read16(OPartition + 0x16);
	  OFat = OPartition + 512L * ReservedSectors;
	  ORoot = OFat + (2 * 512L * SectorsPerFat);
	  // data area starts with Cluster 2, so Offset it here
	  OData = ORoot + (MaxRootDirEntries * 32L) - (2L * ClusterSize); 
	} else {
	  uint32_t SectorsPerFat = Read32(OPartition + 0x24);
	  RootDirFirstCluster = Read32(OPartition + 0x2c);
	  uint32_t fat_begin_lba = (OPartition >> 9) + ReservedSectors;
	  uint32_t cluster_begin_lba = (OPartition >> 9) + ReservedSectors + (2 * SectorsPerFat);

	  OFat = 512L * fat_begin_lba;
	  ORoot = (512L * (cluster_begin_lba + (RootDirFirstCluster - 2) * SectorsPerCluster));
	  OData = (512L * (cluster_begin_lba - 2 * SectorsPerCluster));
	}
	/* After the success case, revert back the clock */
#if defined(__SAM3X8E__)
	SPI.setClockDivider(4);//hardcoding for due board
#else	
	/* any other arduino boards */
	SPI.setClockDivider(SPI_CLOCK_DIV2);
#endif	
	
	return  FT_SD_OK;
}
void FT_SD::Cmd17(uint32_t Off) {
	uint8_t StatusR1,r;
	if (CCS)
	  Cmd(17, Off >> 9);
	else
	  Cmd(17, Off & ~511L);
	StatusR1 = R1(r);    
	Sel();
	while (SPI.transfer(0xff) != 0xfe)
	  ;
}
  
void FT_SD::ReadN(uint8_t *d, uint32_t Off, uint16_t n) {
	Cmd17(Off);
	uint16_t i;
	uint16_t bo = (Off & 511);
	for (i = 0; i < bo; i++)
	  SPI.transfer(0xff);
	for (i = 0; i < n; i++)
	  *d++ = SPI.transfer(0xff);
	for (i = 0; i < (514 - bo - n); i++)
	  SPI.transfer(0xff);
	DeSel();
}

  uint32_t FT_SD::Read32(uint32_t Off) {
    uint32_t r;
    ReadN((uint8_t*)&r, Off, sizeof(r));
    return r;
  }

  uint16_t FT_SD::Read16(uint32_t Off) {
    uint16_t r;
    ReadN((uint8_t*)&r, Off, sizeof(r));
    return r;
  }

  uint8_t FT_SD::Read(uint32_t Off) {
    uint8_t r;
    ReadN((uint8_t*)&r, Off, sizeof(r));
    return r;
  }


static void dos83(uint8_t dst[11], const char *ps)
{
  uint8_t i = 0;
  while (*ps) {
    if (*ps != '.')
      dst[i++] = toupper(*ps);
    else {
      while (i < 8)
        dst[i++] = ' ';
    }
    ps++;
  }
  while (i < 11)
    dst[i++] = ' ';
}
FT_SDStatus FT_SD::OpenFile(FT_SDFile &SdFile,const char *FileName)
//FT_SDStatus FT_SDFile::OpenFile(const char *filename) 
{
	int16_t i = 0;
	uint8_t dosname[11];
	FT_DirEnt de;
	
	/* update the sd class */
	SdFile.FTSD = this;
	dos83(dosname, FileName);
	do {
	  ReadN((uint8_t*)&de, ORoot + i * 32, sizeof(de));
	  // Serial.println(de.Name);
	  if (0 == memcmp(de.Name, dosname, 11)) {
		SdFile.FileStart(de);
		return FT_SD_OK;
	  }
	  i++;
	} while (de.Name[0]);
	return FT_SD_INVALIDFILE;
}
  
void FT_SDFile::FileStart(FT_DirEnt &de) 
{
	Size = de.Size;
	Cluster = de.Cluster;
	if (FTSD->Type == FT_SD_FAT32)
		Cluster |= ((int32_t)de.Cluster_hi << 16);
	Sector = 0;
	Offset = 0;
}
void FT_SDFile::NextCluster() 
{
	if (FTSD->Type == FT_SD_FAT16)
	  Cluster = FTSD->Read16(FTSD->OFat + 2L * Cluster);
	else
	  Cluster = FTSD->Read32(FTSD->OFat + 4L * Cluster);

}
void FT_SDFile::SkipCluster() 
{
	NextCluster();
	Offset += FTSD->ClusterSize;
}
void FT_SDFile::SkipSector() 
{
	if (Sector == FTSD->SectorsPerCluster) {
	  Sector = 0;
	  NextCluster();
	}
	Sector++;
	Offset += 512L;
}
void FT_SDFile::SeekSector(uint32_t Off) 
{
	while (Offset < Off) {
	  if ((Sector == FTSD->SectorsPerCluster) && ((int32_t)(Off - Offset) > (int32_t)FTSD->ClusterSize))
		SkipCluster();
	  else
		SkipSector();
	}
}
void FT_SDFile::ReadSector() 
{
	if (Sector == FTSD->SectorsPerCluster) {
	  Sector = 0;
	  NextCluster();
	}
	uint32_t off = FTSD->OData + ((int32_t)FTSD->ClusterSize * Cluster) + (512L * Sector);

	FTSD->Cmd17(off & ~511L);
	// Serial.println(2 * (micros() - t0), DEC);
	Sector++;
	Offset += 512L;
}
void FT_SDFile::ReadSector(uint8_t *dst) 
{
	ReadSector();
	for (uint8_t i = 0; i < 64; i++) {
		/* Read 8 bytes one shot */
	  *dst++ = SPI.transfer(0xff);
	  *dst++ = SPI.transfer(0xff);
	  *dst++ = SPI.transfer(0xff);
	  *dst++ = SPI.transfer(0xff);
	  *dst++ = SPI.transfer(0xff);
	  *dst++ = SPI.transfer(0xff);
	  *dst++ = SPI.transfer(0xff);
	  *dst++ = SPI.transfer(0xff);
	}
	SPI.transfer(0xff);   // consume CRC
	SPI.transfer(0xff);
	FTSD->DeSel();
}



#endif /* _FT_SD_H_ */


