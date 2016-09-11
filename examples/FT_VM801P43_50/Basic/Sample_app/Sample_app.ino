//this file contains the test application for FT801 plus board
#include "SPI.h"
#include "Wire.h"
#include "FT_VM801P43_50.h"
#include "SampleApp.h"
#include "SampleAppRawdata.h"



#define VERTEX2F(x,y) ((1UL<<30)|(((x)&32767UL)<<15)|(((y)&32767UL)<<0))
#define VERTEX2II(x,y,handle,cell) ((2UL<<30)|(((x)&511UL)<<21)|(((y)&511UL)<<12)|(((handle)&31UL)<<7)|(((cell)&127UL)<<0))
#define BITMAP_SOURCE(addr) ((1UL<<24)|(((addr)&1048575UL)<<0))
#define CLEAR_COLOR_RGB(red,green,blue) ((2UL<<24)|(((red)&255UL)<<16)|(((green)&255UL)<<8)|(((blue)&255UL)<<0))
#define TAG(s) ((3UL<<24)|(((s)&255UL)<<0))
#define COLOR_RGB(red,green,blue) ((4UL<<24)|(((red)&255UL)<<16)|(((green)&255UL)<<8)|(((blue)&255UL)<<0))
#define BITMAP_HANDLE(handle) ((5UL<<24)|(((handle)&31UL)<<0))
#define CELL(cell) ((6UL<<24)|(((cell)&127UL)<<0))
#define BITMAP_LAYOUT(format,linestride,height) ((7UL<<24)|(((format)&31UL)<<19)|(((linestride)&1023UL)<<9)|(((height)&511UL)<<0))
#define BITMAP_SIZE(filter,wrapx,wrapy,width,height) ((8UL<<24)|(((filter)&1UL)<<20)|(((wrapx)&1UL)<<19)|(((wrapy)&1UL)<<18)|(((width)&511UL)<<9)|(((height)&511UL)<<0))
#define ALPHA_FUNC(func,ref) ((9UL<<24)|(((func)&7UL)<<8)|(((ref)&255UL)<<0))
#define STENCIL_FUNC(func,ref,mask) ((10UL<<24)|(((func)&7UL)<<16)|(((ref)&255UL)<<8)|(((mask)&255UL)<<0))
#define BLEND_FUNC(src,dst) ((11UL<<24)|(((src)&7UL)<<3)|(((dst)&7UL)<<0))
#define STENCIL_OP(sfail,spass) ((12UL<<24)|(((sfail)&7UL)<<3)|(((spass)&7UL)<<0))
#define POINT_SIZE(size) ((13UL<<24)|(((size)&8191UL)<<0))
#define LINE_WIDTH(width) ((14UL<<24)|(((width)&4095UL)<<0))
#define CLEAR_COLOR_A(alpha) ((15UL<<24)|(((alpha)&255UL)<<0))
#define COLOR_A(alpha) ((16UL<<24)|(((alpha)&255UL)<<0))
#define CLEAR_STENCIL(s) ((17UL<<24)|(((s)&255UL)<<0))
#define CLEAR_TAG(s) ((18UL<<24)|(((s)&255UL)<<0))
#define STENCIL_MASK(mask) ((19UL<<24)|(((mask)&255UL)<<0))
#define TAG_MASK(mask) ((20UL<<24)|(((mask)&1UL)<<0))
#define BITMAP_TRANSFORM_A(a) ((21UL<<24)|(((a)&131071UL)<<0))
#define BITMAP_TRANSFORM_B(b) ((22UL<<24)|(((b)&131071UL)<<0))
#define BITMAP_TRANSFORM_C(c) ((23UL<<24)|(((c)&16777215UL)<<0))
#define BITMAP_TRANSFORM_D(d) ((24UL<<24)|(((d)&131071UL)<<0))
#define BITMAP_TRANSFORM_E(e) ((25UL<<24)|(((e)&131071UL)<<0))
#define BITMAP_TRANSFORM_F(f) ((26UL<<24)|(((f)&16777215UL)<<0))
#define SCISSOR_XY(x,y) ((27UL<<24)|(((x)&511UL)<<9)|(((y)&511UL)<<0))
#define SCISSOR_SIZE(width,height) ((28UL<<24)|(((width)&1023UL)<<10)|(((height)&1023UL)<<0))
#define CALL(dest) ((29UL<<24)|(((dest)&65535UL)<<0))
#define JUMP(dest) ((30UL<<24)|(((dest)&65535UL)<<0))
#define BEGIN(prim) ((31UL<<24)|(((prim)&15UL)<<0))
#define COLOR_MASK(r,g,b,a) ((32UL<<24)|(((r)&1UL)<<3)|(((g)&1UL)<<2)|(((b)&1UL)<<1)|(((a)&1UL)<<0))
#define CLEAR(c,s,t) ((38UL<<24)|(((c)&1UL)<<2)|(((s)&1UL)<<1)|(((t)&1UL)<<0))
#define END() ((33UL<<24))
#define SAVE_CONTEXT() ((34UL<<24))
#define RESTORE_CONTEXT() ((35UL<<24))
#define RETURN() ((36UL<<24))
#define MACRO(m) ((37UL<<24)|(((m)&1UL)<<0))
#define DISPLAY() ((0UL<<24))


uint32_t coprostatus;
uint16_t FT_DispWidth=480,DisplayWidth = 480, FT_DispHeight=272,DisplayHeight = 272,FT_DispPCLK = 8;


/* Optimized implementation of sin and cos table - precision is 16 bit */

#ifdef SAMAPP_ENABLE_APIS_SET0

PROGMEM  prog_uint16_t sintab[] = {
0, 402, 804, 1206, 1607, 2009, 2410, 2811, 3211, 3611, 4011, 4409, 4807, 5205, 5601, 5997, 6392, 
6786, 7179, 7571, 7961, 8351, 8739, 9126, 9511, 9895, 10278, 10659, 11038, 11416, 11792, 12166, 12539,
12909, 13278, 13645, 14009, 14372, 14732, 15090, 15446, 15799, 16150, 16499, 16845, 17189, 17530, 17868,
18204, 18537, 18867, 19194, 19519, 19840, 20159, 20474, 20787, 21096, 21402, 21705, 22004, 22301, 22594, 
22883, 23169, 23452, 23731, 24006, 24278, 24546, 24811, 25072, 25329, 25582, 25831, 26077, 26318, 26556, 26789, 
27019, 27244, 27466, 27683, 27896, 28105, 28309, 28510, 28706, 28897, 29085, 29268, 29446, 29621, 29790, 29955, 
30116, 30272, 30424, 30571, 30713, 30851, 30984, 31113, 31236, 31356, 31470, 31580, 31684, 31785, 31880, 31970, 
32056, 32137, 32213, 32284, 32350, 32412, 32468, 32520, 32567, 32609, 32646, 32678, 32705, 32727, 32744, 32757, 
32764, 32767, 32764};




int16_t SAMAPP_qsin(uint16_t a)
{
  uint8_t f;
  int16_t s0,s1;

  if (a & 32768)
    return -SAMAPP_qsin(a & 32767);
  if (a & 16384)
      a = 32768 - a;
  f = a & 127;
  s0 = pgm_read_word(sintab + (a >> 7));
  s1 = pgm_read_word(sintab + (a >> 7) + 1);
  return (s0 + ((int32_t)f * (s1 - s0) >> 7));
}

/* cos funtion */
int16_t SAMAPP_qcos(uint16_t a)
{
  return (SAMAPP_qsin(a + 16384));
}

#endif


/* Global object for transport */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);


/* api to test the init of transport layer */
int16_t TestApp_Bootup()
{
  uint32_t chipid = 0;
   FTImpl.Init(FT_DISPLAY_WQVGA_480x272);
   
   delay(20);//safer side
   chipid = FTImpl.Read32(FT_ROM_CHIPID);
   /* identify the chip */
   if(FT800_CHIPID != chipid)
   {
     Serial.print("Error in chip id read ");
     Serial.println(chipid,HEX);
   }
   return 0;
}

int32_t FTUtility_Dec2Ascii(char *pSrc,int32_t value)
{
	int16_t Length;
	char *pdst,charval;
	int32_t CurrVal = value,tmpval,i;
	char tmparray[16],idx = 0;

	Length = strlen(pSrc);
	pdst = pSrc + Length;

	if(0 == value)
	{
		*pdst++ = '0';
		*pdst++ = '\0';
		return 0;
	}

	if(CurrVal < 0)
	{
		*pdst++ = '-';
		CurrVal = - CurrVal;
	}
	/* insert the value */
	while(CurrVal > 0){
		tmpval = CurrVal;
		CurrVal /= 10;
		tmpval = tmpval - CurrVal*10;
		charval = '0' + tmpval;
		tmparray[idx++] = charval;
	}

	for(i=0;i<idx;i++)
	{
		*pdst++ = tmparray[idx - i - 1];
	}
	*pdst++ = '\0';

	return 0;
}



int16_t SAMAPP_API_Screen(char *str)
{
	FTImpl.StartTransferCmd();
    FTImpl.WriteCmd(CMD_DLSTART);
    FTImpl.WriteCmd(CLEAR(1,1,1));	
    FTImpl.WriteCmd(COLOR_RGB(0x80,0x80,0x00));
    FTImpl.Cmd_Text((DisplayWidth/2), DisplayHeight/2, 31, FT_OPT_CENTER, str);
    FTImpl.WriteCmd(DISPLAY());
	FTImpl.WriteCmd(CMD_SWAP);	
    coprostatus = FTImpl.Finish();	
	FTImpl.EndTransferCmd();
    delay(3000);
    return coprostatus;
}

#ifdef SAMAPP_ENABLE_APIS_SET0

/*****************************************************************************/
/* Example code to display few points at various offsets with various colors */
/*****************************************************************************/
void SAMAPP_GPU_Points()
{
	/* Construct DL of points */
	FTImpl.DLStart();
	FTImpl.Clear(1,1,1);
	FTImpl.ColorRGB(128, 0, 0);
	FTImpl.PointSize(5 * 16);
	FTImpl.Begin(FT_POINTS);
	FTImpl.Vertex2f((DisplayWidth/5) * 16, (DisplayHeight/2) * 16);
	FTImpl.ColorRGB(0, 128, 0);
	FTImpl.PointSize(15 * 16);
	FTImpl.Vertex2f((DisplayWidth*2/5) * 16, (DisplayHeight/2) * 16);
	FTImpl.ColorRGB(0, 0, 128);
	FTImpl.PointSize(25 * 16);
	FTImpl.Vertex2f((DisplayWidth*3/5) * 16, (DisplayHeight/2) * 16);
	FTImpl.ColorRGB(128, 0, 128);
	FTImpl.PointSize(35 * 16);
	FTImpl.Vertex2f((DisplayWidth*4/5) * 16, (DisplayHeight/2) * 16);
	FTImpl.DLEnd();
	FTImpl.Finish();
    delay(3000); 
}


void SAMAPP_GPU_Lines()
{
	int16_t LineHeight = 25;
	FTImpl.DLStart();
	FTImpl.Clear(1,1,1);
	FTImpl.ColorRGB(128, 0, 0);	
	FTImpl.LineWidth(5 * 16);
	FTImpl.Begin(FT_LINES);
	FTImpl.Vertex2f((DisplayWidth/4) * 16, ((DisplayHeight-LineHeight)/2) * 16);	
	FTImpl.Vertex2f((DisplayWidth/4) * 16, ((DisplayHeight+LineHeight)/2) * 16);
	FTImpl.ColorRGB(0, 128, 0);
	FTImpl.LineWidth(10 * 16);
	LineHeight = 40;
	FTImpl.Vertex2f((DisplayWidth*2/4) * 16, ((DisplayHeight-LineHeight)/2) * 16);	
	FTImpl.Vertex2f((DisplayWidth*2/4) * 16, ((DisplayHeight+LineHeight)/2) * 16);
	FTImpl.ColorRGB(128, 128, 0);
	FTImpl.LineWidth(20 * 16);
	LineHeight = 55;
	FTImpl.Vertex2f((DisplayWidth*3/4) * 16, ((DisplayHeight-LineHeight)/2) * 16);	
	FTImpl.Vertex2f((DisplayWidth*3/4) * 16, ((DisplayHeight+LineHeight)/2) * 16);
	FTImpl.DLEnd();
	FTImpl.Finish();
    delay(3000); 
}


void SAMAPP_GPU_Rectangles()
{
	int16_t RectWidth,RectHeight;
	FTImpl.DLStart();
	FTImpl.Clear(1,1,1);
	FTImpl.ColorRGB(0, 0, 128);	
	FTImpl.LineWidth(1 * 16);	
	FTImpl.Begin(FT_RECTS);
	RectWidth = 5;RectHeight = 25;
	FTImpl.Vertex2f(((DisplayWidth/4)-(RectWidth/2)) * 16, ((DisplayHeight-RectHeight)/2) * 16);	
	FTImpl.Vertex2f(((DisplayWidth/4)+(RectWidth/2)) * 16, ((DisplayHeight+RectHeight)/2) * 16);
	FTImpl.ColorRGB(0, 0, 128);	
	FTImpl.LineWidth(5* 16);	
	RectWidth = 10;RectHeight = 40;
	FTImpl.Vertex2f(((DisplayWidth*2/4)-(RectWidth/2)) * 16, ((DisplayHeight-RectHeight)/2) * 16);	
	FTImpl.Vertex2f(((DisplayWidth*2/4)+(RectWidth/2)) * 16, ((DisplayHeight+RectHeight)/2) * 16);
	FTImpl.ColorRGB(128, 128, 0);	
	FTImpl.LineWidth(10* 16);	
	RectWidth = 20;RectHeight = 55;
	FTImpl.Vertex2f(((DisplayWidth*3/4)-(RectWidth/2)) * 16, ((DisplayHeight-RectHeight)/2) * 16);	
	FTImpl.Vertex2f(((DisplayWidth*3/4)+(RectWidth/2)) * 16, ((DisplayHeight+RectHeight)/2) * 16);
	FTImpl.DLEnd();
	FTImpl.Finish();
	delay(3000); 
}


/* Display lena face at different locations with various colors, -ve offsets, alpha blend etc */
void SAMAPP_GPU_Bitmap()
{
	SAMAPP_Bitmap_header_t *p_bmhdr;
	int16_t BMoffsetx,BMoffsety;

	p_bmhdr = (SAMAPP_Bitmap_header_t *)&SAMAPP_Bitmap_RawData_Header[0];
	/* Copy raw data into address 0 followed by generation of bitmap */
	FTImpl.Writefromflash(FT_RAM_G,&SAMAPP_Bitmap_RawData[p_bmhdr->Arrayoffset], p_bmhdr->Stride*p_bmhdr->Height);

	
	FTImpl.DLStart();
	FTImpl.Clear(1,1,1);	
	FTImpl.ColorRGB(255,255,255);
	FTImpl.BitmapSource(FT_RAM_G);
	FTImpl.BitmapLayout(p_bmhdr->Format, p_bmhdr->Stride, p_bmhdr->Height);
	FTImpl.BitmapSize(FT_NEAREST,FT_BORDER,FT_BORDER, p_bmhdr->Width, p_bmhdr->Height);
	FTImpl.Begin(FT_BITMAPS); // start drawing bitmaps
	BMoffsetx = ((FT_DispWidth/4) - (p_bmhdr->Width/2));
	BMoffsety = ((FT_DispHeight/2) - (p_bmhdr->Height/2));
	FTImpl.Vertex2ii(BMoffsetx, BMoffsety, 0, 0);
	FTImpl.ColorRGB(255, 64, 64); // red at (200, 120)
	BMoffsetx = ((FT_DispWidth*2/4) - (p_bmhdr->Width/2));
	BMoffsety = ((FT_DispHeight/2) - (p_bmhdr->Height/2));
	FTImpl.Vertex2ii(BMoffsetx, BMoffsety, 0, 0);
	FTImpl.ColorRGB(64, 180, 64); // green at (216, 136)
	BMoffsetx += (p_bmhdr->Width/2);
	BMoffsety += (p_bmhdr->Height/2);
	FTImpl.Vertex2ii(BMoffsetx, BMoffsety, 0, 0);
	FTImpl.ColorRGB(255, 255, 64); // transparent yellow at (232, 152)
	FTImpl.ColorA(150);
	BMoffsetx += (p_bmhdr->Width/2);
	BMoffsety += (p_bmhdr->Height/2);
	FTImpl.Vertex2ii(BMoffsetx, BMoffsety, 0, 0);
	FTImpl.ColorA(255);
	FTImpl.ColorRGB(255,255,255);
	FTImpl.Vertex2f(-10*16, -10*16);//for -ve coordinates use vertex2f instruction
	FTImpl.End();
	FTImpl.DLEnd();
	FTImpl.Finish();
	delay(3000); 
}


#define FT_GPU_NUMCHAR_PERFONT (128)
#define FT_GPU_FONT_TABLE_SIZE (148)

typedef struct FT_Gpu_Fonts
{
	/* All the values are in bytes */
	/* Width of each character font from 0 to 127 */
	uint8_t	FontWidth[FT_GPU_NUMCHAR_PERFONT];
	/* Bitmap format of font wrt bitmap formats supported by FT801 - L1, FT_L4, L8 */
	uint32_t	FontBitmapFormat;
	/* Font line stride in FT801 ROM */
	uint32_t	FontLineStride;
	/* Font width in pixels */
	uint32_t	FontWidthInPixels;
	/* Font height in pixels */
	uint32_t	FontHeightInPixels;
	/* Pointer to font graphics raw data */
	uint32_t	PointerToFontGraphicsData;
}FT_Gpu_Fonts_t;






void SAMAPP_GPU_Fonts()
{
  int32_t i,j,hoffset,voffset,stringlen;
  uint32_t FontTableAddress;
  const char Display_string[] = "Hello World";
  FT_Fonts_t Display_fontstruct;

	hoffset = ((FT_DispWidth - 100)/2);voffset = FT_DispHeight/2;

	/* Read the font address from 0xFFFFC location */
	FontTableAddress = FTImpl.Read32(0xFFFFC);
    stringlen = sizeof(Display_string) - 1;
	for(i=0;i<16;i++)
	{
		/* Read the font table from hardware */
		FTImpl.Read((FontTableAddress + i*FT_GPU_FONT_TABLE_SIZE),(uint8_t *)&Display_fontstruct,FT_GPU_FONT_TABLE_SIZE);
                
		
		FTImpl.DLStart();
		FTImpl.Clear(1,1,1);

		/* Display string at the center of display */
		FTImpl.Begin(FT_BITMAPS);
		hoffset = ((FT_DispWidth - 120)/2);
		voffset = ((FT_DispHeight - Display_fontstruct.FontHeightInPixels)/2);

		/* Display hello world by offsetting wrt char size */
		for(j=0;j<stringlen;j++)                
		{
			FTImpl.Vertex2ii(hoffset,voffset,(i+16),Display_string[j]);
			hoffset += Display_fontstruct.FontWidth[Display_string[j]];
		}
		FTImpl.DLEnd();
		FTImpl.Finish();
		delay(1000); 
	}
}


void SAMAPP_GPU_Text8x8()
{
	/* Write the data into FT_RAM_G */
    const char Text_Array[] = "abcdefgh";
    int32_t String_size,hoffset = 16,voffset = 16;

	
    String_size = sizeof(Text_Array) - 1;
    FTImpl.Write(FT_RAM_G,(uint8_t *)Text_Array, String_size);

    FTImpl.DLStart();
    FTImpl.Clear(1,1,1);
    FTImpl.BitmapSource(FT_RAM_G);
    FTImpl.BitmapLayout(FT_TEXT8X8, 1*8,1);//L1 format, each input data element is in 1 byte size
    FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_REPEAT, 8*8, 8*2);//output is 8x8 format - draw 8 characters in horizontal repeated in 2 line
    FTImpl.Begin(FT_BITMAPS);
	/* Display text 8x8 at hoffset, voffset location */
    FTImpl.Vertex2f(hoffset*16,voffset*16);

	/*     
           abcdabcdabcdabcd
           efghefghefghefgh
	*/
    FTImpl.BitmapLayout(FT_TEXT8X8, 1*4,2);//L1 format and each datatype is 1 byte size
    FTImpl.BitmapSize(FT_NEAREST, FT_REPEAT, FT_BORDER, 8*16, 8*2);//each character is 8x8 in size -  so draw 32 characters in horizontal and 32 characters in vertical
    hoffset = FT_DispWidth/2;
    voffset = FT_DispHeight/2;
    FTImpl.Vertex2f(hoffset*16,voffset*16);
    FTImpl.DLEnd();
    FTImpl.Finish();
    delay(3000);
}


void SAMAPP_GPU_TextVGA()
{
	/* Write the data into FT_RAM_G */
	uint16_t Text_Array[160];
	int32_t String_size,hoffset = 32,voffset = 32,i;
	for(i=0;i<160;i++)
	{
	  Text_Array[i] = random(65536);//within range
	}
	
	String_size = 160*2;
	FTImpl.Write(FT_RAM_G,(uint8_t *)Text_Array, String_size);

	
	FTImpl.DLStart();
        FTImpl.Clear(1,1,1);
        FTImpl.BitmapSource(FT_RAM_G);
  
	/* mandatory for textvga as background color is also one of the parameter in textvga format */
	FTImpl.BlendFunc(FT_ONE,FT_ZERO);

	//draw 8x8
	FTImpl.BitmapLayout(FT_TEXTVGA, 2*4,8);//L1 format, but each input data element is of 2 bytes in size
	FTImpl.BitmapSize(FT_NEAREST,FT_BORDER,FT_BORDER, 8*8, 8*8);//output is 8x8 format - draw 8 characters in horizontal and 8 vertical
	FTImpl.Begin(FT_BITMAPS);
	FTImpl.Vertex2f(hoffset*16,voffset*16);
	FTImpl.End();
	//draw textvga
	
	FTImpl.BitmapLayout(FT_TEXTVGA, 2*16,8);//L1 format but each datatype is 16bit size
	FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_REPEAT, 8*32, 8*32);//8 pixels per character and 32 rows/colomns
	FTImpl.Begin(FT_BITMAPS);
	hoffset = FT_DispWidth/2;
	voffset = FT_DispHeight/2;
	/* Display textvga at hoffset, voffset location */
	FTImpl.Vertex2f(hoffset*16,voffset*16);
	FTImpl.DLEnd();
    FTImpl.Finish();
    delay(3000);
}


void SAMAPP_GPU_Bargraph()
{
	/* Write the data into FT_RAM_G */
	uint8_t Y_Array[512],NumSineWaves = 4;
	int32_t String_size,hoffset = 0,voffset = 0,i;

	hoffset = 0;
	voffset = 0;

	for(i=0;i<512;i++)
	{
		Y_Array[i] = random(128) + 64;//within range
	}
	
	String_size = 512;
	FTImpl.Write(FT_RAM_G,(uint8_t *)&Y_Array[0], String_size);

	
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(0xff,0xff,0xff);
    FTImpl.Clear(1,1,1);
    FTImpl.BitmapSource(FT_RAM_G);
	FTImpl.BitmapLayout(FT_BARGRAPH, 256,1);
	FTImpl.ColorRGB(128, 0,0);
	FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 256, 256);
	FTImpl.Begin(FT_BITMAPS);
	/* Display text 8x8 at hoffset, voffset location */
	FTImpl.Vertex2ii(hoffset,voffset,0,0);
	hoffset = 256;
	FTImpl.Vertex2ii(hoffset,voffset,0,1);
	FTImpl.DLEnd();
    FTImpl.Finish();
    delay(1000);

	for(i=0;i<512;i++)
	{
        int32_t tmpval,tmpidx;
       //tmpidx = (i + 256)&(512 - 1);
        tmpidx = i;
		tmpval = 128 + ((int32_t)(i/3)*SAMAPP_qsin(-65536*i/48)/65536);//within range   
        Y_Array[i] = tmpval&0xff;
	}
	String_size = 512;
	FTImpl.Write(FT_RAM_G,(uint8_t *)Y_Array, String_size);
    delay(1000);

	
	FTImpl.DLStart();	
      FTImpl.Clear(1,1,1);
      FTImpl.BitmapSource(FT_RAM_G);
	FTImpl.BitmapLayout(FT_BARGRAPH, 256,1);
	FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 256, 256);
	FTImpl.Begin(FT_BITMAPS);
	FTImpl.ColorRGB(255, 0,0);
	/* Display bargraph at hoffset, voffset location */
	hoffset = 0;
	voffset = 0;
	FTImpl.Vertex2ii(hoffset,voffset,0,0);
	hoffset = 256;
	voffset = 0;
	FTImpl.Vertex2ii(hoffset,voffset,0,1);
	hoffset = 0;
	voffset += 4;
	FTImpl.ColorRGB(0, 0,0);
	FTImpl.Vertex2ii(hoffset,voffset,0,0);
	hoffset = 256;
	FTImpl.Vertex2ii(hoffset,voffset,0,1);
	FTImpl.DLEnd();
    FTImpl.Finish();
	delay(1000);
}



void SAMAPP_GPU_LineStrips()
{
	FTImpl.DLStart();	
	FTImpl.ClearColorRGB(5, 45, 10);
	FTImpl.Clear(1,1,1);
	FTImpl.ColorRGB(255, 168, 64);	
	FTImpl.Begin(FT_LINE_STRIP);
	FTImpl.Vertex2f(16 * 16,16 * 16);
	FTImpl.Vertex2f((FT_DispWidth*2/3) * 16,(FT_DispHeight*2/3) * 16);
	FTImpl.Vertex2f((FT_DispWidth - 80) * 16,(FT_DispHeight - 20) * 16);
	FTImpl.DLEnd();
    FTImpl.Finish();
	delay(1000);

}

void SAMAPP_GPU_EdgeStrips()
{
	
	FTImpl.DLStart();	
	FTImpl.ClearColorRGB(5, 45, 10);
	FTImpl.Clear(1,1,1);
	FTImpl.ColorRGB(255, 168, 64);
	FTImpl.Begin(FT_EDGE_STRIP_R);
	FTImpl.Vertex2f(16 * 16,16 * 16);
	FTImpl.Vertex2f((FT_DispWidth*2/3) * 16,(FT_DispHeight*2/3) * 16);
	FTImpl.Vertex2f((FT_DispWidth - 80) * 16,(FT_DispHeight - 20) * 16);
	FTImpl.DLEnd();
    FTImpl.Finish();
	delay(1000);
}
void SAMAPP_GPU_Scissor()
{
	FTImpl.DLStart();	
	FTImpl.Clear(1,1,1);
	FTImpl.ScissorXY(40, 20); // Scissor rectangle top left at (40, 20)
	FTImpl.ScissorSize(40, 40); // Scissor rectangle is 40 x 40 pixels
	FTImpl.ClearColorRGB(255, 255, 0);// Clear to yellow	
	FTImpl.Clear(1,1,1);
	/* Download the DL into DL RAM */
	FTImpl.DLEnd();
        FTImpl.Finish();
	delay(1000);
}

void SAMAPP_GPU_Stencil()
{
	int16_t PointSize = 50,DispBtwPoints = 60;
	
	FTImpl.DLStart();	
	FTImpl.Clear(1,1,1);
	
	FTImpl.ScissorXY(40, 20); // Scissor rectangle top left at (40, 20)
	FTImpl.StencilOp(FT_INCR,FT_INCR);
	FTImpl.PointSize(PointSize * 16);
	FTImpl.Begin(FT_POINTS);
	FTImpl.Vertex2ii(((FT_DispWidth - DispBtwPoints)/2), (FT_DispHeight/2), 0, 0);
	FTImpl.Vertex2ii(((FT_DispWidth + DispBtwPoints)/2), (FT_DispHeight/2), 0, 0);
	FTImpl.StencilFunc(FT_EQUAL, 2, 255);
	FTImpl.ColorRGB(128, 0, 0);
	FTImpl.Vertex2ii((FT_DispWidth/2), (FT_DispHeight/2), 0, 0);
	

	/* Download the DL into DL RAM */
	FTImpl.DLEnd();
    FTImpl.Finish();
	delay(1000);
}


void SAMAPP_Gpu_Polygon()
{
	
	FTImpl.DLStart();	
	FTImpl.Clear(1,1,1);
	FTImpl.ColorRGB(255, 0, 0);
	FTImpl.StencilOp(FT_INCR,FT_INCR);
	FTImpl.ColorMask(0,0,0,0);//mask all the colors
	FTImpl.Begin(FT_EDGE_STRIP_L);
	FTImpl.Vertex2ii((FT_DispWidth/2),(FT_DispHeight/4),0,0);
	FTImpl.Vertex2ii((FT_DispWidth*4/5),(FT_DispHeight*4/5),0,0);
	FTImpl.Vertex2ii((FT_DispWidth/4),(FT_DispHeight/2),0,0);
	FTImpl.Vertex2ii((FT_DispWidth/2),(FT_DispHeight/4),0,0);
	FTImpl.End();
	FTImpl.ColorMask(1,1,1,1);//enable all the colors
	FTImpl.StencilFunc(FT_EQUAL,1,255);
	FTImpl.Begin(FT_EDGE_STRIP_L);
	FTImpl.Vertex2ii(FT_DispWidth,0,0,0);
	FTImpl.Vertex2ii(FT_DispWidth,FT_DispHeight,0,0);
	FTImpl.End();
	
	/* Draw lines at the borders to make sure anti aliazing is also done */
	FTImpl.StencilFunc(FT_ALWAYS,0,255);
	FTImpl.LineWidth(1*16);
	FTImpl.ColorRGB(0, 0, 0);
	FTImpl.Begin(FT_LINES);
	FTImpl.Vertex2ii((FT_DispWidth/2),(FT_DispHeight/4),0,0);
	FTImpl.Vertex2ii((FT_DispWidth*4/5),(FT_DispHeight*4/5),0,0);
	FTImpl.Vertex2ii((FT_DispWidth*4/5),(FT_DispHeight*4/5),0,0);
	FTImpl.Vertex2ii((FT_DispWidth/4),(FT_DispHeight/2),0,0);
	FTImpl.Vertex2ii((FT_DispWidth/4),(FT_DispHeight/2),0,0);
	FTImpl.Vertex2ii((FT_DispWidth/2),(FT_DispHeight/4),0,0);
	FTImpl.End();
	/* Download the DL into DL RAM */
	FTImpl.DLEnd();
    FTImpl.Finish();
	delay(1000);
}



void SAMAPP_Gpu_Cube()
{
    uint32_t points[6*5],x,y,i,z;
    int16_t xoffset,yoffset,CubeEdgeSz;

// color vertices
    uint16_t colors[6][3] = { 255,0,0,
                                255,0,150,
                                0,255,0,
                                110,120,110,
                                0,0,255,
                                128,128,0
	                            };

// Cube dimention is of 100*100*100
	CubeEdgeSz = 100;
	xoffset = (FT_DispWidth/2 - CubeEdgeSz);
	yoffset=((FT_DispHeight - CubeEdgeSz)/2);	


// xy plane(front)
       points[0] = VERTEX2F(xoffset*16,yoffset*16);
       points[1] = VERTEX2F((xoffset+CubeEdgeSz)*16,yoffset*16);
       points[2] = VERTEX2F((xoffset+CubeEdgeSz)*16,(yoffset+CubeEdgeSz)*16);
       points[3] = VERTEX2F(xoffset*16,(yoffset+CubeEdgeSz)*16);
       points[4] = points[0];

//yz plane (left)
       x = (xoffset+(CubeEdgeSz/2));            //     xoff+w/2
       y = (yoffset-(CubeEdgeSz/2));            //     yoff-h/2

       points[5] = points[0];     
       points[6] = VERTEX2F(x*16,y*16);
       points[7] = VERTEX2F(x*16,(y+CubeEdgeSz)*16);
       points[8] = points[3];
       points[9] = points[5];

//xz plane(top)
       points[10] = points[0];
       points[11] = points[1];  
       points[12] = VERTEX2F((x+CubeEdgeSz)*16,(y)*16);
       points[13] = points[6];
       points[14] = points[10];

//xz plane(bottom)
       points[15] = points[3];
       points[16] = points[2];
       points[17] = VERTEX2F((x+CubeEdgeSz)*16,(y+CubeEdgeSz)*16);
       points[18] = points[7];
       points[19] = points[15];

//yz plane (right)
       points[20] = points[2];
       points[21] = points[17];
       points[22] = points[12];
       points[23] = points[1];
       points[24] = points[20];

//yz plane (back)
       points[25] = points[6];
       points[26] = points[7];
       points[27] = points[17];
       points[28] = points[12];
       points[29] = points[25];

       
    FTImpl.DLStart();		
    FTImpl.ClearColorRGB(255,255,255);
    FTImpl.Clear(1,1,1);
    FTImpl.LineWidth(16);
    FTImpl.ColorRGB(255,255,255);   

// Draw a cube 
    FTImpl.StencilOp(FT_INCR,FT_INCR);
    FTImpl.ColorA(192);
    for(z=0;z<6;z++)
    {      
		FTImpl.Clear(0,1,1);
		FTImpl.ColorMask(0,0,0,0);//mask all the colors and draw one surface
		FTImpl.StencilFunc(FT_ALWAYS,0,255);//stencil function to increment all the values
        FTImpl.Begin(FT_EDGE_STRIP_L);
        for(i = 0;i<5;i++)
        {
			FTImpl.WriteCmd(points[z*5 + i]);
        }
        FTImpl.End();
		/* set the color and draw a strip */
		FTImpl.ColorMask(1,1,1,1);
		FTImpl.StencilFunc(FT_EQUAL,1,255);		
        FTImpl.ColorRGB(colors[z][0],colors[z][1],colors[z][2]);
		FTImpl.Begin(FT_RECTS);
		FTImpl.Vertex2ii(xoffset,0,0,0);
		FTImpl.Vertex2ii(xoffset + CubeEdgeSz*2,yoffset + CubeEdgeSz*2,0,0);
		FTImpl.End();
    }
	/* Download the DL into DL RAM */
	FTImpl.DLEnd();
    FTImpl.Finish();
	delay(1000);
}


float lerp(float t, float a, float b)
{
	return (float)((1 - t) * a + t * b);
}
float smoothlerp(float t, float a, float b)
{
	
    float lt = 3 * t * t - 2 * t * t * t;

    return lerp(lt, a, b);
}

/* First draw points followed by lines to create 3d ball kind of effect */
void SAMAPP_GPU_Ball_Stencil()
{
	int16_t xball = (FT_DispWidth/2),yball = 120,rball = (FT_DispWidth/8);
	int16_t numpoints = 6,numlines = 8,i,asize,aradius,gridsize = 20;
	int32_t asmooth,loopflag = 1,dispr = (FT_DispWidth - 10),displ = 10,dispa = 10,dispb = (FT_DispHeight - 10),xflag = 1,yflag = 1;

	dispr -= ((dispr - displ)%gridsize);
	dispb -= ((dispb - dispa)%gridsize);
	/* write the play sound */
	FTImpl.Write16(REG_SOUND,0x50);
    loopflag = 100;
	while(loopflag-- >0)
	{
		if(((xball + rball + 2) >= dispr) || ((xball - rball - 2) <= displ))
		{
			xflag ^= 1;			
			FTImpl.Write(REG_PLAY,1);
		}
		if(((yball + rball + 8) >= dispb) || ((yball - rball - 8) <= dispa))
		{
			yflag ^= 1;
			FTImpl.Write(REG_PLAY,1);
		}
		if(xflag)
		{
			xball += 2;
		}
		else
		{
			xball -= 2;
		}
		if(yflag)
		{
			yball += 8 ;
		}
		else
		{
			yball -= 8;
		}

		FTImpl.DLStart();	
		FTImpl.ClearColorRGB(128, 128, 0);
		FTImpl.Clear(1, 1, 1); // clear screen
		FTImpl.StencilOp(FT_INCR,FT_INCR);
		FTImpl.ColorRGB(0, 0, 0);
		/* draw grid */
		FTImpl.LineWidth(16);
		FTImpl.Begin(FT_LINES);
		for(i=0;i<=((dispr - displ)/gridsize);i++)
		{
			FTImpl.Vertex2f((displ + i*gridsize)*16,dispa*16);
			FTImpl.Vertex2f((displ + i*gridsize)*16,dispb*16);
		}
		for(i=0;i<=((dispb - dispa)/gridsize);i++)
		{
			FTImpl.Vertex2f(displ*16,(dispa + i*gridsize)*16);
			FTImpl.Vertex2f(dispr*16,(dispa + i*gridsize)*16);
		}
		FTImpl.End();
		FTImpl.ColorMask(0,0,0,0);//mask all the colors
		FTImpl.PointSize(rball*16);
		FTImpl.Begin(FT_POINTS);
		FTImpl.Vertex2f(xball*16,yball*16);
		FTImpl.StencilOp(FT_INCR,FT_ZERO);
		FTImpl.StencilFunc(FT_GEQUAL,1,255);
		/* one side points */
		for(i=1;i<=numpoints;i++)
		{
			asize = i*rball*2/(numpoints + 1);
			asmooth = (int16_t)smoothlerp((float)((float)(asize)/(2*(float)rball)),0,2*(float)rball);		
			if(asmooth > rball)
			{
				//change the offset to -ve
				int32_t tempsmooth;
				tempsmooth = asmooth - rball;
				aradius = (rball*rball + tempsmooth*tempsmooth)/(2*tempsmooth);
				FTImpl.PointSize(aradius*16);
				FTImpl.Vertex2f((xball - aradius + tempsmooth)*16,yball*16);
			}
			else
			{
				int32_t tempsmooth;
				tempsmooth = rball - asmooth;
				aradius = (rball*rball + tempsmooth*tempsmooth)/(2*tempsmooth);
				FTImpl.PointSize(aradius*16);
				FTImpl.Vertex2f((xball+ aradius - tempsmooth)*16,yball*16);
			}
		}



		FTImpl.End();
		FTImpl.Begin(FT_LINES);
		/* draw lines - line should be at least radius diameter */
		for(i=1;i<=numlines;i++)
		{
			asize = (i*rball*2/numlines);
			asmooth = (int16_t)smoothlerp((float)((float)(asize)/(2*(float)rball)),0,2*(float)rball);
			FTImpl.LineWidth(asmooth * 16);
			FTImpl.Vertex2f((xball - rball)*16,(yball - rball )*16);
			FTImpl.Vertex2f((xball + rball)*16,(yball - rball )*16);
		}
		FTImpl.End();
	
		FTImpl.ColorMask(1,1,1,1);//enable all the colors
		FTImpl.StencilFunc(FT_ALWAYS,1,255);
		FTImpl.StencilOp(FT_KEEP,FT_KEEP);
		FTImpl.ColorRGB(255, 255, 255);
		FTImpl.PointSize(rball*16);
		FTImpl.Begin(FT_POINTS);
		FTImpl.Vertex2f((xball - 1)*16,(yball - 1)*16);
		FTImpl.ColorRGB(0, 0, 0);//shadow
		FTImpl.ColorA(160);
		FTImpl.Vertex2f((xball+16)*16,(yball+8)*16);
		FTImpl.ColorA(255);
		FTImpl.ColorRGB(255, 255, 255);
		FTImpl.Vertex2f(xball*16,yball*16);
		FTImpl.ColorRGB(255, 0, 0);
		FTImpl.StencilFunc(FT_GEQUAL,1,1);
		FTImpl.StencilOp(FT_KEEP,FT_KEEP);
		FTImpl.Vertex2f(xball*16,yball*16);
		FTImpl.End();

		

		/* Download the DL into DL RAM */
		FTImpl.DLEnd();
		FTImpl.Finish();
	//	delay(1000);

	}

}



/* Call and return example - simple graph */
void SAMAPP_GPU_StreetMap()
{
	FTImpl.DLStart();	
	FTImpl.ClearColorRGB(236,232,224); //light gray
	FTImpl.Clear(1, 1, 1); // clear screen
	FTImpl.ColorRGB(170,157,136);//medium gray
	FTImpl.LineWidth(63);
	FTImpl.Call(19);//draw the streets
	FTImpl.ColorRGB(250,250,250);//white
	FTImpl.LineWidth(48);
	FTImpl.Call(19);//draw the streets
	FTImpl.ColorRGB(0,0,0);
	FTImpl.Begin(FT_BITMAPS);
	FTImpl.Vertex2ii(240,91,27,77);
	FTImpl.Vertex2ii(252,91,27,97);
	FTImpl.Vertex2ii(260,91,27,105);
	FTImpl.Vertex2ii(263,91,27,110);
	FTImpl.Vertex2ii(275,91,27,115);
	FTImpl.Vertex2ii(282,91,27,116);
	FTImpl.Vertex2ii(286,91,27,46);
	FTImpl.End();
	//FTImpl.Display();
	FTImpl.Begin(FT_LINES);
	FTImpl.Vertex2f(-160,-20  );
	FTImpl.Vertex2f(320,4160  );
	FTImpl.Vertex2f(800,-20   );
	FTImpl.Vertex2f(1280,4160 );
	FTImpl.Vertex2f(1920,-20  );
	FTImpl.Vertex2f(2400,4160 );
	FTImpl.Vertex2f(2560,-20  );
	FTImpl.Vertex2f(3040,4160 );
	FTImpl.Vertex2f(3200,-20  );
	FTImpl.Vertex2f(3680,4160 );
	FTImpl.Vertex2f(2880,-20  );
	FTImpl.Vertex2f(3360,4160 );
	FTImpl.Vertex2f(-20,0	  );
	FTImpl.Vertex2f(5440,-480 );
	FTImpl.Vertex2f(-20,960   );
	FTImpl.Vertex2f(5440,480  );
	FTImpl.Vertex2f(-20,1920  );
	FTImpl.Vertex2f(5440,1440 );
	FTImpl.Vertex2f(-20,2880  );
	FTImpl.Vertex2f(5440,2400 );
	FTImpl.End();
	FTImpl.Return();

	/* Download the DL into DL RAM */
	FTImpl.DLEnd();
	FTImpl.Finish();
	delay(1000);
}

/* usage of additive blending - draw 3 Gs*/
void SAMAPP_GPU_AdditiveBlendText()
{
	FTImpl.DLStart();		
	FTImpl.Clear(1, 1, 1); // clear screen	
	FTImpl.Begin(FT_BITMAPS);
	FTImpl.Vertex2ii(50, 30, 31, 0x47);
	FTImpl.ColorA(128);
	FTImpl.Vertex2ii(58, 38, 31, 0x47);
	FTImpl.ColorA(64);
	FTImpl.Vertex2ii(66, 46, 31, 0x47);
	FTImpl.End();

	/* Download the DL into DL RAM */
	FTImpl.DLEnd();
	FTImpl.Finish();
	delay(1000);
}


void SAMAPP_GPU_MacroUsage()
{
	int32_t xoffset,yoffset,xflag = 1,yflag = 1,flagloop = 1;
	SAMAPP_Bitmap_header_t *p_bmhdr;
	xoffset = FT_DispWidth/3;
	yoffset = FT_DispHeight/2;

	/* First write a valid macro instruction into macro0 */
	FTImpl.Write32(REG_MACRO_0,VERTEX2F(xoffset*16,yoffset*16));
	/* update lena face as bitmap 0 */

	p_bmhdr = &SAMAPP_Bitmap_RawData_Header[0];
	/* Copy raw data into address 0 followed by generation of bitmap */
	FTImpl.Writefromflash(FT_RAM_G,&SAMAPP_Bitmap_RawData[p_bmhdr->Arrayoffset], p_bmhdr->Stride*p_bmhdr->Height);

	
	FTImpl.DLStart();		
	FTImpl.Clear(1, 1, 1); // clear screen	
	
	FTImpl.BitmapSource(FT_RAM_G);
	FTImpl.BitmapLayout(p_bmhdr->Format, p_bmhdr->Stride, p_bmhdr->Height);
	FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, p_bmhdr->Width, p_bmhdr->Height);
	FTImpl.Begin(FT_BITMAPS); // start drawing bitmaps
	FTImpl.Macro(0); // draw the image at (100,120)
	FTImpl.End();
	FTImpl.DLEnd();
	FTImpl.Finish();
	
    flagloop = 300;
	while(flagloop-- > 0)
	{
		if(((xoffset + p_bmhdr->Width) >= FT_DispWidth) || (xoffset <= 0))
		{
			xflag ^= 1;
		}
		if(((yoffset + p_bmhdr->Height) >= FT_DispHeight) || (yoffset <= 0))
		{
			yflag ^= 1;
		}
		if(xflag)
		{
			xoffset++;
		}
		else
		{
			xoffset--;
		}
		if(yflag)
		{
			yoffset++;
		}
		else
		{
			yoffset--;
		}
		/*  update just the macro */
		FTImpl.Write32(REG_MACRO_0,VERTEX2F(xoffset*16,yoffset*16));
        delay(10);              
	}
}


/* Additive blending of points - 1000 points */

void  SAMAPP_GPU_AdditiveBlendPoints()
{
	int32_t i,hoffset,voffset,flagloop=1,j,hdiff,vdiff,PointSz;

#define MSVC_PI (3.141592653)

	PointSz = 4;
    flagloop = 10;
	while(flagloop-- > 0)
	{
  		FTImpl.DLStart();		
		FTImpl.Clear(1, 1, 1); // clear screen				
		FTImpl.ColorRGB(20, 91,20); // green color for additive blending
		FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE);//input is source alpha and destination is whole color
		FTImpl.PointSize(PointSz*16);	
		FTImpl.Begin(FT_POINTS);
		/* First 100 random values */
		for(i=0;i<100;i++)
		{
			hoffset = random(FT_DispWidth);
			voffset = random(FT_DispHeight);
			FTImpl.Vertex2f(hoffset*16,voffset*16);
		}

		/* next 480 are sine values of two cycles */
		for(i=0;i<160;i++)
		{
			/* i is x offset, y is sinwave */
			hoffset = i*3;
			//voffset = 136 + 136*sin((-(360*i/80)*MSVC_PI)/180);
			voffset = (FT_DispHeight/2) + ((int32_t)(FT_DispHeight/2)*SAMAPP_qsin(-65536*i/(FT_DispWidth/6))/65536);

			FTImpl.Vertex2f(hoffset*16,voffset*16);		
			for(j=0;j<4;j++)
			{
				hdiff = random(PointSz * 6) - (PointSz*3);
				vdiff = random(PointSz * 6) - (PointSz*3);
				FTImpl.Vertex2f((hoffset + hdiff)*16,(voffset + vdiff)*16);
			}
		}

		FTImpl.End();


		/* Download the DL into DL RAM */
		FTImpl.DLEnd();
		FTImpl.Finish();
		delay(10);

	}
}


/*****************************************************************************/
/* Example code to demonstrated display of point and text                    */
/*****************************************************************************/
void SAMAPP_GPU_FtdiString()
{
	int16_t hoffset,voffset,PointSz;

	voffset = ((FT_DispHeight - 49)/2);//49 is the max height of inbuilt font handle 31
	hoffset = ((FT_DispWidth - 4*36)/2);
	PointSz = 20;
	hoffset += PointSz;
	FTImpl.Write32(FT_RAM_DL + 0,  CLEAR(1, 1, 1)); // clear screen
	FTImpl.Write32(FT_RAM_DL + 4,  BEGIN(FT_BITMAPS)); // start drawing bitmaps

	FTImpl.Write32(FT_RAM_DL + 8,  VERTEX2II(hoffset, voffset, 31, 'F')); // ascii F in font 31
	hoffset += 24;
	FTImpl.Write32(FT_RAM_DL + 12, VERTEX2II(hoffset, voffset, 31, 'T')); // ascii T
	hoffset += 26;
	FTImpl.Write32(FT_RAM_DL + 16, VERTEX2II(hoffset, voffset, 31, 'D')); // ascii D
	hoffset += 29;
	FTImpl.Write32(FT_RAM_DL + 20, VERTEX2II(hoffset, voffset, 31, 'I')); // ascii I
	FTImpl.Write32(FT_RAM_DL + 24, END());
	FTImpl.Write32(FT_RAM_DL + 28, COLOR_RGB(160, 22, 22)); // change color to red
	FTImpl.Write32(FT_RAM_DL + 32, POINT_SIZE(PointSz * 16)); // set point size
	hoffset = ((FT_DispWidth - 4*36)/2);
	FTImpl.Write32(FT_RAM_DL + 36, BEGIN(FT_POINTS)); // start drawing points
	FTImpl.Write32(FT_RAM_DL + 40, VERTEX2II(hoffset, (FT_DispHeight/2), 0, 0)); // red point
	FTImpl.Write32(FT_RAM_DL + 44, END());
	FTImpl.Write32(FT_RAM_DL + 48, DISPLAY()); // display the image

	/* Do a swap */
	FTImpl.Write(REG_DLSWAP,FT_DLSWAP_FRAME);
        delay(1000);
}
#endif

int16_t SAMAPP_CoPro_Logo()
{
  uint32_t coprostatus;
  FTImpl.Cmd_Logo();
  delay(3000);
  Serial.print("Logo app: ");
  do
  {  
    coprostatus = FTImpl.CheckLogo();
  } 
  while(coprostatus!=2);
  Serial.println(coprostatus,HEX);
  return coprostatus;
}


void SAMAPP_CoPro_Widget_Calibrate()
{  
/*************************************************************************/
/* Below code demonstrates the usage of calibrate function. Calibrate    */
/* function will wait untill user presses all the three dots. Only way to*/
/* come out of this api is to reset the coprocessor bit.                 */
/*************************************************************************/
  FTImpl.DLStart();
  FTImpl.ClearColorRGB(64,64,64);
  FTImpl.Clear(1,1,1);    
  FTImpl.ColorRGB(0xff, 0xff, 0xff);
  FTImpl.Cmd_Text((DisplayWidth/2), (DisplayHeight/2), 27, FT_OPT_CENTER, "Please Tap on the dot");
  FTImpl.Cmd_Calibrate(0);
  FTImpl.Finish();
}

/* API to demonstrate gradient widget */
void SAMAPP_CoPro_Widget_Gradient()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of gradient function. Gradient func */
	/* can be used to construct three effects - horizontal, vertical and     */
	/* diagonal effects.                                                      */
	/*************************************************************************/

	int16_t wScissor,hScissor,xOffset,yOffset;

	wScissor = ((FT_DispWidth - 4*10)/3);
	hScissor = ((FT_DispHeight - 3*20)/2);
	xOffset = 10;
	yOffset = 20;
	/* Draw gradient  */
	
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(64,64,64);
	FTImpl.Clear(1,1,1);    
	FTImpl.ColorRGB(0xff, 0xff, 0xff);
	FTImpl.ScissorSize(wScissor,hScissor);
	/* Horizontal gradient effect */
	FTImpl.ScissorXY(xOffset,yOffset);//clip the display 
	FTImpl.Cmd_Gradient(xOffset,yOffset,0x808080,(xOffset + wScissor),yOffset,0xffff00);	
	/* Vertical gradient effect */
	xOffset += wScissor + 10;
	FTImpl.ScissorXY(xOffset,yOffset);//clip the display 
	FTImpl.Cmd_Gradient(xOffset,yOffset,0xff0000,xOffset,(yOffset + hScissor),0x00ff00);	
	/* diagonal gradient effect */
	xOffset += wScissor + 10;
	FTImpl.ScissorXY(xOffset,yOffset);//clip the display 
	FTImpl.Cmd_Gradient(xOffset,yOffset,0x800000,(xOffset + wScissor),(yOffset + hScissor),0xffffff);
	/* Diagonal gradient with text info */
	xOffset = 10;
	yOffset += hScissor + 20;
	FTImpl.ScissorSize(wScissor,30);
	FTImpl.ScissorXY(xOffset,(yOffset + hScissor/2 - 15));//clip the display 
	FTImpl.Cmd_Gradient(xOffset,(yOffset + hScissor/2 - 15),0x606060,(xOffset + wScissor),(yOffset + hScissor/2 + 15),0x404080);
	FTImpl.Cmd_Text((xOffset + wScissor/2), (yOffset + hScissor/2), 28, FT_OPT_CENTER, "Heading 1");//text info

	/* Draw horizontal, vertical and diagonal with alpha */
	xOffset += wScissor + 10;
	FTImpl.ScissorSize(wScissor,hScissor);
	FTImpl.ScissorXY(xOffset,yOffset);//clip the display 
	FTImpl.Cmd_Gradient(xOffset,yOffset,0x808080,(xOffset + wScissor),yOffset,0xffff00);	
	wScissor -= 30; hScissor -= 30;
	FTImpl.ScissorSize(wScissor,hScissor);
	xOffset += 15; yOffset += 15;
	FTImpl.ScissorXY(xOffset,yOffset);//clip the display 
	FTImpl.Cmd_Gradient(xOffset,yOffset,0x800000,xOffset,(yOffset + hScissor),0xffffff);
	wScissor -= 30; hScissor -= 30;
	FTImpl.ScissorSize(wScissor,hScissor);
	xOffset += 15; yOffset += 15;
	FTImpl.ScissorXY(xOffset,yOffset);//clip the display 
	FTImpl.Cmd_Gradient(xOffset,yOffset,0x606060,(xOffset + wScissor),(yOffset + hScissor),0x404080);	

	/* Display the text wrt gradient */
	wScissor = ((FT_DispWidth - 4*10)/3);
	hScissor = ((FT_DispHeight - 3*20)/2);
	xOffset = 10 + wScissor/2;
	yOffset = 20 + hScissor + 5;
	FTImpl.ScissorXY(0,0);//set to default values
	FTImpl.ScissorSize(512,512);
	FTImpl.Cmd_Text(xOffset,yOffset, 26, FT_OPT_CENTER, "Horizontal grad");//text info
	xOffset += wScissor + 10;
	FTImpl.Cmd_Text(xOffset,yOffset, 26, FT_OPT_CENTER, "Vertical grad");//text info
	xOffset += wScissor + 10;
	FTImpl.Cmd_Text(xOffset,yOffset, 26, FT_OPT_CENTER, "Diagonal grad");//text info
    FTImpl.DLEnd();
	FTImpl.Finish();
	delay(3000);
 }
 
/* API to explain the usage of touch engine */
#ifdef SAMAPP_ENABLE_APIS_SET1
void SAMAPP_Touch()
{
	int32_t LoopFlag = 0,wbutton,hbutton,tagval,tagoption;
	char StringArray[100];
	uint32_t ReadWord;
	int16_t xvalue,yvalue,pendown;

	/*************************************************************************/
	/* Below code demonstrates the usage of touch function. Display info     */
	/* touch raw, touch screen, touch tag, raw adc and resistance values     */
	/*************************************************************************/
	LoopFlag = 300;
	wbutton = FT_DispWidth/8;
	hbutton = FT_DispHeight/8;
	while(LoopFlag--)
	{
		
		FTImpl.DLStart();
		FTImpl.ClearColorRGB(64,64,64);
		FTImpl.Clear(1,1,1);    
		FTImpl.ColorRGB(0xff,0xff,0xff);
		FTImpl.TagMask(0);
		/* Draw informative text at width/2,20 location */
		StringArray[0] = '\0';
		strcat(StringArray,"Touch Raw XY (");
		ReadWord = FTImpl.Read32( REG_TOUCH_RAW_XY);
		yvalue = (int16_t)(ReadWord & 0xffff);
		xvalue = (int16_t)((ReadWord>>16) & 0xffff);
		
		FTUtility_Dec2Ascii(StringArray,(int32_t)xvalue);
		strcat(StringArray,",");
		FTUtility_Dec2Ascii(StringArray,(int32_t)yvalue);
		strcat(StringArray,")");
		FTImpl.Cmd_Text(FT_DispWidth/2, 10, 26, FT_OPT_CENTER, StringArray);

		StringArray[0] = '\0';
		strcat(StringArray,"Touch RZ (");
		ReadWord = FTImpl.Read16(REG_TOUCH_RZ);
		FTUtility_Dec2Ascii(StringArray,ReadWord);
		strcat(StringArray,")");
		FTImpl.Cmd_Text(FT_DispWidth/2, 25, 26, FT_OPT_CENTER, StringArray);

		StringArray[0] = '\0';
		strcat(StringArray,"Touch Screen XY (");
		ReadWord = FTImpl.Read32( REG_TOUCH_SCREEN_XY);
		yvalue = (int16_t)(ReadWord & 0xffff);
		xvalue = (int16_t)((ReadWord>>16) & 0xffff);
		FTUtility_Dec2Ascii(StringArray,(int32_t)xvalue);
		strcat(StringArray,",");
		FTUtility_Dec2Ascii(StringArray,(int32_t)yvalue);
		strcat(StringArray,")");
		FTImpl.Cmd_Text(FT_DispWidth/2, 40, 26, FT_OPT_CENTER, StringArray);

		StringArray[0] = '\0';
		strcat(StringArray,"Touch TAG (");
		ReadWord = FTImpl.Read( REG_TOUCH_TAG);
		FTUtility_Dec2Ascii(StringArray,ReadWord);
		strcat(StringArray,")");
		FTImpl.Cmd_Text(FT_DispWidth/2, 55, 26, FT_OPT_CENTER, StringArray);
		tagval = ReadWord;
		StringArray[0] = '\0';
		strcat(StringArray,"Touch Direct XY (");
		ReadWord = FTImpl.Read32( REG_TOUCH_DIRECT_XY);
		yvalue = (int16_t)(ReadWord & 0x03ff);
		xvalue = (int16_t)((ReadWord>>16) & 0x03ff);
		FTUtility_Dec2Ascii(StringArray,(int32_t)xvalue);
		strcat(StringArray,",");
		FTUtility_Dec2Ascii(StringArray,(int32_t)yvalue);
		pendown = (int16_t)((ReadWord>>31) & 0x01);
		strcat(StringArray,",");
		FTUtility_Dec2Ascii(StringArray,(int32_t)pendown);
		strcat(StringArray,")");
		FTImpl.Cmd_Text(FT_DispWidth/2, 70, 26, FT_OPT_CENTER, StringArray);

		StringArray[0] = '\0';
		strcat(StringArray,"Touch Direct Z1Z2 (");
		ReadWord = FTImpl.Read32( REG_TOUCH_DIRECT_Z1Z2);
		yvalue = (int16_t)(ReadWord & 0x03ff);
		xvalue = (int16_t)((ReadWord>>16) & 0x03ff);
		FTUtility_Dec2Ascii(StringArray,(int32_t)xvalue);
		strcat(StringArray,",");
		FTUtility_Dec2Ascii(StringArray,(int32_t)yvalue);
		strcat(StringArray,")");

		FTImpl.Cmd_Text(FT_DispWidth/2, 85, 26, FT_OPT_CENTER, StringArray);

		FTImpl.Cmd_FGColor(0x008000);
		FTImpl.TagMask(1);
		tagoption = 0;
		if(12 == tagval)
		{
			tagoption = FT_OPT_FLAT;
		}

		FTImpl.Tag(12);
		FTImpl.Cmd_Button((FT_DispWidth/4) - (wbutton/2),(FT_DispHeight*2/4) - (hbutton/2),wbutton,hbutton,26,tagoption,"Tag12");
		FTImpl.Tag(13);
		tagoption = 0;
		if(13 == tagval)
		{
			tagoption = FT_OPT_FLAT;
		}
		FTImpl.Cmd_Button((FT_DispWidth*3/4) - (wbutton/2),(FT_DispHeight*3/4) - (hbutton/2),wbutton,hbutton,26,tagoption,"Tag13");
		FTImpl.DLEnd();
		FTImpl.Finish();	
		delay(30);
	
	}
}
#endif


/* API to demonstrate clock widget */
void SAMAPP_CoPro_Widget_Clock()
{

	/*************************************************************************/
	/* Below code demonstrates the usage of clock function. Clocks can be    */
	/* constructed using flat or 3d effect. Clock background and foreground  */
	/* colors can be set by gbcolor and colorrgb. Clock can be constructed   */
	/* with multiple options such as no background, no needles, no pointer.  */
	/*************************************************************************/
	{
	int16_t xOffset,yOffset,cRadius,xDistBtwClocks;

	xDistBtwClocks = FT_DispWidth/5;
	cRadius = xDistBtwClocks/2 - FT_DispWidth/64;

	/* Download the bitmap data for lena faced clock */		
	FTImpl.Writefromflash(FT_RAM_G,(uint8_t *)&SAMAPP_Bitmap_RawData[SAMAPP_Bitmap_RawData_Header[0].Arrayoffset],
		SAMAPP_Bitmap_RawData_Header[0].Stride*SAMAPP_Bitmap_RawData_Header[0].Height);

	/* Draw clock with blue as background and read as needle color */
	
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(64,64,64);
	FTImpl.Clear(1,1,1);    
	FTImpl.ColorRGB(255,255,255);
	/* flat effect and default color background */
	xOffset = xDistBtwClocks/2;
	yOffset = cRadius + 5;
	FTImpl.Cmd_BGColor( 0x0000ff);
	FTImpl.ColorRGB(0xff,0x00,0x00);
	FTImpl.Cmd_Clock( xOffset,yOffset,cRadius,FT_OPT_FLAT,30,100,5,10);
	FTImpl.Cmd_Text(xOffset, (yOffset + cRadius + 6), 26, FT_OPT_CENTER, "Flat effect");//text info
	/* no seconds needle */
	FTImpl.Cmd_BGColor( 0x00ff00);
	FTImpl.ColorRGB(0xff,0x00,0x00);
	FTImpl.Cmd_FGColor(0xff0000);
	xOffset += xDistBtwClocks;
	FTImpl.Cmd_Clock( xOffset,yOffset,cRadius,FT_OPT_NOSECS,10,10,5,10);
	FTImpl.ColorA(255);
	FTImpl.Cmd_Text(xOffset, (yOffset + cRadius + 6), 26, FT_OPT_CENTER, "No Secs");//text info
	/* no background color */
	FTImpl.Cmd_BGColor( 0x00ffff);
	FTImpl.ColorRGB(0xff,0xff,0x00);
	xOffset += xDistBtwClocks;
	FTImpl.Cmd_Clock( xOffset,yOffset,cRadius,FT_OPT_NOBACK,10,10,5,10);
	FTImpl.Cmd_Text(xOffset, (yOffset + cRadius + 6), 26, FT_OPT_CENTER, "No BG");//text info
	/* No ticks */
	FTImpl.Cmd_BGColor( 0xff00ff);
	FTImpl.ColorRGB(0x00,0xff,0xff);
	xOffset += xDistBtwClocks;
	FTImpl.Cmd_Clock( xOffset,yOffset,cRadius,FT_OPT_NOTICKS,10,10,5,10);
	FTImpl.Cmd_Text(xOffset, (yOffset + cRadius + 6), 26, FT_OPT_CENTER, "No Ticks");//text info
	/* No hands */
	FTImpl.Cmd_BGColor( 0x808080);
	FTImpl.ColorRGB(0x00,0xff,0x00);
	xOffset += xDistBtwClocks;
	FTImpl.Cmd_Clock( xOffset,yOffset,cRadius,FT_OPT_NOHANDS,10,10,5,10);
	FTImpl.Cmd_Text(xOffset, (yOffset + cRadius + 6), 26, FT_OPT_CENTER, "No Hands");//text info
	/* Bigger clock */
	yOffset += (cRadius + 10);
	cRadius = FT_DispHeight - (2*cRadius + 5 + 10);//calculate radius based on remaining height
	cRadius = (cRadius - 5 - 20)/2;
	xOffset = cRadius + 10;
	yOffset += cRadius + 5;
	FTImpl.ColorRGB(0x00,0x00,0xff);
	FTImpl.Cmd_Clock( xOffset,yOffset,cRadius,0,10,10,5,10);

	xOffset += 2 * cRadius + 10;
	/* Lena clock with no background and no ticks */
	FTImpl.LineWidth(10*16);
	FTImpl.ColorRGB(0xff,0xff,0xff);
	FTImpl.Begin(FT_RECTS);	
	FTImpl.Vertex2ii((xOffset - cRadius + 10)*16,(yOffset - cRadius + 10)*16);
	FTImpl.Vertex2ii((xOffset + cRadius - 10)*16,(yOffset + cRadius - 10)*16);
	FTImpl.End();
	FTImpl.ColorA(0xff);
	FTImpl.ColorRGB(0xff,0xff,0xff);
	FTImpl.ColorMask(0,0,0,1);
	FTImpl.Clear(1,1,1);
	FTImpl.Begin(FT_RECTS);	
	FTImpl.Vertex2ii((xOffset - cRadius + 12)*16,(yOffset - cRadius + 12)*16);
	FTImpl.Vertex2ii((xOffset + cRadius - 12)*16,(yOffset + cRadius - 12)*16);
	FTImpl.End();
	FTImpl.ColorMask(1,1,1,1);
	FTImpl.BlendFunc(FT_DST_ALPHA,FT_ONE_MINUS_DST_ALPHA);
	/* Lena bitmap - scale proportionately wrt output resolution */	
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Scale( 65536*2*cRadius/SAMAPP_Bitmap_RawData_Header[0].Width,65536*2*cRadius/SAMAPP_Bitmap_RawData_Header[0].Height);
	FTImpl.Cmd_SetMatrix( );
	FTImpl.Begin(FT_BITMAPS);
	FTImpl.BitmapSource(0);
	FTImpl.BitmapLayout(SAMAPP_Bitmap_RawData_Header[0].Format,
		SAMAPP_Bitmap_RawData_Header[0].Stride,SAMAPP_Bitmap_RawData_Header[0].Height);
	FTImpl.BitmapSize(FT_BILINEAR,FT_BORDER,FT_BORDER,
		2*cRadius,2*cRadius);
	FTImpl.Vertex2ii((xOffset - cRadius)*16,(yOffset - cRadius)*16);
	FTImpl.End();
	FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE_MINUS_SRC_ALPHA);
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_SetMatrix( );
	FTImpl.ColorRGB(0xff,0xff,0xff);
	FTImpl.Cmd_Clock( xOffset,yOffset,cRadius,FT_OPT_NOTICKS | FT_OPT_NOBACK,10,10,5,10);
	FTImpl.DLEnd();
	FTImpl.Finish();	
	delay(1000);
	}	
}
void SAMAPP_CoPro_Widget_Guage()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of gauge function. Gauge can be     */
	/* constructed using flat or 3d effect. Gauge background and foreground  */
	/* colors can be set by gbcolor and colorrgb. Gauge can be constructed   */
	/* with multiple options such as no background, no minors/majors and     */
	/* no pointer.                                                           */
	/*************************************************************************/
	{
	int16_t xOffset,yOffset,cRadius,xDistBtwClocks;

	xDistBtwClocks = FT_DispWidth/5;
	cRadius = xDistBtwClocks/2 - FT_DispWidth/64;
		
	/* Download the bitmap data */		
	FTImpl.Writefromflash(FT_RAM_G,(uint8_t *)&SAMAPP_Bitmap_RawData[SAMAPP_Bitmap_RawData_Header[0].Arrayoffset],
		SAMAPP_Bitmap_RawData_Header[0].Stride*SAMAPP_Bitmap_RawData_Header[0].Height);

	/* Draw gauge with blue as background and read as needle color */
	
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(64,64,64);
	FTImpl.Clear(1,1,1);
	FTImpl.ColorRGB(255,255,255);
	/* flat effect and default color background */
	xOffset = xDistBtwClocks/2;
	yOffset = cRadius + 5;
	FTImpl.Cmd_BGColor( 0x0000ff);
	FTImpl.ColorRGB(0xff,0x00,0x00);
	FTImpl.Cmd_Gauge( xOffset,yOffset,cRadius,FT_OPT_FLAT,5,4,45,100);
	FTImpl.Cmd_Gauge( xOffset,yOffset,cRadius,FT_OPT_FLAT,5,4,45,100);
	FTImpl.Cmd_Text(xOffset, (yOffset + cRadius + 6), 26,FT_OPT_CENTER, "Flat effect");//text info
	/* 3d effect */
	FTImpl.Cmd_BGColor( 0x00ff00);
	FTImpl.ColorRGB(0xff,0x00,0x00);
	FTImpl.Cmd_FGColor(0xff0000);
	xOffset += xDistBtwClocks;
	FTImpl.Cmd_Gauge( xOffset,yOffset,cRadius,0,5,1,60,100);
	FTImpl.Cmd_Text(xOffset, (yOffset + cRadius + 6), 26, FT_OPT_CENTER, "3d effect");//text info
	/* no background color */
	FTImpl.Cmd_BGColor( 0x00ffff);
	FTImpl.ColorRGB(0xff,0xff,0x00);
	xOffset += xDistBtwClocks;
	FTImpl.Cmd_Gauge( xOffset,yOffset,cRadius,FT_OPT_NOBACK,1,6,90,100);
	FTImpl.ColorA(255);
	FTImpl.Cmd_Text(xOffset, (yOffset + cRadius + 6), 26, FT_OPT_CENTER, "No BG");//text info
	/* No ticks */
	FTImpl.Cmd_BGColor( 0xff00ff);
	FTImpl.ColorRGB(0x00,0xff,0xff);
	xOffset += xDistBtwClocks;
	FTImpl.Cmd_Gauge( xOffset,yOffset,cRadius,FT_OPT_NOTICKS,5,4,20,100);
	FTImpl.Cmd_Text(xOffset, (yOffset + cRadius + 6), 26, FT_OPT_CENTER, "No Ticks");//text info
	/* No hands */
	FTImpl.Cmd_BGColor( 0x808080);
	FTImpl.ColorRGB(0x00,0xff,0x00);
	xOffset += xDistBtwClocks;
	FTImpl.Cmd_Gauge( xOffset,yOffset,cRadius,FT_OPT_NOHANDS,5,4,55,100);
	FTImpl.ColorA(255);
	FTImpl.Cmd_Text(xOffset, (yOffset + cRadius + 6), 26, FT_OPT_CENTER, "No Hands");//text info
	/* Bigger gauge */
	yOffset += cRadius + 10;
	cRadius = FT_DispHeight - (2*cRadius + 5 + 10);//calculate radius based on remaining height
	cRadius = (cRadius - 5 - 20)/2;
	xOffset = cRadius + 10;
	yOffset += cRadius + 5;
	FTImpl.Cmd_BGColor( 0x808000);
	FTImpl.ColorRGB(0xff,0xff,0xff);
	FTImpl.Cmd_Gauge( xOffset,yOffset,cRadius,FT_OPT_NOPOINTER,5,4,80,100);
	FTImpl.ColorRGB(0xff,0x00,0x00);
	FTImpl.Cmd_Gauge( xOffset,yOffset,cRadius,FT_OPT_NOTICKS | FT_OPT_NOBACK,5,4,30,100);

	xOffset += 2*cRadius + 10;
	/* Lena guage with no background and no ticks */
	FTImpl.PointSize(cRadius*16);
	FTImpl.ColorRGB(0xff,0xff,0xff);
	FTImpl.Begin(FT_POINTS);	
	FTImpl.Vertex2ii(xOffset*16,yOffset*16);
	FTImpl.End();
	FTImpl.ColorA(0xff);
	FTImpl.ColorRGB(0xff,0xff,0xff);
	FTImpl.ColorMask(0,0,0,1);
	FTImpl.Clear(1,1,1);
	FTImpl.Begin(FT_POINTS);	
	FTImpl.PointSize((cRadius - 2)*16);
	FTImpl.Vertex2ii(xOffset*16,yOffset*16);
	FTImpl.End();
	FTImpl.ColorMask(1,1,1,1);
	FTImpl.BlendFunc(FT_DST_ALPHA,FT_ONE_MINUS_DST_ALPHA);
	/* Lena bitmap - scale proportionately wrt output resolution */	
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Scale(65536*2*cRadius/SAMAPP_Bitmap_RawData_Header[0].Width,65536*2*cRadius/SAMAPP_Bitmap_RawData_Header[0].Height);
	FTImpl.Cmd_SetMatrix( );
	FTImpl.Begin(FT_BITMAPS);
	FTImpl.BitmapSource(0);
	FTImpl.BitmapLayout(SAMAPP_Bitmap_RawData_Header[0].Format,
		SAMAPP_Bitmap_RawData_Header[0].Stride,SAMAPP_Bitmap_RawData_Header[0].Height);
	FTImpl.BitmapSize(FT_BILINEAR,FT_BORDER,FT_BORDER,2*cRadius,2*cRadius);
	FTImpl.Vertex2ii((xOffset - cRadius)*16,(yOffset - cRadius)*16);
	FTImpl.End();
	FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE_MINUS_SRC_ALPHA);
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_SetMatrix( );
	
	FTImpl.ColorRGB(0xff,0xff,0xff);
	FTImpl.Cmd_Gauge( xOffset,yOffset,cRadius,FT_OPT_NOTICKS | FT_OPT_NOBACK,5,4,30,100);
	FTImpl.DLEnd();
	FTImpl.Finish();	
	delay(1000);
	}
	
}
 /* API to demonstrate keys widget */
void SAMAPP_CoPro_Widget_Keys()
{
/*************************************************************************/
/* Below code demonstrates the usage of keys function. keys function     */
/* draws buttons with characters given as input parameters. Keys support */
/* Flat and 3D effects, draw at (x,y) coordinates or center of the display*/
/* , inbuilt or custom fonts can be used for key display                 */
/*************************************************************************/
	int16_t TextFont = 29,ButtonW = 30,ButtonH = 30,yBtnDst = 5,yOffset,xOffset;
#ifdef SAMAPP_DISPLAY_QVGA
	TextFont = 27;
	ButtonW = 22;
	ButtonH = 22;
	yBtnDst = 3;
#endif
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(64,64,64);
	FTImpl.Clear(1,1,1);    
	FTImpl.ColorRGB(0xff, 0xff, 0xff);
	/* Draw keys with flat effect */
	FTImpl.Cmd_FGColor(0x404080);	
	FTImpl.Cmd_Keys( 10, 10, 4*ButtonW, 30, TextFont, FT_OPT_FLAT, "ABCD");
	FTImpl.Cmd_Text(10, 40, 26, 0, "Flat effect");//text info
	/* Draw keys with 3d effect */
	FTImpl.Cmd_FGColor(0x800000);
	xOffset = 4*ButtonW + 20;
	FTImpl.Cmd_Keys( xOffset, 10, 4*ButtonW, 30, TextFont, 0, "ABCD");
	FTImpl.Cmd_Text(xOffset, 40, 26, 0, "3D effect");//text info
	/* Draw keys with center option */
	FTImpl.Cmd_FGColor(0xffff000);
	xOffset += 4*ButtonW + 20;
	FTImpl.Cmd_Keys( xOffset, 10, (FT_DispWidth - 230), 30, TextFont, FT_OPT_CENTER, "ABCD");
	FTImpl.Cmd_Text(xOffset, 40, 26, 0, "Option Center");//text info

	yOffset = 80 + 10;
	/* Construct a simple keyboard - note that the tags associated with the keys are the character values given in the arguments */
	FTImpl.Cmd_FGColor(0x404080);
	FTImpl.Cmd_GradColor(0x00ff00);
	FTImpl.Cmd_Keys( yBtnDst, yOffset, 10*ButtonW, ButtonH, TextFont, FT_OPT_CENTER, "qwertyuiop");
	FTImpl.Cmd_GradColor(0x00ffff);
	yOffset += ButtonH + yBtnDst;
	FTImpl.Cmd_Keys( yBtnDst, yOffset, 10*ButtonW, ButtonH, TextFont, FT_OPT_CENTER, "asdfghjkl");
	FTImpl.Cmd_GradColor(0xffff00);
	yOffset += ButtonH + yBtnDst;
	FTImpl.Cmd_Keys( yBtnDst, yOffset, 10*ButtonW, ButtonH, TextFont, (FT_OPT_CENTER | 'z'), "zxcvbnm");//hilight button z
	yOffset += ButtonH + yBtnDst;
	FTImpl.Cmd_Button(yBtnDst, yOffset, 10*ButtonW, ButtonH, TextFont, FT_OPT_CENTER, " ");//mandatory to give '\0' at the end to make sure coprocessor understands the string end
	yOffset = 80 + 10;
	FTImpl.Cmd_Keys( 11*ButtonW, yOffset, 3*ButtonW, ButtonH, TextFont, 0, "789");
	yOffset += ButtonH + yBtnDst;
	FTImpl.Cmd_Keys( 11*ButtonW, yOffset, 3*ButtonW, ButtonH, TextFont, 0, "456");
	yOffset += ButtonH + yBtnDst;
	FTImpl.Cmd_Keys( 11*ButtonW, yOffset, 3*ButtonW, ButtonH, TextFont, 0, "123");
	yOffset += ButtonH + yBtnDst;
	FTImpl.ColorA(255);
	FTImpl.Cmd_Keys( 11*ButtonW, yOffset, 3*ButtonW, ButtonH, TextFont, (0 | '0'), "0.");//hilight button 0
	FTImpl.DLEnd();
	FTImpl.Finish();
	delay(3000);	
}

void SAMAPP_CoPro_Widget_Keys_Interactive()
{
/*************************************************************************/
/* Below code demonstrates the usage of keys function. keys function     */
/* draws buttons with characters given as input parameters. Keys support */
/* Flat and 3D effects, draw at (x,y) coordinates or center of the display*/
/* , inbuilt or custom fonts can be used for key display                 */
/*************************************************************************/

	int32_t loopflag = 600;
	int16_t TextFont = 29,ButtonW = 30,ButtonH = 30,yBtnDst = 5,yOffset,xOffset;
#define SAMAPP_COPRO_WIDGET_KEYS_INTERACTIVE_TEXTSIZE (512)
              char DispText[SAMAPP_COPRO_WIDGET_KEYS_INTERACTIVE_TEXTSIZE],CurrChar = '|';
              uint8_t CurrTag = 0,PrevTag = 0,Pendown = 1;
              int32_t CurrTextIdx = 0;
#ifdef SAMAPP_DISPLAY_QVGA
              TextFont = 27;
              ButtonW = 22;
              ButtonH = 22;
              yBtnDst = 3;
#endif
    while(loopflag --)
    {


	   /* Check the user input and then add the characters into array */
	   CurrTag = FTImpl.Read(REG_TOUCH_TAG);
	   Pendown = ((FTImpl.Read32(REG_TOUCH_DIRECT_XY)>>31) & 0x01);

       CurrChar = CurrTag;
       if(0 == CurrTag)
       {
              CurrChar = '|';
       }

       /* check whether pwndown has happened */
       if(( 1 == Pendown) && (0 != PrevTag))
       {
              CurrTextIdx++;
              /* clear all the charaters after 100 are pressed */
              if(CurrTextIdx > 24)
              {
                     CurrTextIdx = 0;
              }
       }
		FTImpl.DLStart();
		FTImpl.ClearColorRGB(64,64,64);
		FTImpl.Clear(1,1,1);    
		FTImpl.ColorRGB(0xff, 0xff, 0xff);
       /* Draw text entered by user */   
       /* make sure the array is a string */
       DispText[CurrTextIdx] = CurrChar;
       DispText[CurrTextIdx + 1] = '\0';

       FTImpl.TagMask(0);
       FTImpl.Cmd_Text(FT_DispWidth/2, 40, TextFont, FT_OPT_CENTER, DispText);//text info
       FTImpl.TagMask(1);


       yOffset = 80 + 10;
       /* Construct a simple keyboard - note that the tags associated with the keys are the character values given in the arguments */
       FTImpl.Cmd_FGColor(0x404080);
       FTImpl.Cmd_GradColor(0x00ff00);
       FTImpl.Cmd_Keys(yBtnDst, yOffset, 10*ButtonW, ButtonH, TextFont, (FT_OPT_CENTER | CurrTag), "qwertyuiop");
       FTImpl.Cmd_GradColor(0x00ffff);
       yOffset += ButtonH + yBtnDst;
       FTImpl.Cmd_Keys(yBtnDst, yOffset, 10*ButtonW, ButtonH, TextFont, (FT_OPT_CENTER | CurrTag), "aasdfghjkl);
       FTImpl.Cmd_GradColor(0xffff00);
       yOffset += ButtonH + yBtnDst;
       FTImpl.Cmd_Keys(yBtnDst, yOffset, 10*ButtonW, ButtonH, TextFont, (FT_OPT_CENTER | CurrTag), "zxcvbnm");//hilight button z
       yOffset += ButtonH + yBtnDst;
       FTImpl.Tag(' ');
       if(' ' == CurrTag)
       {
			FTImpl.Cmd_Button(yBtnDst, yOffset, 10*ButtonW, ButtonH, TextFont, FT_OPT_CENTER | FT_OPT_FLAT, " ");//mandatory to give '\0' at the end to make sure coprocessor understands the string end
       }
       else
       {
            FTImpl.Cmd_Button(yBtnDst, yOffset, 10*ButtonW, ButtonH, TextFont, FT_OPT_CENTER, " ");//mandatory to give '\0' at the end to make sure coprocessor understands the string end
       }
       yOffset = 80 + 10;
       FTImpl.Cmd_Keys( 11*ButtonW, yOffset, 3*ButtonW, ButtonH, TextFont, (0 | CurrTag), "789");
       yOffset += ButtonH + yBtnDst;
       FTImpl.Cmd_Keys( 11*ButtonW, yOffset, 3*ButtonW, ButtonH, TextFont, (0 | CurrTag), "456");
       yOffset += ButtonH + yBtnDst;
       FTImpl.Cmd_Keys( 11*ButtonW, yOffset, 3*ButtonW, ButtonH, TextFont, (0 | CurrTag), "123");
       yOffset += ButtonH + yBtnDst;
       FTImpl.ColorA(255);
       FTImpl.Cmd_Keys( 11*ButtonW, yOffset, 3*ButtonW, ButtonH, TextFont, (0 | CurrTag), "0.");//hilight button 0
	   FTImpl.DLEnd();
	   FTImpl.Finish();
       delay(10);
       PrevTag = CurrTag;
    } 
}



void SAMAPP_CoPro_Widget_Progressbar()
{
/*************************************************************************/
/* Below code demonstrates the usage of progress function. Progress func */
/* draws process bar with fgcolor for the % completion and bgcolor for   */
/* % remaining. Progress bar supports flat and 3d effets                 */
/*************************************************************************/
	int16_t xOffset,yOffset,yDist = FT_DispWidth/12,ySz = FT_DispWidth/24;

	FTImpl.DLStart();
	FTImpl.ClearColorRGB(64,64,64);
	FTImpl.Clear(1,1,1);    
	FTImpl.ColorRGB(0xff, 0xff, 0xff);
	/* Draw progress bar with flat effect */
	FTImpl.Cmd_BGColor( 0x404080);
	FTImpl.Cmd_Progress( 20, 10, 120, 20, FT_OPT_FLAT, 50, 100);//note that h/2 will be added on both sides of the progress bar
	FTImpl.ColorA(255);
	FTImpl.Cmd_Text(20, 40, 26, 0, "Flat effect");//text info
	/* Draw progress bar with 3d effect */
	FTImpl.ColorRGB(0x00,0xff,0x00);
	FTImpl.Cmd_BGColor( 0x800000);
	FTImpl.Cmd_Progress( 180, 10, 120, 20, 0, 75, 100);
	FTImpl.ColorA(255);
	FTImpl.Cmd_Text(180, 40, 26, 0, "3D effect");//text info
	/* Draw progress bar with 3d effect and string on top */
	FTImpl.ColorRGB(0xff, 0x0, 0x0);
	FTImpl.Cmd_BGColor( 0x000080);
	FTImpl.Cmd_Progress( 30, 60, 120, 30, 0, 19660, 65535);
	FTImpl.ColorRGB(0xff, 0xff, 0xff);
	FTImpl.Cmd_Text(78, 68, 26, 0, "30 %");//text info

	xOffset = 20;yOffset = 120;
	FTImpl.ColorRGB(0x00,0xa0,0x00);
	FTImpl.Cmd_BGColor( 0x800000);
	FTImpl.Cmd_Progress( xOffset, yOffset, 150, ySz, 0, 10, 100);	
	FTImpl.Cmd_BGColor( 0x000080);
	yOffset += yDist;
	FTImpl.Cmd_Progress( xOffset, yOffset, 150, ySz, 0, 40, 100);	
	FTImpl.Cmd_BGColor( 0xffff00);
	yOffset += yDist;
	FTImpl.Cmd_Progress( xOffset, yOffset, 150, ySz, 0, 70, 100);	
	FTImpl.Cmd_BGColor( 0x808080);
	yOffset += yDist;
	FTImpl.Cmd_Progress( xOffset, yOffset, 150, ySz, 0, 90, 100);

	FTImpl.Cmd_Text(xOffset + 180, 80, 26, 0, "40 % TopBottom");//text info
	FTImpl.Cmd_Progress( xOffset + 180, 100, ySz, 150, 0, 40, 100);

	FTImpl.DLEnd();
	FTImpl.Finish();
    delay(3000); 
}


void SAMAPP_CoPro_Widget_Scroll()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of scroll function. Scroll function */
	/* draws scroll bar with fgcolor for inner color and current location and*/
	/* can be given by val parameter */
	/*************************************************************************/
	int16_t xOffset,yOffset,xDist = FT_DispWidth/12,yDist = FT_DispWidth/12,wSz;

	FTImpl.DLStart();
	FTImpl.ClearColorRGB(64,64,64);
	FTImpl.Clear(1,1,1);    
	FTImpl.ColorRGB(0xff, 0xff, 0xff);
	/* Draw scroll bar with flat effect */
	FTImpl.Cmd_FGColor(0xffff00);
	FTImpl.Cmd_BGColor( 0x404080);
	FTImpl.Cmd_Scrollbar( 20, 10, 120, 8, FT_OPT_FLAT, 20, 30, 100);//note that h/2 size will be added on both sides of the progress bar
	FTImpl.ColorA(255);
	FTImpl.Cmd_Text(20, 40, 26, 0, "Flat effect");//text info
	/* Draw scroll bar with 3d effect */
	FTImpl.Cmd_FGColor(0x00ff00);
	FTImpl.Cmd_BGColor( 0x800000);
	FTImpl.Cmd_Scrollbar( 180, 10, 120, 8, 0, 20, 30, 100);
	FTImpl.ColorA(255);
	FTImpl.Cmd_Text(180, 40, 26, 0, "3D effect");//text info
	
	xOffset = 20;
	yOffset = 120;
	wSz = ((FT_DispWidth/2) - 40);
	/* Draw horizontal scroll bars */
	FTImpl.Cmd_FGColor(0x00a000);
	FTImpl.Cmd_BGColor( 0x800000);
	FTImpl.Cmd_Scrollbar( xOffset, yOffset, wSz, 8, 0, 10, 30, 100);
	FTImpl.Cmd_BGColor( 0x000080);
	yOffset += yDist;
	FTImpl.Cmd_Scrollbar( xOffset, yOffset, wSz, 8, 0, 30, 30, 100);
	FTImpl.ColorA((255));
	FTImpl.Cmd_BGColor( 0xffff00);
	yOffset += yDist;
	FTImpl.Cmd_Scrollbar( xOffset, yOffset, wSz, 8, 0, 50, 30, 100);
	FTImpl.Cmd_BGColor( 0x808080);
	yOffset += yDist;
	FTImpl.Cmd_Scrollbar( xOffset, yOffset, wSz, 8, 0, 70, 30, 100);

	xOffset = (FT_DispWidth/2) + 40;
	yOffset = 80;
	wSz = (FT_DispHeight - 100);
	/* draw vertical scroll bars */
	FTImpl.Cmd_BGColor( 0x800000);
	FTImpl.Cmd_Scrollbar( xOffset, yOffset, 8, wSz, 0, 10, 30, 100);
	FTImpl.Cmd_BGColor( 0x000080);
	xOffset += xDist;
	FTImpl.Cmd_Scrollbar( xOffset, yOffset, 8, wSz, 0, 30, 30, 100);
	FTImpl.ColorA((255));
	FTImpl.Cmd_BGColor( 0xffff00);
	xOffset += xDist;
	FTImpl.Cmd_Scrollbar( xOffset, yOffset, 8, wSz, 0, 50, 30, 100);
	FTImpl.Cmd_BGColor( 0x808080);
	xOffset += xDist;
	FTImpl.Cmd_Scrollbar( xOffset, yOffset, 8, wSz, 0, 70, 30, 100);

	FTImpl.DLEnd();
	FTImpl.Finish();
    delay(3000); 
}
/* API to demonstrate slider widget */
void SAMAPP_CoPro_Widget_Slider()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of slider function. Slider function */
	/* draws slider bar with fgcolor for inner color and bgcolor for the knob*/
	/* , contains input parameter for position of the knob                   */
	/*************************************************************************/	
	int16_t xOffset,yOffset,xDist = FT_DispWidth/12,yDist = FT_DispWidth/12,wSz;
	
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(64,64,64);
	FTImpl.Clear(1,1,1);    
	FTImpl.ColorRGB(0xff, 0xff, 0xff);
	/* Draw scroll bar with flat effect */
	FTImpl.Cmd_FGColor(0xffff00);
	FTImpl.Cmd_BGColor( 0x000080);
	FTImpl.Cmd_Slider( 20, 10, 120, 10, FT_OPT_FLAT, 30, 100);//note that h/2 size will be added on both sides of the progress bar
	FTImpl.ColorA((255));
	FTImpl.Cmd_Text(20, 40, 26, 0, "Flat effect");//text info
	/* Draw scroll bar with 3d effect */
	FTImpl.Cmd_FGColor(0x00ff00);
	FTImpl.Cmd_BGColor( 0x800000);
	FTImpl.Cmd_Slider( 180, 10, 120, 10, 0, 50, 100);
	FTImpl.ColorA((255));
	FTImpl.Cmd_Text(180, 40, 26, 0, "3D effect");//text info

	xOffset = 20;
	yOffset = 120;
	wSz = ((FT_DispWidth/2) - 40);
	/* Draw horizontal slider bars */	
	FTImpl.Cmd_FGColor(0x00a000);
	FTImpl.Cmd_BGColor( 0x800000);
	FTImpl.ColorRGB(41,1,5);
	FTImpl.Cmd_Slider( xOffset, yOffset, wSz, 10, 0, 10, 100);
	FTImpl.ColorRGB(11,7,65);
	FTImpl.Cmd_BGColor( 0x000080);
	yOffset += yDist;
	FTImpl.Cmd_Slider( xOffset, yOffset, wSz, 10, 0, 30, 100);
	FTImpl.ColorA((255));
	FTImpl.Cmd_BGColor( 0xffff00);
	FTImpl.ColorRGB(87,94,9);
	yOffset += yDist;
	FTImpl.Cmd_Slider( xOffset, yOffset, wSz, 10, 0, 50, 100);
	FTImpl.Cmd_BGColor( 0x808080);
	FTImpl.ColorRGB(51,50,52);
	yOffset += yDist;
	FTImpl.Cmd_Slider( xOffset, yOffset, wSz, 10, 0, 70, 100);

	xOffset = (FT_DispWidth/2) + 40;
	yOffset = 80;
	wSz = (FT_DispHeight - 100);
	/* draw vertical slider bars */
	FTImpl.Cmd_BGColor( 0x800000);
	FTImpl.Cmd_Slider( xOffset, yOffset, 10, wSz, 0, 10, 100);
	FTImpl.Cmd_BGColor( 0x000080);
	xOffset += xDist;
	FTImpl.Cmd_Slider( xOffset, yOffset, 10, wSz, 0, 30, 100);
	FTImpl.ColorA((255));
	FTImpl.Cmd_BGColor( 0xffff00);
	xOffset += xDist;
	FTImpl.Cmd_Slider( xOffset, yOffset, 10, wSz, 0, 50, 100);
	FTImpl.Cmd_BGColor( 0x808080);
	xOffset += xDist;
	FTImpl.Cmd_Slider( xOffset, yOffset, 10, wSz, 0, 70, 100);
	FTImpl.DLEnd();
	FTImpl.Finish();
    delay(3000); 
}

void SAMAPP_CoPro_Widget_Dial()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of dial function. Dial function     */
	/* draws rounded bar with fgcolor for knob color and colorrgb for pointer*/
	/* , contains input parameter for angle of the pointer                   */
	/*************************************************************************/
	
	
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(64,64,64);
	FTImpl.Clear(1,1,1);    
	FTImpl.ColorRGB(0xff, 0xff, 0xff);
	/* Draw dial with flat effect */
	FTImpl.Cmd_FGColor(0xffff00);
	FTImpl.Cmd_BGColor( 0x000080);
	FTImpl.Cmd_Dial( 50, 50, 40, FT_OPT_FLAT, 0.2*65535);//20%
	FTImpl.ColorA((255));
	FTImpl.Cmd_Text(15, 90, 26, 0, "Flat effect");//text info
	/* Draw dial with 3d effect */
	FTImpl.Cmd_FGColor(0x00ff00);
	FTImpl.Cmd_BGColor( 0x800000);
	FTImpl.Cmd_Dial( 140, 50, 40, 0, 0.45*65535);//45%
	FTImpl.ColorA((255));
	FTImpl.Cmd_Text(105, 90, 26, 0, "3D effect");//text info

	/* Draw increasing dials horizontally */	
	FTImpl.Cmd_FGColor(0x800000);
	FTImpl.ColorRGB(41,1,5);
	FTImpl.Cmd_Dial( 30, 160, 20, 0, 0.30*65535);
	FTImpl.Cmd_Text(20, 180, 26, 0, "30 %");//text info
	FTImpl.ColorRGB(11,7,65);
	FTImpl.Cmd_FGColor(0x000080);
	FTImpl.Cmd_Dial( 100, 160, 40, 0, 0.45*65535);
	FTImpl.ColorA((255));
	FTImpl.Cmd_Text(90, 200, 26, 0, "45 %");//text info
	FTImpl.Cmd_FGColor(0xffff00);
	FTImpl.ColorRGB(87,94,9);
	FTImpl.Cmd_Dial( 210, 160, 60, 0, 0.60*65535);
	FTImpl.Cmd_Text(200, 220, 26, 0, "60 %");//text info
	FTImpl.Cmd_FGColor(0x808080);

#ifndef SAMAPP_DISPLAY_QVGA
	FTImpl.ColorRGB(51,50,52);
	FTImpl.Cmd_Dial( 360, 160, 80, 0, 0.75*65535);
	FTImpl.Cmd_Text(350, 240, 26, 0, "75 %");//text info
#endif
	FTImpl.DLEnd();
	FTImpl.Finish();
    delay(3000); 
}
/* API to demonstrate toggle widget */
void SAMAPP_CoPro_Widget_Toggle()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of toggle function. Toggle function */
	/* draws line with inside knob to choose between left and right. Toggle  */
	/* inside color can be changed by bgcolor and knob color by fgcolor. Left*/
	/* right texts can be written and the color of the text can be changed by*/
	/* colorrgb graphics function                                            */
	/*************************************************************************/
	int16_t xOffset,yOffset,yDist = 40;
	
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(64,64,64);
	FTImpl.Clear(1,1,1);    
	FTImpl.ColorRGB(0xff, 0xff, 0xff);
	/* Draw toggle with flat effect */
	FTImpl.Cmd_FGColor(0xffff00);
	FTImpl.Cmd_BGColor( 0x000080);

	FTImpl.ColorRGB(0xff,0xff,0xff);

	FTImpl.Cmd_Toggle( 40, 10, 30, 27, FT_OPT_FLAT, 0.5*65535,"no""\xff""yes");//circle radius will be extended on both the horizontal sides
	FTImpl.ColorA((255));
	FTImpl.Cmd_Text(40, 40, 26, 0, "Flat effect");//text info
	/* Draw toggle bar with 3d effect */
	FTImpl.Cmd_FGColor(0x00ff00);
	FTImpl.Cmd_BGColor( 0x800000);
	FTImpl.Cmd_Toggle( 140, 10, 30, 27, 0, 1*65535,"stop""\xff""run");
	FTImpl.ColorA((255));
	FTImpl.Cmd_Text(140, 40, 26, 0, "3D effect");//text info

	xOffset = 40;
	yOffset = 80;
	/* Draw horizontal toggle bars */	
	FTImpl.Cmd_BGColor( 0x800000);
	FTImpl.Cmd_FGColor(0x410105);
	FTImpl.Cmd_Toggle( xOffset, yOffset, 30, 27, 0, 0*65535,"-ve""\xff""+ve");
	FTImpl.Cmd_FGColor(0x0b0721);
	FTImpl.Cmd_BGColor( 0x000080);
	yOffset += yDist;
	FTImpl.Cmd_Toggle( xOffset, yOffset, 30, 27, 0, 0.25*65535,"zero""\xff""one");
	FTImpl.Cmd_BGColor( 0xffff00);
	FTImpl.Cmd_FGColor(0x575e1b);
	FTImpl.ColorRGB(0,0,0);
	yOffset += yDist;
	FTImpl.Cmd_Toggle( xOffset, yOffset, 30, 27, 0, 0.5*65535,"exit""\xff""init");
	FTImpl.Cmd_BGColor( 0x808080);
	FTImpl.Cmd_FGColor(0x333234);
	FTImpl.ColorRGB(0xff,0xff,0xff);
	yOffset += yDist;
	FTImpl.Cmd_Toggle( xOffset, yOffset, 30, 27, 0, 0.75*65535,"off""\xff""on");

	FTImpl.DLEnd();
	FTImpl.Finish();
    delay(3000); 

}
/* API to demonstrate spinner widget */
void SAMAPP_CoPro_Widget_Spinner()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of spinner function. Spinner func   */
	/* will wait untill stop command is sent from the mcu. Spinner has option*/
	/* for displaying points in circle fashion or in a line fashion.         */
	/*************************************************************************/
	

	
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(64,64,64);
	FTImpl.Clear(1,1,1);    
	FTImpl.ColorRGB(0xff, 0xff, 0xff);	
	FTImpl.Cmd_Text((FT_DispWidth/2), 20, 27, FT_OPT_CENTER, "Spinner circle");
	FTImpl.Cmd_Text((FT_DispWidth/2), 80, 27, FT_OPT_CENTER, "Please Wait ...");
	FTImpl.Cmd_Spinner( (FT_DispWidth/2),(FT_DispHeight/2),0,1);//style 0 and scale 0

	FTImpl.Finish();
    delay(3000); 
	/**************************** spinner with style 1 and scale 1 *****************************************************/

	
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(64,64,64);
	FTImpl.Clear(1,1,1);    
	FTImpl.ColorRGB(0xff, 0xff, 0xff);	
	FTImpl.Cmd_Text((FT_DispWidth/2), 20, 27, FT_OPT_CENTER, "Spinner line");
	FTImpl.Cmd_Text((FT_DispWidth/2), 80, 27, FT_OPT_CENTER, "Please Wait ...");
	FTImpl.ColorRGB(0x00,0x00,0x80);
	FTImpl.Cmd_Spinner( (FT_DispWidth/2),(FT_DispHeight/2),1,1);//style 1 and scale 1
	FTImpl.Finish();
    delay(3000); 


	
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(64,64,64);
	FTImpl.Clear(1,1,1);    
	FTImpl.ColorRGB(0xff, 0xff, 0xff);	
	FTImpl.Cmd_Text((FT_DispWidth/2), 20, 27, FT_OPT_CENTER, "Spinner clockhand");
	FTImpl.Cmd_Text((FT_DispWidth/2), 80, 27, FT_OPT_CENTER, "Please Wait ...");
	FTImpl.ColorRGB(0x80,0x00,0x00);
	FTImpl.Cmd_Spinner( (FT_DispWidth/2),((FT_DispHeight/2) + 20),2,1);//style 2 scale 1
	
	FTImpl.Finish();
    delay(3000); 
	
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(64,64,64);
	FTImpl.Clear(1,1,1);    
	FTImpl.ColorRGB(0xff, 0xff, 0xff);	
	FTImpl.Cmd_Text((FT_DispWidth/2), 20, 27, FT_OPT_CENTER, "Spinner two dots");
	FTImpl.Cmd_Text((FT_DispWidth/2), 80, 27, FT_OPT_CENTER, "Please Wait ...");
	FTImpl.ColorRGB(0x80,0x00,0x00);
	FTImpl.Cmd_Spinner( (FT_DispWidth/2),((FT_DispHeight/2) + 20),3,1);//style 3 scale 0

	FTImpl.Finish();
    delay(3000); 

	/* Send the stop command */
	FTImpl.WriteCmd( CMD_STOP);
	/* Update the command buffer pointers - both read and write pointers */
	FTImpl.Finish();
	delay(1000);

}

/* API to demonstrate the usage of inflate command - compression done via zlib */
void SAMAPP_CoPro_Inflate()
{
	const SAMAPP_Bitmap_header_t *pBitmapHdr = NULL;
	uint8_t *pbuff = NULL;
	int16_t xoffset,yoffset;
	/*************************************************************************/
	/* Below code demonstrates the usage of inflate functiona                */
	/* Download the deflated data into command buffer and in turn coprocessor infaltes*/
	/* the deflated data and outputs at 0 location                           */
	/*************************************************************************/
	{
#ifdef MSVC_PLATFORM  
	FILE *pFile = fopen("..\\..\\..\\Test\\lenaface40.bin","rb");
#endif
	int32_t FileLen = 0;
	pBitmapHdr = &SAMAPP_Bitmap_RawData_Header[0];

	xoffset = ((FT_DispWidth - SAMAPP_Bitmap_RawData_Header[0].Width)/2);
	yoffset = ((FT_DispHeight - SAMAPP_Bitmap_RawData_Header[0].Height)/2);

	/* Clear the memory at location 0 - any previous bitmap data */
	
	FTImpl.WriteCmd( CMD_MEMSET);
	FTImpl.WriteCmd( 0L);//starting address of memset
	FTImpl.WriteCmd( 255L);//value of memset
	FTImpl.WriteCmd( 1L*pBitmapHdr->Stride*pBitmapHdr->Height);//number of elements to be changed

	/* Set the display list for graphics processor */
	/* Bitmap construction by MCU - display lena at 200x90 offset */
	/* Transfer the data into coprocessor memory directly word by word */	
	FTImpl.WriteCmd( CMD_DLSTART);
	FTImpl.WriteCmd( CLEAR_COLOR_RGB(0,0,255));	
	FTImpl.WriteCmd( CLEAR(1,1,1));
	FTImpl.WriteCmd( COLOR_RGB(255,255,255));
	FTImpl.WriteCmd( BEGIN(FT_BITMAPS));
	FTImpl.WriteCmd( BITMAP_SOURCE(0));
	FTImpl.WriteCmd( BITMAP_LAYOUT(SAMAPP_Bitmap_RawData_Header[0].Format,
		SAMAPP_Bitmap_RawData_Header[0].Stride,SAMAPP_Bitmap_RawData_Header[0].Height));
	FTImpl.WriteCmd( BITMAP_SIZE(FT_BILINEAR,FT_BORDER,FT_BORDER,
		SAMAPP_Bitmap_RawData_Header[0].Width,SAMAPP_Bitmap_RawData_Header[0].Height));
	FTImpl.WriteCmd( VERTEX2F(xoffset*16,yoffset*16));
	FTImpl.WriteCmd( END());

	/*  Display the text information */
	FTImpl.WriteCmd(COLOR_A(255));
	xoffset -= 50;
	yoffset += 40;
	FTImpl.Cmd_Text(xoffset, yoffset, 26, 0, "Display bitmap by inflate");
	FTImpl.WriteCmd(DISPLAY());

	FTImpl.Cmd_Swap();
	
	/* Wait till coprocessor completes the operation */
	FTImpl.Finish();
	

#ifdef MSVC_PLATFORM

	/* inflate the data read from binary file */
	if(NULL == pFile)
	{
		printf("Error in opening file %s \n","lenaface40.bin");
	}
	else
	{
#endif
		/* Inflate the data into location 0 */
		FTImpl.WriteCmd(  CMD_INFLATE);
		FTImpl.WriteCmd(  0);//destination address if inflate
#ifdef MSVC_PLATFORM
		fseek(pFile,0,SEEK_END);
		FileLen = ftell(pFile);
		fseek(pFile,0,SEEK_SET);
		pbuff = (uint8_t *)malloc(8192);
		while(FileLen > 0)
		{
			/* download the data into the command buffer by 2kb one shot */
			uint16_t blocklen = FileLen>8192?8192:FileLen;

			/* copy the data into pbuff and then transfter it to command buffer */			
			fread(pbuff,1,blocklen,pFile);
			FileLen -= blocklen;
			/* copy data continuously into command memory */
			Ft_Gpu_Hal_WrCmdBuf( pbuff, blocklen);//alignment is already taken care by this api
		}
		/* close the opened binary zlib file */
		fclose(pFile);
		free(pbuff);
	}
#endif
#ifdef ARDUINO_PLATFORM
      /* Copy the deflated/jpeg encoded data into coprocessor fifo */
      FTImpl.WriteCmdfromflash(Lenaface40,1L*SAMAPP_Lenaface40_SIZE);   
#endif
	}
    delay(2000);
}
/* API to demonstrate jpeg decode functionality */
void SAMAPP_CoPro_Loadimage()
{
	
	uint8_t *pbuff;
	const SAMAPP_Bitmap_header_t *pBitmapHdr = NULL;
	int16_t ImgW,ImgH,xoffset,yoffset;
	/*************************************************************************/
	/* Below code demonstrates the usage of loadimage function               */
	/* Download the jpg data into command buffer and in turn coprocessor decodes      */
	/* and dumps into location 0 with rgb565 format                          */
	/*************************************************************************/
	{
#ifdef MSVC_PLATFORM 
	FILE *pFile = fopen("..\\..\\..\\Test\\mandrill256.jpg","rb");
#endif
	int32_t FileLen = 0;
	pBitmapHdr = &SAMAPP_Bitmap_RawData_Header[0];
	ImgW = ImgH = 256;

	xoffset = ((FT_DispWidth - ImgW)/2);
	yoffset = ((FT_DispHeight - ImgH)/2);
	/* Clear the memory at location 0 - any previous bitmap data */
	
	FTImpl.WriteCmd( CMD_MEMSET);
	FTImpl.WriteCmd( 0L);//starting address of memset
	FTImpl.WriteCmd( 255L);//value of memset
	FTImpl.WriteCmd( 256L*2*256);//number of elements to be changed
	
	/* Set the display list for graphics processor */

	/* Bitmap construction by MCU - display lena at 112x8 offset */
	/* Transfer the data into coprocessor memory directly word by word */	
	FTImpl.WriteCmd( CMD_DLSTART);
	FTImpl.WriteCmd( CLEAR_COLOR_RGB(0,255,255));
	FTImpl.WriteCmd( CLEAR(1,1,1));
	FTImpl.WriteCmd( COLOR_RGB(255,255,255));
	FTImpl.WriteCmd( BEGIN(FT_BITMAPS));
	FTImpl.WriteCmd( BITMAP_SOURCE(0));
	FTImpl.WriteCmd( BITMAP_LAYOUT(FT_RGB565,ImgW*2,ImgH));
	FTImpl.WriteCmd( BITMAP_SIZE(FT_BILINEAR,FT_BORDER,FT_BORDER,ImgW,ImgH));
	FTImpl.WriteCmd( VERTEX2F(xoffset*16,yoffset*16));
	FTImpl.WriteCmd( END());

	/*  Display the text information */
	xoffset = ((FT_DispWidth)/2);
	yoffset = ((FT_DispHeight)/2);
	FTImpl.Cmd_Text(xoffset, yoffset, 26, FT_OPT_CENTER, "Display bitmap by jpg decode");
	FTImpl.WriteCmd(DISPLAY());
	FTImpl.Cmd_Swap();

	/* Wait till coprocessor completes the operation */
	coprostatus = FTImpl.Finish();
	Serial.println(coprostatus,DEC);
	
#ifdef MSVC_PLATFORM

	/* decode the jpeg data */
	if(NULL == pFile)
	{  
		printf("Error in opening file %s \n","mandrill256.jpg");
	}
	else
	{			
#endif
  
		/******************* Decode jpg output into location 0 and output color format as RGB565 *********************/
		FTImpl.WriteCmd(  CMD_LOADIMAGE);
		FTImpl.WriteCmd(  0);//destination address of jpg decode
		FTImpl.WriteCmd(  0);//output format of the bitmap
#ifdef MSVC_PLATFORM
		fseek(pFile,0,SEEK_END);
		FileLen = ftell(pFile);
		fseek(pFile,0,SEEK_SET);
		pbuff = (uint8_t *)malloc(8192);

		while(FileLen > 0)
		{
			/* download the data into the command buffer by 2kb one shot */
			uint16_t blocklen = FileLen>8192?8192:FileLen;

			/* copy the data into pbuff and then transfter it to command buffer */			
			fread(pbuff,1,blocklen,pFile);
			FileLen -= blocklen;
			/* copy data continuously into command memory */
			Ft_Gpu_Hal_WrCmdBuf(pbuff, blocklen);//alignment is already taken care by this api
		}
#endif
#ifdef ARDUINO_PLATFORM
        /* Copy the deflated/jpeg encoded data into coprocessor fifo */
         FTImpl.WriteCmdfromflash(Mandrill256,SAMAPP_Mandrill256_SIZE);     
				
#endif		
		FTImpl.Finish();
        delay(1000);
                /******************** Decode jpg output into location 0 & output as MONOCHROME ******************************/
		/* Clear the memory at location 0 - any previous bitmap data */\
		xoffset = ((FT_DispWidth - ImgW)/2);
		yoffset = ((FT_DispHeight - ImgH)/2);

		
		FTImpl.WriteCmd( CMD_MEMSET);
		FTImpl.WriteCmd( 0L);//starting address of memset
		FTImpl.WriteCmd( 255L);//value of memset
		FTImpl.WriteCmd( 256L*2*256);//number of elements to be changed

		/* Set the display list for graphics processor */
		/* Bitmap construction by MCU - display lena at 112x8 offset */
		/* Transfer the data into coprocessor memory directly word by word */	
		FTImpl.WriteCmd( CMD_DLSTART);
		FTImpl.WriteCmd( CLEAR_COLOR_RGB(0,0,0));	
		FTImpl.WriteCmd( CLEAR(1,1,1));
		FTImpl.WriteCmd( COLOR_RGB(255,255,255));
		FTImpl.WriteCmd( BEGIN(FT_BITMAPS));
		FTImpl.WriteCmd( BITMAP_SOURCE(0));
		FTImpl.WriteCmd( BITMAP_LAYOUT(FT_L8,ImgW,ImgH));//monochrome
		FTImpl.WriteCmd( BITMAP_SIZE(FT_BILINEAR,FT_BORDER,FT_BORDER,ImgW,ImgH));
		FTImpl.WriteCmd( VERTEX2F(xoffset*16,yoffset*16));
		FTImpl.WriteCmd( END());

		/*  Display the text information */
		xoffset = ((FT_DispWidth)/2);
		yoffset = ((FT_DispHeight)/2);
		FTImpl.Cmd_Text(xoffset, yoffset, 26, FT_OPT_CENTER, "Display bitmap by jpg decode L8");
		FTImpl.WriteCmd(DISPLAY());
		FTImpl.Cmd_Swap();

		
		/* Wait till coprocessor completes the operation */
		FTImpl.Finish();
        delay(1000);
		
		FTImpl.WriteCmd(  CMD_LOADIMAGE);
		FTImpl.WriteCmd(  0);//destination address of jpg decode
		FTImpl.WriteCmd(  FT_OPT_MONO);//output format of the bitmap - default is rgb565
#ifdef MSVC_PLATFORM
		fseek(pFile,0,SEEK_END);
		FileLen = ftell(pFile);
		fseek(pFile,0,SEEK_SET);
		while(FileLen > 0)
		{
			/* download the data into the command buffer by 2kb one shot */
			uint16_t blocklen = FileLen>8192?8192:FileLen;

			/* copy the data into pbuff and then transfter it to command buffer */			
			fread(pbuff,1,blocklen,pFile);
			FileLen -= blocklen;
			/* copy data continuously into command memory */
			Ft_Gpu_Hal_WrCmdBuf(pbuff, blocklen);//alignment is already taken care by this api
		}
		free(pbuff);

		/* close the opened jpg file */
		fclose(pFile);
        }
#endif
#ifdef ARDUINO_PLATFORM
      /* Copy the deflated/jpeg encoded data into coprocessor fifo */
      FTImpl.WriteCmdfromflash(Mandrill256,SAMAPP_Mandrill256_SIZE);     
#endif
	
	}
     delay(1000);	
}

/* API to demonstrate custom font display */
void SAMAPP_CoPro_Setfont()
{
	uint8_t *pbuff;
	uint8_t FontIdxTable[148];
	/*************************************************************************/
	/* Below code demonstrates the usage of setfont. Setfont function draws  */
	/* custom configured fonts on screen. Download the font table and raw    */
	/* font data in L1/FT_L4/L8 format and disply text                          */
	/*************************************************************************/
#ifdef MSVC_PLATFORM
{
	uint32_t fontaddr = (128+5*4);
	FILE *pFile = fopen("..\\..\\..\\Test\\font16.jpg","rb");
	int32_t FileLen = 0;

	memset(FontIdxTable,16,128);
	fontaddr = 0x00000003;//l8 format
	memcpy(&FontIdxTable[128],&fontaddr,4);
	fontaddr = 16;//stride
	memcpy(&FontIdxTable[128+4],&fontaddr,4);
	fontaddr = 16;//max width
	memcpy(&FontIdxTable[128+8],&fontaddr,4);
	fontaddr = 16;//max height
	memcpy(&FontIdxTable[128+12],&fontaddr,4);
	fontaddr = (1024);//data address - starts at location 1024
	memcpy(&FontIdxTable[128+16],&fontaddr,4);

	FTImpl.Writefromflash( FT_RAM_G,	FontIdxTable,(128+5*4));

	/* download the jpeg image and decode */
	/* Characters from 32 to 128 are present and each character is 16*16 dimention */
	if(NULL == pFile)
	{
		printf("Error in opening file %s \n","font16.jpg");
	}
	else
	{			
		/******************* Decode jpg output into location 0 and output color format as RGB565 *********************/
		Ft_Gpu_Hal_WrCmd32(  CMD_LOADIMAGE);
		Ft_Gpu_Hal_WrCmd32(  (9216));//destination address of jpg decode
		Ft_Gpu_Hal_WrCmd32(  OPT_MONO);//output format of the bitmap
		fseek(pFile,0,SEEK_END);
		FileLen = ftell(pFile);
		fseek(pFile,0,SEEK_SET);
		pbuff = malloc(8192);
		while(FileLen > 0)
		{
			/* download the data into the command buffer by 2kb one shot */
			uint16_t blocklen = FileLen>8192?8192:FileLen;

			/* copy the data into pbuff and then transfter it to command buffer */			
			fread(pbuff,1,blocklen,pFile);
			FileLen -= blocklen;
			/* copy data continuously into command memory */
			Ft_Gpu_Hal_WrCmdBuf(pbuff, blocklen);//alignment is already taken care by this api
		}
	}

	
	FTImpl.DLStart();
	FTImpl.WriteCmd(CLEAR_COLOR_RGB(0xff,0xff,0xff));//set the background to white
	FTImpl.WriteCmd(CLEAR(1,1,1));
	FTImpl.WriteCmd(COLOR_RGB(32,32,32));//black color text

	FTImpl.Cmd_Text((FT_DispWidth/2), 20, 27, FT_OPT_CENTER, "SetFont - format L8");
	FTImpl.WriteCmd(BITMAP_HANDLE(7));
	FTImpl.WriteCmd(BITMAP_SOURCE(1024));
	FTImpl.WriteCmd(BITMAP_LAYOUT(L8,16,16));
	FTImpl.WriteCmd(BITMAP_SIZE(FT_NEAREST,FT_BORDER,FT_BORDER,16,16));

	FTImpl.Cmd_SetFont( 7,0);	
	
	FTImpl.Cmd_Text((FT_DispWidth/2), 80, 7, FT_OPT_CENTER, "The quick brown fox jumps");
	FTImpl.Cmd_Text((FT_DispWidth/2), 120, 7, FT_OPT_CENTER, "over the lazy dog.");
	FTImpl.Cmd_Text((FT_DispWidth/2), 160,7, FT_OPT_CENTER, "1234567890");	
	
	FTImpl.WriteCmd(DISPLAY());
	FTImpl.Cmd_Swap();

	
	/* Wait till coprocessor completes the operation */
	FTImpl.Finish();

	delay(1000);//timeout for snapshot to be performed by coprocessor

	fclose(pFile);
	free(pbuff);

	}
#endif
	/* Display custom font by reading from the binary file - header of 148 bytes is at the starting followed by font data of 96 characters */
	/*Roboto-BoldCondensed.ttf*/
	{
	uint32_t fontaddr = (128+5*4);//header size
	int32_t FileLen = 0,i;
	uint16_t blocklen;
#ifdef MSVC_PLATFORM
	FILE *pFile = fopen("..\\..\\..\\Test\\Roboto-BoldCondensed_12.bin","rb");

	//first 148 bytes in the file is the header and later is the raw data for ascii 32 to 128 index charaters
	if(NULL == pFile)
	{
  
		printf("Error in opening file %s \n","Roboto-BoldCondensed_12.bin");
	}
	else
	{	
#endif
		fontaddr = FT_RAM_G;
		blocklen = 128+5*4;//header size

#ifdef MSVC_PLATFORM
		fseek(pFile,0,SEEK_END);
		FileLen = ftell(pFile);
		fseek(pFile,0,SEEK_SET);

		pbuff = malloc(8192);
		/* First download the header into location 0 */
		fread(pbuff,1,blocklen,pFile);
#endif

#ifdef ARDUINO_PLATFORM
                pbuff = FontIdxTable;
                /* Copy data from starting of the array into buffer */
                //hal_memcpy((char*)pbuff,(char*)Roboto_BoldCondensed_12,1L*blocklen);
                memcpy_P((char*)pbuff,(char*)Roboto_BoldCondensed_12,1L*blocklen);
#endif
		{
			uint32_t *ptemp = (uint32_t *)&pbuff[128+4*4],i;
			*ptemp = 1024;//download the font data at location 1024+32*8*25
			//memset(pbuff,16,32);
                        for(i=0;i<32;i++)
                        {
                          pbuff[i] = 16;
                        }
		}
		/* Modify the font data location */
		FTImpl.Write(fontaddr,(uint8_t *)pbuff,blocklen);
		
		/* Next download the data at location 32*8*25 - skip the first 32 characters */
		/* each character is 8x25 bytes */
		fontaddr += (1024+32*8*25);//make sure space is left at the starting of the buffer for first 32 characters - TBD manager this buffer so that this buffer can be utilized by other module
#ifdef MSVC_PLATFORM
		FileLen -= blocklen;
		while(FileLen > 0)
		{
			/* download the data into the command buffer by 8kb one shot */
			blocklen = FileLen>8192?8192:FileLen;

			/* copy the data into pbuff and then transfter it to command buffer */			
			fread(pbuff,1,blocklen,pFile);
			/* copy data continuously into command memory */
			FTImpl.Writefromflash( fontaddr,(uint8_t *)pbuff,blocklen);
			FileLen -= blocklen;
			fontaddr += blocklen;
		}
		free(pbuff);
		fclose(pFile);
          }
#endif
#ifdef ARDUINO_PLATFORM
        FTImpl.Writefromflash( fontaddr,&Roboto_BoldCondensed_12[blocklen],1L*SAMAPP_Roboto_BoldCondensed_12_SIZE);
#endif
	
	FTImpl.DLStart();
	FTImpl.WriteCmd(CLEAR_COLOR_RGB(0xff,0xff,0xff));//set the background to white
	FTImpl.WriteCmd(CLEAR(1,1,1));
	FTImpl.WriteCmd(COLOR_RGB(32,32,32));//black color text

	FTImpl.Cmd_Text((FT_DispWidth/2), 20, 27, FT_OPT_CENTER, "SetFont - format FT_L4");
	FTImpl.WriteCmd(BITMAP_HANDLE(6));//give index table 6
	FTImpl.WriteCmd(BITMAP_SOURCE(1024));//make the address to 0
	FTImpl.WriteCmd(BITMAP_LAYOUT(FT_L4,8,25));//stride is 8 and height is 25
	FTImpl.WriteCmd(BITMAP_SIZE(FT_NEAREST,FT_BORDER,FT_BORDER,16,25));//width is 16 and height is 25

	FTImpl.Cmd_SetFont( 6,0);	
	FTImpl.Cmd_Text((FT_DispWidth/2), 80, 6, FT_OPT_CENTER, "The quick brown fox jumps");
	FTImpl.Cmd_Text((FT_DispWidth/2), 120, 6, FT_OPT_CENTER, "over the lazy dog.");
	FTImpl.Cmd_Text((FT_DispWidth/2), 160, 6, FT_OPT_CENTER, "1234567890");
	
	FTImpl.WriteCmd(DISPLAY());
	FTImpl.Cmd_Swap();
	
	
	/* Wait till coprocessor completes the operation */
	FTImpl.Finish();

	delay(1000);//timeout for snapshot to be performed by coprocessor
	}
}


void SAMAPP_CoPro_Widget_Text()
{

/*************************************************************************/
/* Below code demonstrates the usage of text function. Text function     */
/* draws text with either in built or externally configured text. Text   */
/* color can be changed by colorrgb and text function supports display of*/
/* texts on left, right, top, bottom and center respectively             */
/*************************************************************************/
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(64,64,64);
	FTImpl.Clear(1,1,1);    
	FTImpl.ColorRGB(0x00,0x00,0x80);	
	/* Draw text at 0,0 location */	
	FTImpl.Cmd_Text(0, 0, 29, 0, "FTDI!");
	FTImpl.ColorRGB(0xff,0xff,0xff);
	FTImpl.Cmd_Text(0, 40, 26, 0, "Text29 at 0,0");//text info
	/* text with centerx */
	FTImpl.ColorRGB(0x80,0x00,0x00);
	FTImpl.Cmd_Text((FT_DispWidth/2), 50, 29, FT_OPT_CENTERX, "FTDI!");
	FTImpl.ColorRGB(0xff,0xff,0xff);
	FTImpl.Cmd_Text(((FT_DispWidth/2) - 30), 90, 26, 0, "Text29 centerX");//text info
	/* text with centery */
	FTImpl.ColorRGB(0x41,0x01,0x05);
	FTImpl.Cmd_Text((FT_DispWidth/5), 120, 29, FT_OPT_CENTERY, "FTDI!");
	FTImpl.ColorRGB(0xff,0xff,0xff);
	FTImpl.Cmd_Text((FT_DispWidth/5), 140, 26, 0, "Text29 centerY");//text info
	/* text with center */
	FTImpl.ColorRGB(0x0b,0x07,0x21);
	FTImpl.Cmd_Text((FT_DispWidth/2), 180, 29, FT_OPT_CENTER, "FTDI!");
	FTImpl.ColorRGB(0xff,0xff,0xff);
	FTImpl.Cmd_Text(((FT_DispWidth/2) - 50), 200, 26, 0, "Text29 center");//text info
	/* text with rightx */
	FTImpl.ColorRGB(0x57,0x5e,0x1b);
	FTImpl.Cmd_Text(FT_DispWidth, 60, 29, FT_OPT_RIGHTX, "FTDI!");
	FTImpl.ColorRGB(0xff,0xff,0xff);
	FTImpl.Cmd_Text((FT_DispWidth - 90), 100, 26, 0, "Text29 rightX");//text info
	/* Download the commands into fifo */
	FTImpl.DLEnd();
	FTImpl.Finish();
	delay(1000);	
}

void SAMAPP_CoPro_Widget_Number()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of number function. Number function */
	/* draws text with only 32bit decimal number, signed or unsigned can also*/
	/* be specified as input parameter. Options like centerx, centery, center*/
	/* and rightx are supported                                              */
	/*************************************************************************/
	{

	
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(64,64,64);
	FTImpl.Clear(1,1,1);    
	FTImpl.ColorRGB(0x00,0x00,0x80);	
	
	FTImpl.Cmd_Number(0, 0, 29, 0, 1234);
	FTImpl.ColorRGB(0xff,0xff,0xff);
	FTImpl.Cmd_Text(0, 40, 26, 0, "Number29 at 0,0");//text info
	/* number with centerx */
	FTImpl.ColorRGB(0x80,0x00,0x00);
	FTImpl.Cmd_Number((FT_DispWidth/2), 50, 29, FT_OPT_CENTERX | FT_OPT_SIGNED, -1234);
	FTImpl.ColorRGB(0xff,0xff,0xff);
	FTImpl.Cmd_Text(((FT_DispWidth/2) - 30), 90, 26, 0, "Number29 centerX");//text info
	/* number with centery */
	FTImpl.ColorRGB(0x41,0x01,0x05);
	FTImpl.Cmd_Number((FT_DispWidth/5), 120, 29, FT_OPT_CENTERY, 1234);
	FTImpl.ColorRGB(0xff,0xff,0xff);
	FTImpl.Cmd_Text((FT_DispWidth/5), 140, 26, 0, "Number29 centerY");//text info
	/* number with center */
	FTImpl.ColorRGB(0x0b,0x07,0x21);
	FTImpl.Cmd_Number((FT_DispWidth/2), 180, 29, FT_OPT_CENTER | FT_OPT_SIGNED, -1234);
	FTImpl.ColorRGB(0xff,0xff,0xff);
	FTImpl.Cmd_Text(((FT_DispWidth/2) - 50), 200, 26, 0, "Number29 center");//text info
	/* number with rightx */
	FTImpl.ColorRGB(0x57,0x5e,0x1b);
	FTImpl.Cmd_Number(FT_DispWidth, 60, 29, FT_OPT_RIGHTX, 1234);
	FTImpl.ColorRGB(0xff,0xff,0xff);
	FTImpl.Cmd_Text((FT_DispWidth - 100), 100, 26, 0, "Number29 rightX");//text info


	/* Download the commands into fifo */
		FTImpl.DLEnd();
		FTImpl.Finish();
		delay(1000);
	}
}


/* Main entry point */
/* API to demonstrate button functionality */
void SAMAPP_CoPro_Widget_Button()
{

	/*************************************************************************/
	/* Below code demonstrates the usage of button function. Buttons can be  */
	/* constructed using flat or 3d effect. Button color can be changed by   */
	/* fgcolor command and text color is set by COLOR_RGB graphics command   */
	/*************************************************************************/
	{
	int16_t xOffset,yOffset,bWidth,bHeight,bDistx,bDisty;

	bWidth = 60;
	bHeight = 30;
	bDistx = bWidth + ((FT_DispWidth - 4 * bWidth)/5);
	bDisty = bHeight + 5;
	xOffset = 10;
	yOffset = (FT_DispHeight/2 - 2*bDisty);
	/************ Construct a buttons with "ONE/TWO/THREE" text and default background *********************/	
	/* Draw buttons 60x30 resolution at 10x40,10x75,10x110 */
	
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(64,64,64);
	FTImpl.Clear(1,1,1);    
	FTImpl.ColorRGB(0xff,0xff,0xff);		
	/* flat effect and default color background */
	FTImpl.Cmd_FGColor(0x0000ff);
	yOffset = (FT_DispHeight/2 - 2*bDisty);
	FTImpl.Cmd_Button(xOffset,yOffset,bWidth,bHeight,28,FT_OPT_FLAT,"ABC");
	yOffset += bDisty;
	FTImpl.Cmd_Button(xOffset,yOffset,bWidth,bHeight,28,FT_OPT_FLAT,"ABC");
	yOffset += bDisty;
	FTImpl.Cmd_Button(xOffset,yOffset,bWidth,bHeight,28,FT_OPT_FLAT,"ABC");
	FTImpl.Cmd_Text(xOffset, (yOffset + 40), 26, 0, "Flat effect");//text info
	/* 3D effect */
	xOffset += bDistx;
	yOffset = (FT_DispHeight/2 - 2*bDisty);
	FTImpl.Cmd_Button(xOffset,yOffset,bWidth,bHeight,28,0,"ABC");
	yOffset += bDisty;
	FTImpl.Cmd_Button(xOffset,yOffset,bWidth,bHeight,28,0,"ABC");
	yOffset += bDisty;
	FTImpl.Cmd_Button(xOffset,yOffset,bWidth,bHeight,28,0,"ABC");	
	FTImpl.Cmd_Text(xOffset, (yOffset + 40), 26, 0, "3D Effect");//text info
	/* 3d effect with background color */	
	FTImpl.Cmd_FGColor(0xffff00);	
	xOffset += bDistx;
	yOffset = (FT_DispHeight/2 - 2*bDisty);
	FTImpl.Cmd_Button(xOffset,yOffset,bWidth,bHeight,28,0,"ABC");
	yOffset += bDisty;
	FTImpl.Cmd_FGColor(0x00ffff);
	FTImpl.Cmd_Button(xOffset,yOffset,bWidth,bHeight,28,0,"ABC");
	yOffset += bDisty;
	FTImpl.Cmd_FGColor(0xff00ff);
	FTImpl.Cmd_Button(xOffset,yOffset,bWidth,bHeight,28,0,"ABC");
	FTImpl.Cmd_Text(xOffset, (yOffset + 40), 26, 0, "3D Color");//text info
	/* 3d effect with gradient color */
	FTImpl.Cmd_FGColor(0x101010);
	FTImpl.Cmd_GradColor(0xff0000);
	xOffset += bDistx;
	yOffset = (FT_DispHeight/2 - 2*bDisty);
	FTImpl.Cmd_Button(xOffset,yOffset,bWidth,bHeight,28,0,"ABC");
	yOffset += bDisty;
	FTImpl.Cmd_GradColor(0x00ff00);
	FTImpl.Cmd_Button(xOffset,yOffset,bWidth,bHeight,28,0,"ABC");
	yOffset += bDisty;
	FTImpl.Cmd_GradColor(0x0000ff);
	FTImpl.Cmd_Button(xOffset,yOffset,bWidth,bHeight,28,0,"ABC");
	FTImpl.Cmd_Text(xOffset, (yOffset + 40), 26, 0, "3D Gradient");//text info
	
	
	/* Download the commands into fifo */
	FTImpl.DLEnd();
	FTImpl.Finish();
	delay(1000);


	}
}


/* API to demonstrate the use of transfer commands */
void SAMAPP_CoPro_AppendCmds()
{
	uint32_t AppendCmds[16];
	int16_t xoffset,yoffset;
	/*************************************************************************/
	/* Below code demonstrates the usage of coprocessor append command - to append any*/ 
	/* mcu specific graphics commands to coprocessor generated graphics commands      */
	/*************************************************************************/
	
	/* Bitmap construction by MCU - display lena at 200x60 offset */
	/* Construct the bitmap data to be copied in the temp buffer then by using 
	   coprocessor append command copy it into graphics processor DL memory */	
	xoffset = ((FT_DispWidth - SAMAPP_Bitmap_RawData_Header[0].Width)/2);
	yoffset = ((FT_DispHeight/3) - SAMAPP_Bitmap_RawData_Header[0].Height/2);
	
	FTImpl.DLStart();
	AppendCmds[0] = CLEAR_COLOR_RGB(255,0,0);
	AppendCmds[1] = CLEAR(1,1,1);
	AppendCmds[2] = COLOR_RGB(255,255,255);
	AppendCmds[3] = BEGIN(FT_BITMAPS);
	AppendCmds[4] = BITMAP_SOURCE(0);
	AppendCmds[5] = BITMAP_LAYOUT(SAMAPP_Bitmap_RawData_Header[0].Format,
		SAMAPP_Bitmap_RawData_Header[0].Stride,SAMAPP_Bitmap_RawData_Header[0].Height);
	AppendCmds[6] = BITMAP_SIZE(FT_BILINEAR,FT_BORDER,FT_BORDER,
		SAMAPP_Bitmap_RawData_Header[0].Width,SAMAPP_Bitmap_RawData_Header[0].Height);
	AppendCmds[7] = VERTEX2F(xoffset * 16,yoffset * 16);
	AppendCmds[8] = END();

	
        /* Download the bitmap data*/
	FTImpl.Writefromflash(FT_RAM_G,(uint8_t *)&SAMAPP_Bitmap_RawData[SAMAPP_Bitmap_RawData_Header[0].Arrayoffset],
		SAMAPP_Bitmap_RawData_Header[0].Stride*SAMAPP_Bitmap_RawData_Header[0].Height);

	/* Download the DL data constructed by the MCU to location 40*40*2 in sram */	
	FTImpl.Write(FT_RAM_G + SAMAPP_Bitmap_RawData_Header[0].Stride*SAMAPP_Bitmap_RawData_Header[0].Height,(uint8_t *)AppendCmds,9*4);

	/* Call the append api for copying the above generated data into graphics processor 
	   DL commands are stored at location 40*40*2 offset from the starting of the sram */
	FTImpl.Cmd_Append(FT_RAM_G + SAMAPP_Bitmap_RawData_Header[0].Stride*SAMAPP_Bitmap_RawData_Header[0].Height, 9*4);
	/*  Display the text information */
	FTImpl.Cmd_FGColor(0xffff00);
	xoffset -=50;
	yoffset += 40;
	FTImpl.Cmd_Text(xoffset, yoffset, 26, 0, "Display bitmap by Append");
	/* Download the commands into fifo */
	FTImpl.DLEnd();
	FTImpl.Finish();
	delay(1000);
}

PROGMEM  prog_uchar SAMAPP_Snd_Array[5*58] = "Slce\0Sqrq\0Sinw\0Saww\0Triw\0Beep\0Alrm\0Warb\0Crsl\0Pp01\0Pp02\0Pp03\0Pp04\0Pp05\0Pp06\0Pp07\0Pp08\0Pp09\0Pp10\0Pp11\0Pp12\0Pp13\0Pp14\0Pp15\0Pp16\0DMF#\0DMF*\0DMF0\0DMF1\0DMF2\0DMF3\0DMF4\0DMF5\0DMF6\0DMF7\0DMF8\0DMF9\0Harp\0Xyph\0Tuba\0Glok\0Orgn\0Trmp\0Pian\0Chim\0MBox\0Bell\0Clck\0Swth\0Cowb\0Noth\0Hiht\0Kick\0Pop\0Clak\0Chak\0Mute\0uMut\0";
PROGMEM  prog_uchar SAMAPP_Snd_TagArray[58] = {
	0x63,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
	0x23,0x2a,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,
	0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,
	0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,
	0x60,0x61
      };

/* API to demonstrate the usage of sound engine of FT801 */
void SAMAPP_Sound()
{
	int32_t LoopFlag = 0,wbutton,hbutton,tagval,tagvalsnd = -1,numbtnrow,numbtncol,i,j,prevtag = -1;
	uint32_t freqtrack = 0,currfreq = 0,prevcurrfreq;
	PROGMEM prog_uchar *pString;
	PROGMEM prog_uchar *pTagArray;
	char StringArray[8];
	/*************************************************************************/
	/* Below code demonstrates the usage of sound function. All the supported*/
	/* sounds and respective pitches are put as part of keys/buttons, by     */
	/* choosing particular key/button the sound is played                    */
	/*************************************************************************/
	LoopFlag = 100;
	numbtnrow = 7 /*16*/;//number of rows to be created - note that mute and unmute are not played in this application
	numbtncol = 8 /*13*/;//number of colomns to be created
	wbutton = (FT_DispWidth - 40)/numbtncol;
	hbutton = FT_DispHeight/numbtnrow;

	/* set the volume to maximum */
	FTImpl.Write(REG_VOL_SOUND,0xFF);
	/* set the tracker to track the slider for frequency */	
	FTImpl.Cmd_Track( FT_DispWidth - 15, 20, 8, (FT_DispHeight - 40), 100);
	
	FTImpl.Finish();
	while(LoopFlag--)
	{
		tagval = FTImpl.Read( REG_TOUCH_TAG);
		freqtrack = FTImpl.Read32( REG_TRACKER);
		
		if(100 == (freqtrack & 0xff))
		{
			currfreq = (uint32_t)(freqtrack >> 16);
			currfreq = (uint32_t)(88*currfreq)/65536;
			if(currfreq > 108)
				currfreq = 108;
		}
		if((tagval > 0))
		{
			if( tagval <= 99)
			{
				tagvalsnd = tagval;
			}
			if(0x63 == tagvalsnd)
			{
				tagvalsnd = 0;
			}
			if((prevtag != tagval) || (prevcurrfreq != currfreq))
			{
				/* Play sound wrt pitch */				
				FTImpl.Write16( REG_SOUND,(((currfreq + 21) << 8) | tagvalsnd));
				FTImpl.Write( REG_PLAY,1);
			}
			if(0 == tagvalsnd)
				tagvalsnd = 99;
		}
		/* start a new display list for construction of screen */
		
		FTImpl.DLStart();
		FTImpl.ClearColorRGB(64,64,64);
		FTImpl.Clear(1,1,1);    
		/* line width for the rectangle */
		FTImpl.LineWidth(1*16);

		/* custom keys for sound input */
		pString = SAMAPP_Snd_Array;
		pTagArray = SAMAPP_Snd_TagArray;
		/* First draw all the rectangles followed by the font */
		/* yellow color for background color */
		FTImpl.ColorRGB(0x80,0x80,0x00);
		FTImpl.Begin(FT_RECTS);
		for(i=0;i<numbtnrow;i++)
		{
			for(j=0;j<numbtncol;j++)
			{
				FTImpl.Tag(pgm_read_byte(pTagArray));
				FTImpl.Vertex2ii((j*wbutton + 2),(hbutton*i + 2),0,0);
				FTImpl.Vertex2ii(((j*wbutton) + wbutton - 2),((hbutton*i) + hbutton - 2),0,0);
				pTagArray++;
			}
		}
		FTImpl.End();
		FTImpl.ColorRGB(0xff,0xff,0xff);
		/* draw the highlight rectangle and text info */
		pTagArray = SAMAPP_Snd_TagArray;
		pString = SAMAPP_Snd_Array;
		for(i=0;i<numbtnrow;i++)
		{
			for(j=0;j<numbtncol;j++)
			{
				FTImpl.Tag(pgm_read_byte(pTagArray));
				if(tagvalsnd == pgm_read_byte(pTagArray))
				{
					/* red color for highlight effect */
					FTImpl.ColorRGB(0x80,0x00,0x00);
					FTImpl.Begin(FT_RECTS);
					FTImpl.Tag(pgm_read_byte(pTagArray));
					FTImpl.Vertex2ii((j*wbutton + 2),(hbutton*i + 2),0,0);
					FTImpl.Vertex2ii(((j*wbutton) + wbutton - 2),((hbutton*i) + hbutton - 2),0,0);
					FTImpl.End();
					/* reset the color to make sure font doesnt get impacted */
					FTImpl.ColorRGB(0xff,0xff,0xff);
				}
				/* to make sure that highlight rectangle as well as font to take the same tag values */
	
#ifdef ARDUINO_PLATFORM	
                strcpy_P(StringArray,(const PROGMEM prog_char*)pString);	
#endif
#ifdef MSVC_PLATFORM			
                strcpy(StringArray,pString);	
#endif                                
                FTImpl.Cmd_Text((wbutton/2) + j*wbutton,(hbutton/2) + hbutton*i,26,FT_OPT_CENTER,StringArray);

                                pString += (strlen(StringArray) + 1);
				pTagArray++;
			}
		}

		/* Draw vertical slider bar for frequency control */
		StringArray[0] = '\0';
		strcat(StringArray,"Pt ");
		FTUtility_Dec2Ascii(StringArray,(int32_t)(currfreq + 21));
		FTImpl.TagMask(0);
		FTImpl.Cmd_Text(FT_DispWidth - 20,10,26,FT_OPT_CENTER,StringArray);
		FTImpl.TagMask(1);
		FTImpl.Tag(100);
		FTImpl.Cmd_Slider( FT_DispWidth - 15, 20, 8, (FT_DispHeight - 40), 0, currfreq, 88);

		/* Download the commands into fifo */
		FTImpl.DLEnd();
		FTImpl.Finish();
		//delay(1000);
	}

	FTImpl.Write16( REG_SOUND,0);
	FTImpl.Write( REG_PLAY,1);
}

/* API to demonstrate screen saver widget - playing of bitmap via macro0 */
void SAMAPP_CoPro_Screensaver()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of screensaver function. Screen     */
	/* saver modifies macro0 with the vertex2f command.                      */
	/* MCU can display any static display list at the background with the    */
	/* changing bitmap                                                       */
	/*************************************************************************/
	{
	/* Download the bitmap data */		
	FTImpl.Writefromflash( FT_RAM_G,(uint8_t *)&SAMAPP_Bitmap_RawData[SAMAPP_Bitmap_RawData_Header[0].Arrayoffset],
		SAMAPP_Bitmap_RawData_Header[0].Stride*SAMAPP_Bitmap_RawData_Header[0].Height);
	
	/* Send command screen saver */
	
	FTImpl.WriteCmd( CMD_SCREENSAVER);//screen saver command will continuously update the macro0 with vertex2f command
	/* Copy the display list */
	FTImpl.WriteCmd(CMD_DLSTART);
	FTImpl.WriteCmd(CLEAR_COLOR_RGB(0,0,0x80));
	FTImpl.WriteCmd(CLEAR(1,1,1));
	FTImpl.WriteCmd(COLOR_RGB(0xff,0xff,0xff));
	/* lena bitmap */
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Scale( 3*65536,3*65536);//scale the bitmap 3 times on both sides
	FTImpl.Cmd_SetMatrix( );
	FTImpl.WriteCmd(BEGIN(FT_BITMAPS));
	FTImpl.WriteCmd(BITMAP_SOURCE(0));
	FTImpl.WriteCmd(BITMAP_LAYOUT(SAMAPP_Bitmap_RawData_Header[0].Format,
		SAMAPP_Bitmap_RawData_Header[0].Stride,SAMAPP_Bitmap_RawData_Header[0].Height));
	FTImpl.WriteCmd(BITMAP_SIZE(FT_BILINEAR,FT_BORDER,FT_BORDER,
		SAMAPP_Bitmap_RawData_Header[0].Width*3,SAMAPP_Bitmap_RawData_Header[0].Height*3));
	FTImpl.WriteCmd(MACRO(0));
	FTImpl.WriteCmd(END());
	/* Display the text */
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_SetMatrix( );
	FTImpl.Cmd_Text((FT_DispWidth/2), (FT_DispHeight/2), 27, FT_OPT_CENTER, "Screen Saver ...");
	FTImpl.Cmd_Memset((FT_RAM_G + 3200), 0xff, (160L*2*120));
	FTImpl.WriteCmd(DISPLAY());
	FTImpl.Cmd_Swap();

	FTImpl.Finish();
	delay(3*1000);

	/* Send the stop command */
	FTImpl.WriteCmd(  CMD_STOP);
	/* Update the command buffer pointers - both read and write pointers */
	FTImpl.Finish();
	}
}

void SAMAPP_fadeout()
{
    int32_t i;
	
	for (i = 100; i >= 0; i -= 3) 
	{
		FTImpl.Write(REG_PWM_DUTY,i);

		delay(2);//sleep for 2 ms
	}
}

/* API to perform display fadein effect by changing the display PWM from 0 till 100 and finally 128 */
void SAMAPP_fadein()
{
	int32_t i;
	
	for (i = 0; i <=100 ; i += 3) 
	{
		FTImpl.Write(REG_PWM_DUTY,i);
		delay(2);//sleep for 2 ms
	}
	/* Finally make the PWM 100% */
	i = 128;
	FTImpl.Write(REG_PWM_DUTY,i);
}

/* Sample app to demonstrate snapshot widget/functionality */
void SAMAPP_CoPro_Snapshot()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of snapshot function. Snapshot func */
	/* captures the present screen and dumps into bitmap with color formats  */
	/* argb4.                                                                */
	/*************************************************************************/
	{
	uint16_t WriteByte = 0;

	/* fadeout before switching off the pclock */
	SAMAPP_fadeout();
	/* Switch off the lcd */
	{
		uint8_t n = 0;
		FTImpl.Write( REG_GPIO, 0x7f);
		delay(100);
	}
	/* Disable the pclock */
	FTImpl.Write( REG_PCLK,WriteByte);
	/* Configure the resolution to 160x120 dimention */
	WriteByte = 160;
	FTImpl.Write16( REG_HSIZE,WriteByte);
	WriteByte = 120;
	FTImpl.Write16( REG_VSIZE,WriteByte);

	/* Construct screen shot for snapshot */
	
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(0,0,0);
	FTImpl.Clear(1,1,1);    
	FTImpl.ColorRGB(28,20,99);
	/* captured snapshot */	
	FTImpl.Begin(FT_POINTS);
	FTImpl.ColorA(128);
	FTImpl.PointSize(20*16);
	FTImpl.Vertex2f(0*16,0*16);
	FTImpl.PointSize(25*16);
	FTImpl.Vertex2f(20*16,10*16);
	FTImpl.PointSize(30*16);
	FTImpl.Vertex2f(40*16,20*16);
	FTImpl.PointSize(35*16);
	FTImpl.Vertex2f(60*16,30*16);
	FTImpl.PointSize(40*16);
	FTImpl.Vertex2f(80*16,40*16);
	FTImpl.PointSize(45*16);
	FTImpl.Vertex2f(100*16,50*16);
	FTImpl.PointSize(50*16);
	FTImpl.Vertex2f(120*16,60*16);
	FTImpl.PointSize(55*16);
	FTImpl.Vertex2f(140*16,70*16);
	FTImpl.PointSize(60*16);
	FTImpl.Vertex2f(160*16,80*16);
	FTImpl.PointSize(65*16);
	FTImpl.Vertex2f(0*16,120*16);
	FTImpl.PointSize(70*16);
	FTImpl.Vertex2f(160*16,0*16);
	FTImpl.End();//display the bitmap at the center of the display
	FTImpl.ColorA(255);
	FTImpl.ColorRGB(32,32,32);
	FTImpl.Cmd_Text(80, 60, 26, FT_OPT_CENTER, "Points");

	/* Download the commands into fifo */
	FTImpl.DLEnd();
	FTImpl.Finish();
	delay(100);//timeout for snapshot to be performed by coprocessor

	/* Take snap shot of the current screen */
	FTImpl.WriteCmd(CMD_SNAPSHOT);	
	FTImpl.WriteCmd( 3200);//store the rgb content at location 3200

	//timeout for snapshot to be performed by coprocessor

	/* Wait till coprocessor completes the operation */
	FTImpl.Finish();

	delay(100);//timeout for snapshot to be performed by coprocessor

	/* reconfigure the resolution wrt configuration */
	WriteByte = FT_DispWidth;
	FTImpl.Write16( REG_HSIZE,WriteByte);
	WriteByte = FT_DispHeight;
	FTImpl.Write16( REG_VSIZE,WriteByte);

	
	
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(0,0,0);
	FTImpl.Clear(1,1,1);    
	FTImpl.ColorRGB(0xff,0xff,0xff);
	/* captured snapshot */	
	FTImpl.Begin(FT_BITMAPS);
	FTImpl.BitmapSource(3200);
	FTImpl.BitmapLayout(FT_ARGB4,160*2,120);
	FTImpl.BitmapSize(FT_BILINEAR,FT_BORDER,FT_BORDER,160,120);
	FTImpl.Vertex2f(((FT_DispWidth - 160)/2)*16,((FT_DispHeight - 120)/2)*16);
	FTImpl.End();//display the bitmap at the center of the display
	/* Display the text info */
	FTImpl.ColorRGB(32,32,32);
	FTImpl.Cmd_Text((FT_DispWidth/2), 40, 27, FT_OPT_CENTER, "Snap shot");

	FTImpl.DLEnd();
	FTImpl.Finish();
	
	/* reenable the pclock */
	WriteByte = FT_DispPCLK;
	FTImpl.Write( REG_PCLK,WriteByte);
	delay(60);
	/* Power on the LCD */
	{
		uint8_t n = 0x80;
		FTImpl.Write( REG_GPIO,0xff);		
	}

	delay(200);//give some time for the lcd to switchon - hack for one perticular panel

	/* set the display pwm back to 128 */
	{
		SAMAPP_fadein();
	}

     delay(1000);
	
	}
}


/* API to demonstrate sketch widget */
void SAMAPP_CoPro_Sketch()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of sketch function. Sketch function */
	/* draws line for pen movement. Skecth supports L1 and L8 output formats */
	/* Sketch draws till stop command is executed.                           */
	/*************************************************************************/
	{
	int16_t BorderSz = 40;
    uint32_t MemZeroSz;
	/* Send command sketch */
	
    MemZeroSz = 1L*(FT_DispWidth - 2*BorderSz)*(FT_DispHeight - 2*BorderSz);
	FTImpl.Cmd_Memzero(FT_RAM_G,MemZeroSz);        
	FTImpl.Cmd_Sketch( BorderSz,BorderSz,(FT_DispWidth - 2*BorderSz),(FT_DispHeight - 2*BorderSz),0,FT_L1);//sketch in L1 format
	/* Display the sketch */
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(0x80,0,0);
	FTImpl.Clear(1,1,1);    
	FTImpl.ColorRGB(0xff,0xff,0xff);
	FTImpl.ScissorSize((FT_DispWidth - 2*BorderSz),(FT_DispHeight - 2*BorderSz));
	FTImpl.ScissorXY(BorderSz,BorderSz);
	FTImpl.ClearColorRGB(0xff,0xff,0xff);
	FTImpl.Clear(1,1,1);

	FTImpl.ScissorSize(512,512);
	FTImpl.ScissorXY(0,0);
	FTImpl.ColorRGB(0,0,0);
	/* L1 bitmap display */
	FTImpl.Begin(FT_BITMAPS);
	FTImpl.BitmapSource(0);
	FTImpl.BitmapLayout(FT_L1,(FT_DispWidth - 2*BorderSz)/8,(FT_DispHeight - 2*BorderSz));
	FTImpl.BitmapSize(FT_BILINEAR,FT_BORDER,FT_BORDER,(FT_DispWidth - 2*BorderSz),(FT_DispHeight - 2*BorderSz));
	FTImpl.Vertex2f(BorderSz*16,BorderSz*16);
	FTImpl.End();
	/* Display the text */	
	FTImpl.ColorRGB(0xff,0xff,0xff);
	FTImpl.Cmd_Text((FT_DispWidth/2), 20, 27, FT_OPT_CENTER, "Sketch L1");
	FTImpl.DLEnd();
	FTImpl.Finish();
	delay(3*1000);//sleep for 10 seconds

	/* Send the stop command */
	FTImpl.WriteCmd(CMD_STOP);
	/* Update the command buffer pointers - both read and write pointers */
	FTImpl.Finish();	

	/* Sketch L8 format */
	
	/* Send command sketch */
	
	FTImpl.Cmd_Memzero(FT_RAM_G,MemZeroSz);
	FTImpl.Cmd_Sketch( BorderSz,BorderSz,(FT_DispWidth - 2*BorderSz),(FT_DispHeight - 2*BorderSz),0,FT_L8);//sketch in L8 format
	/* Display the sketch */
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(0x00,0,0x80);
	FTImpl.Clear(1,1,1);
	FTImpl.ColorRGB(0xff,0xff,0xff);
	FTImpl.ScissorSize((FT_DispWidth - 2*BorderSz),(FT_DispHeight - 2*BorderSz));
	FTImpl.ScissorXY(BorderSz,BorderSz);
	FTImpl.ClearColorRGB(0xff,0xff,0xff);
	FTImpl.Clear(1,1,1);

	FTImpl.ScissorSize(512,512);
	FTImpl.ScissorXY(0,0);
	FTImpl.ColorRGB(0,0,0);
	/* L8 bitmap display */
	FTImpl.Begin(FT_BITMAPS);
	FTImpl.BitmapSource(0);
	FTImpl.BitmapLayout(FT_L8,(FT_DispWidth - 2*BorderSz),(FT_DispHeight - 2*BorderSz));
	FTImpl.BitmapSize(FT_BILINEAR,FT_BORDER,FT_BORDER,(FT_DispWidth - 2*BorderSz),(FT_DispHeight - 2*BorderSz));
	FTImpl.Vertex2f(BorderSz*16,BorderSz*16);
	FTImpl.End();
	/* Display the text */	
	FTImpl.ColorRGB(0xff,0xff,0xff);
	FTImpl.Cmd_Text((FT_DispWidth/2), 20, 27, FT_OPT_CENTER, "Sketch L8");
	
	FTImpl.DLEnd();
	FTImpl.Finish();

	delay(3000);//sleep for 3 seconds

	/* Send the stop command */
	FTImpl.WriteCmd(  CMD_STOP);
	/* Update the command buffer pointers - both read and write pointers */
	FTImpl.Finish();
	}
}

/* API to demonstrate scale, rotate and translate functionality */
void SAMAPP_CoPro_Matrix()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of bitmap matrix processing apis.   */
	/* Mainly matrix apis consists if scale, rotate and translate.           */
	/* Units of translation and scale are interms of 1/65536, rotation is in */
	/* degrees and in terms of 1/65536. +ve theta is anticlock wise, and -ve  */
	/* theta is clock wise rotation                                          */
	/*************************************************************************/

	/* Lena image with 40x40 rgb565 is used as an example */	
	{
	int32_t imagewidth,imagestride,imageheight,imagexoffset,imageyoffset;

	/* Download the bitmap data */		
	FTImpl.Writefromflash(FT_RAM_G,(uint8_t *)&SAMAPP_Bitmap_RawData[SAMAPP_Bitmap_RawData_Header[0].Arrayoffset],
		SAMAPP_Bitmap_RawData_Header[0].Stride*SAMAPP_Bitmap_RawData_Header[0].Height);

	
	FTImpl.WriteCmd(CMD_DLSTART);
	FTImpl.WriteCmd(CLEAR_COLOR_RGB(0xff,0xff,0xff));
	FTImpl.WriteCmd(CLEAR(1,1,1));
	FTImpl.WriteCmd(COLOR_RGB(32,32,32));
	FTImpl.Cmd_Text(10, 5, 16, 0, "BM with rotation");
	FTImpl.Cmd_Text(10, 20 + 40 + 5, 16, 0, "BM with scaling");
	FTImpl.Cmd_Text(10, 20 + 40 + 20 + 80 + 5, 16, 0, "BM with flip");

	imagewidth = SAMAPP_Bitmap_RawData_Header[0].Width;
	imageheight = SAMAPP_Bitmap_RawData_Header[0].Height;
	imagestride = SAMAPP_Bitmap_RawData_Header[0].Stride;
	imagexoffset = 10*16;
	imageyoffset = 20*16;

	FTImpl.WriteCmd(COLOR_RGB(0xff,0xff,0xff));
	FTImpl.WriteCmd(BEGIN(FT_BITMAPS));
	FTImpl.WriteCmd(BITMAP_SOURCE(0));
	FTImpl.WriteCmd(BITMAP_LAYOUT(SAMAPP_Bitmap_RawData_Header[0].Format,imagestride,imageheight));
	FTImpl.WriteCmd(BITMAP_SIZE(FT_BILINEAR,FT_BORDER,FT_BORDER,imagewidth,imageheight));
	/******************************************* Perform display of plain bitmap ************************************/
	FTImpl.WriteCmd(VERTEX2F(imagexoffset,imageyoffset));
		
	/* Perform display of plain bitmap with 45 degrees anti clock wise and the rotation is performed on top left coordinate */
	imagexoffset += (imagewidth + 10)*16;
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Rotate( (-45*65536/360));//rotate by 45 degrees anticlock wise
	FTImpl.Cmd_SetMatrix( );
	FTImpl.WriteCmd(VERTEX2F(imagexoffset,imageyoffset));

	/* Perform display of plain bitmap with 30 degrees clock wise and the rotation is performed on top left coordinate */
	imagexoffset += (imagewidth*1.42 + 10)*16;//add the width*1.41 as diagonal is new width and extra 10 pixels
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Rotate( 30*65536/360);//rotate by 33 degrees clock wise
	FTImpl.Cmd_SetMatrix( );
	FTImpl.WriteCmd(VERTEX2F(imagexoffset,imageyoffset));

	/* Perform display of plain bitmap with 45 degrees anti clock wise and the rotation is performed wrt centre of the bitmap */
	imagexoffset += (imagewidth*1.42 + 10)*16;//add the width*1.41 as diagonal is new width and extra 10 pixels
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Translate( 65536*imagewidth/2,65536*imageheight/2);//make the rotation coordinates at the center
	FTImpl.Cmd_Rotate( -45*65536/360);//rotate by 45 degrees anticlock wise
	FTImpl.Cmd_Translate( -65536*imagewidth/2,-65536*imageheight/2);
	FTImpl.Cmd_SetMatrix( );
	FTImpl.WriteCmd(VERTEX2F(imagexoffset,imageyoffset));

	/* Perform display of plain bitmap with 45 degrees clock wise and the rotation is performed so that whole bitmap is viewable */
	imagexoffset += (imagewidth*1.42 + 10)*16;//add the width*1.41 as diagonal is new width and extra 10 pixels
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Translate( 65536*imagewidth/2,65536*imageheight/2);//make the rotation coordinates at the center
	FTImpl.Cmd_Rotate( 45*65536/360);//rotate by 45 degrees clock wise
	FTImpl.Cmd_Translate( -65536*imagewidth/10,-65536*imageheight/2);
	FTImpl.Cmd_SetMatrix( );
	FTImpl.WriteCmd(BITMAP_SIZE(FT_BILINEAR,FT_BORDER,FT_BORDER,imagewidth*2,imageheight*2));
	FTImpl.WriteCmd(VERTEX2F(imagexoffset,imageyoffset));

	/* Perform display of plain bitmap with 90 degrees anti clock wise and the rotation is performed so that whole bitmap is viewable */
	imagexoffset += (imagewidth*2 + 10)*16;
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Translate( 65536*imagewidth/2,65536*imageheight/2);//make the rotation coordinates at the center
	FTImpl.Cmd_Rotate( -90*65536/360);//rotate by 90 degrees anticlock wise
	FTImpl.Cmd_Translate( -65536*imagewidth/2,-65536*imageheight/2);
	FTImpl.Cmd_SetMatrix( );
	FTImpl.WriteCmd(BITMAP_SIZE(FT_BILINEAR,FT_BORDER,FT_BORDER,imagewidth,imageheight));
	FTImpl.WriteCmd(VERTEX2F(imagexoffset,imageyoffset));

	/* Perform display of plain bitmap with 180 degrees clock wise and the rotation is performed so that whole bitmap is viewable */
	imagexoffset += (imagewidth + 10)*16;
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Translate( 65536*imagewidth/2,65536*imageheight/2);//make the rotation coordinates at the center
	FTImpl.Cmd_Rotate( -180*65536/360);//rotate by 180 degrees anticlock wise
	FTImpl.Cmd_Translate( -65536*imagewidth/2,-65536*imageheight/2);
	FTImpl.Cmd_SetMatrix( );
	FTImpl.WriteCmd(VERTEX2F(imagexoffset,imageyoffset));
	/******************************************* Perform display of bitmap with scale ************************************/
	/* Perform display of plain bitmap with scale factor of 2x2 in x & y direction */
	imagexoffset = (10)*16;
	imageyoffset += (imageheight + 20)*16;
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Scale( 2*65536,2*65536);//scale by 2x2
	FTImpl.Cmd_SetMatrix( );
	FTImpl.WriteCmd(BITMAP_SIZE(FT_BILINEAR,FT_BORDER,FT_BORDER,imagewidth*2,imageheight*2));
	FTImpl.WriteCmd(VERTEX2F(imagexoffset,imageyoffset));

	/* Perform display of plain bitmap with scale factor of .5x.25 in x & y direction, rotate by 45 degrees clock wise wrt top left */
	imagexoffset += (imagewidth*2 + 10)*16;
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Translate( 65536*imagewidth/2,65536*imageheight/2);//make the rotation coordinates at the center
	
	FTImpl.Cmd_Rotate( 45*65536/360);//rotate by 45 degrees clock wise
	FTImpl.Cmd_Scale( 65536/2,65536/4);//scale by .5x.25
	FTImpl.Cmd_Translate( -65536*imagewidth/2,-65536*imageheight/2);
	FTImpl.Cmd_SetMatrix( );
	FTImpl.WriteCmd(VERTEX2F(imagexoffset,imageyoffset));

	/* Perform display of plain bitmap with scale factor of .5x2 in x & y direction, rotate by 75 degrees anticlock wise wrt center of the image */
	imagexoffset += (imagewidth + 10)*16;
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Translate( 65536*imagewidth/2,65536*imageheight/2);//make the rotation coordinates at the center	
	FTImpl.Cmd_Rotate( -75*65536/360);//rotate by 75 degrees anticlock wise
	FTImpl.Cmd_Scale( 65536/2,2*65536);//scale by .5x2
	FTImpl.Cmd_Translate( -65536*imagewidth/2,-65536*imageheight/8);
	FTImpl.Cmd_SetMatrix( );
	FTImpl.WriteCmd(BITMAP_SIZE(FT_BILINEAR,FT_BORDER,FT_BORDER,imagewidth*5/2,imageheight*5/2));
	FTImpl.WriteCmd(VERTEX2F(imagexoffset,imageyoffset));
	/******************************************* Perform display of bitmap flip ************************************/
	/* perform display of plain bitmap with 1x1 and flip right */
	imagexoffset = (10)*16;
	imageyoffset += (imageheight*2 + 20)*16;
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Translate( 65536*imagewidth/2,65536*imageheight/2);//make the rotation coordinates at the center
	FTImpl.Cmd_Scale( -1*65536,1*65536);//flip right
	FTImpl.Cmd_Translate( -65536*imagewidth/2,-65536*imageheight/2);
	FTImpl.Cmd_SetMatrix( );
	FTImpl.WriteCmd(BITMAP_SIZE(FT_BILINEAR,FT_BORDER,FT_BORDER,imagewidth,imageheight));
	FTImpl.WriteCmd(VERTEX2F(imagexoffset,imageyoffset));

	/* Perform display of plain bitmap with 2x2 scaling, flip bottom */
	imagexoffset += (imagewidth + 10)*16;
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Translate( 65536*imagewidth/2,65536*imageheight/2);//make the rotation coordinates at the center
	FTImpl.Cmd_Scale( 2*65536,-2*65536);//flip bottom and scale by 2 on both sides
	FTImpl.Cmd_Translate( -65536*imagewidth/4,-65536*imageheight/1.42);
	FTImpl.Cmd_SetMatrix( );
	FTImpl.WriteCmd(BITMAP_SIZE(FT_BILINEAR,FT_BORDER,FT_BORDER,imagewidth*4,imageheight*4));
	FTImpl.WriteCmd(VERTEX2F(imagexoffset,imageyoffset));

	/* Perform display of plain bitmap with 2x1 scaling, rotation and flip right and make sure whole image is viewable */
	imagexoffset += (imagewidth*2 + 10)*16;
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Translate( 65536*imagewidth/2,65536*imageheight/2);//make the rotation coordinates at the center
	
	FTImpl.Cmd_Rotate( -45*65536/360);//rotate by 45 degrees anticlock wise
	FTImpl.Cmd_Scale( -2*65536,1*65536);//flip right and scale by 2 on x axis
	FTImpl.Cmd_Translate( -65536*imagewidth/2,-65536*imageheight/8);
	FTImpl.Cmd_SetMatrix( );
	FTImpl.WriteCmd(BITMAP_SIZE(FT_BILINEAR,FT_BORDER,FT_BORDER,(imagewidth*5),(imageheight*5)));
	FTImpl.WriteCmd(VERTEX2F(imagexoffset,imageyoffset));

	FTImpl.WriteCmd(END());
	FTImpl.WriteCmd(DISPLAY());
	FTImpl.Cmd_Swap();

	FTImpl.Finish();
    delay(2000);

	}
}
/* Sample app api to demonstrate track widget funtionality */
void SAMAPP_CoPro_Track()
{
	uint8_t *pbuff;
	uint32_t NumBytesGen = 0;
	uint16_t CurrWriteOffset = 0;
	//SAMAPP_Bitmap_header_t *pBitmapHdr = NULL;

	/*************************************************************************/
	/* Below code demonstrates the usage of track function. Track function   */
	/* tracks the pen touch on any specific object. Track function supports  */
	/* rotary and horizontal/vertical tracks. Rotary is given by rotation    */
	/* angle and horizontal/vertucal track is offset position.               */
	/*************************************************************************/
	{
	int32_t LoopFlag = 0;
	uint32_t TrackRegisterVal = 0;
	uint16_t angleval = 0,slideval = 0,scrollval = 0;

	/* Set the tracker for 3 bojects */
	
	FTImpl.Cmd_Track( FT_DispWidth/2, FT_DispHeight/2, 1,1, 10);
	FTImpl.Cmd_Track( 40, (FT_DispHeight - 40), (FT_DispWidth - 80),8, 11);
	FTImpl.Cmd_Track( (FT_DispWidth - 40), 40, 8,(FT_DispHeight - 80), 12);
	/* Download the commands into fifo */
	FTImpl.Finish();

	LoopFlag = 600;
	/* update the background color continuously for the color change in any of the trackers */
	while(LoopFlag--)
	{
		uint8_t tagval = 0;
		TrackRegisterVal = FTImpl.Read32(REG_TRACKER);
		tagval = TrackRegisterVal & 0xff;
		if(0 != tagval)
		{
			if(10 == tagval)
			{
				angleval = TrackRegisterVal>>16;
			}
			else if(11 == tagval)
			{
				slideval = TrackRegisterVal>>16;
			}
			else if(12 == tagval)
			{
				scrollval = TrackRegisterVal>>16;				
				if((scrollval + 65535/10) > (9*65535/10))
				{
					scrollval = (8*65535/10);
				}
				else if(scrollval < (1*65535/10))
				{
					scrollval = 0;
				}
				else
				{
					scrollval -= (1*65535/10);
				}
			}
		}
		/* Display a rotary dial, horizontal slider and vertical scroll */
		FTImpl.DLStart();	
		{
                        int32_t tmpval0,tmpval1,tmpval2;
						uint8_t angval,sldval,scrlval;

                        tmpval0 = (int32_t)angleval*255/65536;
                        tmpval1 = (int32_t)slideval*255/65536;
                        tmpval2 = (int32_t)scrollval*255/65536;
                        
                        angval = tmpval0&0xff;
                        sldval = tmpval1&0xff;
                        scrlval = tmpval2&0xff;
			FTImpl.ClearColorRGB(angval,sldval,scrlval);
		}
		
		FTImpl.Clear(1,1,1);
		FTImpl.ColorRGB(0xff,0xff,0xff);

		/* Draw dial with 3d effect */
		FTImpl.Cmd_FGColor(0x00ff00);
		FTImpl.Cmd_BGColor(0x800000);
		FTImpl.Tag(10);
		FTImpl.Cmd_Dial((FT_DispWidth/2), (FT_DispHeight/2), (FT_DispWidth/8), 0, angleval);
		
		/* Draw slider with 3d effect */		
		FTImpl.Cmd_FGColor(0x00a000);
		FTImpl.Cmd_BGColor(0x800000);
		FTImpl.Tag(11);
		FTImpl.Cmd_Slider( 40, (FT_DispHeight - 40), (FT_DispWidth - 80),8, 0, slideval, 65535);

		/* Draw scroll with 3d effect */		
		FTImpl.Cmd_FGColor(0x00a000);
		FTImpl.Cmd_BGColor( 0x000080);
		FTImpl.Tag(12);
		FTImpl.Cmd_Scrollbar((FT_DispWidth - 40), 40, 8, (FT_DispHeight - 80), 0, scrollval, (65535*0.2), 65535);
		
		FTImpl.TagMask(0);
		FTImpl.ColorRGB(0xff,0xff,0xff);
		FTImpl.Cmd_Text((FT_DispWidth/2), ((FT_DispHeight/2) + (FT_DispWidth/8) + 8), 26, FT_OPT_CENTER, "Rotary track");
		FTImpl.Cmd_Text(((FT_DispWidth/2)), ((FT_DispHeight - 40) + 8 + 8), 26, FT_OPT_CENTER, "Horizontal track");
		FTImpl.Cmd_Text((FT_DispWidth - 40), 20, 26, FT_OPT_CENTER, "Vertical track");
		FTImpl.DLEnd();
		FTImpl.Finish();
		delay(10);//sleep for 3 seconds
	}

	/* Set the tracker for 3 bojects */
	
	FTImpl.Cmd_Track( 240, 136, 0,0, 10);
	FTImpl.Cmd_Track( 40, 232, 0,0, 11);
	FTImpl.Cmd_Track( 400, 40, 0,0, 12);

	/* Download the commands into fifo */
	FTImpl.Finish();
	}
}




void setup()
{
/* Initialize transport related functionality */
	Serial.begin(9600);
/* Set the Display Enable pin*/   
	Serial.println("--Start Test Application--");
	TestApp_Bootup();
/*Initalize the Display Properties*/
	FTImpl.Init(FT_DISPLAY_WQVGA_480x272);
/*Set the Display pin*/	
	FTImpl.SetDisplayEnablePin(FT_DISPENABLE_PIN);
        FTImpl.SetAudioEnablePin(FT_AUDIOENABLE_PIN); 
	FTImpl.DisplayOn();   		
        FTImpl.AudioOn();   	
}



void loop()
{
/* Test Condition for the coprocessor Apis*/     
 #ifdef SAMAPP_ENABLE_APIS_SET0
 /*   SAMAPP_API_Screen("Set0   START");
	SAMAPP_GPU_Points();
	SAMAPP_GPU_Lines();
	SAMAPP_GPU_Rectangles();
	SAMAPP_GPU_Bitmap();
	SAMAPP_GPU_Fonts();
	SAMAPP_GPU_Text8x8();
	SAMAPP_GPU_TextVGA();
	SAMAPP_GPU_Bargraph();
	SAMAPP_GPU_LineStrips();
	SAMAPP_GPU_EdgeStrips();
	SAMAPP_GPU_Scissor();
	SAMAPP_Gpu_Polygon();
	SAMAPP_Gpu_Cube();
	SAMAPP_GPU_Ball_Stencil();
	SAMAPP_GPU_FtdiString();*/
	SAMAPP_GPU_StreetMap();
	/*SAMAPP_GPU_AdditiveBlendText();
	SAMAPP_GPU_MacroUsage();
	SAMAPP_GPU_AdditiveBlendPoints();
    SAMAPP_API_Screen("Set0   End!");*/
#endif /* #ifdef SAMAPP_ENABLE_APIS_SET0 */

#ifdef SAMAPP_ENABLE_APIS_SET1
    SAMAPP_API_Screen("Set1   START");
    SAMAPP_CoPro_Logo();    
    SAMAPP_CoPro_Widget_Calibrate();
    SAMAPP_Touch();
    SAMAPP_CoPro_Widget_Clock();
    SAMAPP_CoPro_Widget_Guage();
    SAMAPP_CoPro_Widget_Gradient();
    SAMAPP_CoPro_Widget_Keys();
    SAMAPP_CoPro_Widget_Keys_Interactive();
    SAMAPP_CoPro_Widget_Progressbar();
    SAMAPP_CoPro_Widget_Scroll();
    SAMAPP_CoPro_Widget_Slider();
    SAMAPP_CoPro_Widget_Dial();
    SAMAPP_CoPro_Widget_Toggle();
    SAMAPP_CoPro_Widget_Spinner();
//    SAMAPP_PowerMode();    
    SAMAPP_API_Screen("Set1   END!");
#endif /* #ifdef SAMAPP_ENABLE_APIS_SET1 */	

#ifdef SAMAPP_ENABLE_APIS_SET2
    SAMAPP_API_Screen("Set2   START");
    SAMAPP_CoPro_Inflate();
    SAMAPP_CoPro_Loadimage();
    SAMAPP_API_Screen("Set2   END!");
#endif /* #ifdef SAMAPP_ENABLE_APIS_SET2 */

#ifdef SAMAPP_ENABLE_APIS_SET3
       SAMAPP_API_Screen("Set3   START");
       SAMAPP_CoPro_Setfont();
       SAMAPP_API_Screen("Set3   END!");
#endif /* #ifdef SAMAPP_ENABLE_APIS_SET3 */

#ifdef SAMAPP_ENABLE_APIS_SET4     
        SAMAPP_API_Screen("Set4   START");
	/* Sample code for coprocessor widgets */
	SAMAPP_CoPro_Widget_Text();
	SAMAPP_CoPro_Widget_Number();
	SAMAPP_CoPro_Widget_Button();
	SAMAPP_CoPro_AppendCmds();
    SAMAPP_CoPro_Widget_Calibrate();
	FTImpl.Write( REG_VOL_SOUND,255);
	SAMAPP_Sound();	
	SAMAPP_CoPro_Screensaver();
	SAMAPP_CoPro_Snapshot();
	SAMAPP_CoPro_Sketch();
	
	SAMAPP_CoPro_Matrix();
	SAMAPP_CoPro_Track();
	
    SAMAPP_API_Screen("Set4   END!");

#ifdef MSVC_PLATFORM
	//Enable Audio out by setting GPIO
	FTImpl.Write( REG_GPIO,0x083 | FTImpl.Read(REG_GPIO));
	/* Audio playback api*/    
	FTImpl.Write( REG_VOL_SOUND,255);
        SAMAPP_Aud_Music_Player_Streaming();	
	//SAMAPP_Aud_Music_Player();
#endif

#endif

#ifdef SAMAPP_ENABLE_APIS_SET5
#ifdef MSVC_PLATFORM
	SAMAPP_ChineseFont();
#endif
#endif
}
