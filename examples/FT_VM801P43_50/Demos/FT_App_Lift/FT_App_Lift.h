#ifndef _FT_APP_LIFT_H_
#define _FT_APP_LIFT_H_




#define FT_APP_MATIRX_PRECITION (1)
#define FT_APP_MATRIX_GPU_PRECITION (256)
/* Macros for lift application */
#define LIFTAPP_DIR_NONE (0)
#define LIFTAPP_DIR_DOWN (1)
#define LIFTAPP_DIR_UP (2)

#define FT_GPU_NUMCHAR_PERFONT (128)
#define FT_APP_GRAPHICS_FLIP_RIGHT (1)
#define FT_APP_GRAPHICS_FLIP_BOTTOM (2)
/* Structure for lift demo implementation */
typedef struct S_LiftAppBallsLinear
{
	int16_t xOffset;
	int16_t yOffset;
	uint8_t dx;
	uint8_t dy;
}S_LiftAppBallsLinear_t;

/* Structure for lift application font */
#define LIFTAPPCHARRESIZEPRECISION (256)
#define LIFTAPPMAXCHARS (12)
#define LIFTAPPMAXFILENAMEARRAY (16)//macro used for file name array
#define LIFTAPPCHUNKSIZE (512L) //sdcard sector size
#define LIFTAPPAUDIOBUFFERTOTALSIZE (64*1024) //total buffer size of audio buffer allocated
/* Audio atate machine */
#define LIFTAPPAUDIOSTATE_NONE (0)
#define LIFTAPPAUDIOSTATE_INIT (1)
#define LIFTAPPAUDIOSTATE_DOWNLOAD (2)
#define LIFTAPPAUDIOSTATE_PLAYBACK (3)
#define LIFTAPPAUDIOSTATE_STOP (4)

typedef struct S_LiftAppFont
{
	uint32_t MaxWidth;
	uint32_t MaxHeight;
	uint32_t Stride;
	uint8_t Format;
	uint8_t Width[LIFTAPPMAXCHARS];
}S_LiftAppFont_t;

/* bitmap transformation matrix - tbd - change the below to fixed point arthematic */
typedef struct Ft_App_GraphicsMatrix
{
	float a;
	float b;
	float c;
	float d;
	float e;
	float f;
}Ft_App_GraphicsMatrix_t;
/* Transform functionality */
typedef struct Ft_App_PostProcess_Transform
{
	/* all the below units are in terms of 256 units - signed 8.8 format */
	/* a and d are used for scaling, c and f are used for offest position */
	int32_t Transforma;
	int32_t Transformb;
	int32_t Transformc;
	int32_t Transformd;
	int32_t Transforme;
	int32_t Transformf;
	/* Spectial effects, mirror effect,  */
	uint32_t SpecialEffect;
}Ft_App_PostProcess_Transform_t;


typedef struct S_LiftAppRate
{
	uint16_t IttrCount;//total number of counts since starting
	uint16_t CurrTime;//current time since starting
}S_LiftAppRate_t;

typedef struct S_LiftAppTrasParams
{
	int16_t FloorNumChangeRate;//change of floor number in terms of units
	int16_t ResizeRate;//resize rate which includes from org to resize and resize to orginal
	int16_t ResizeDimMin;//dimension of min resize in terms of 8 bits precision
	int16_t ResizeDimMax;//dimension of max resize in terms of 8 bits precision
	int16_t FloorNumStagnantRate;//Floor being stagnant in terms of units
	int16_t MaxFloorNum;//make sure 0 is not considered in this usecase
	int16_t MinFloorNum;//make sure 0 is not considered in this usecase
}S_LiftAppTrasParams_t;


/* context of lift application */
typedef struct S_LiftAppCtxt
{
	S_LiftAppTrasParams_t SLATransPrms;
	S_LiftAppRate_t SLARate;
	uint8_t ArrowDir;//0 means no direction, 1 means going down, 2 means going up
	int8_t CurrFloorNum;//not more less than -128 and more than 127
	int8_t NextFloorNum;//Next floor number from this floor - need to handle same floor number also
	int16_t CurrFloorNumChangeRate;
	int16_t CurrFloorNumStagnantRate;
	int16_t CurrFloorNumResizeRate;
	int16_t CurrArrowResizeRate;
        /* Audio playback management */
	uint32_t AudioCurrAddr;
	uint32_t AudioCurrFileSz;
	//int32_t AudioTotBuffSz;
	char AudioFileNames[LIFTAPPMAXFILENAMEARRAY];
  	char AudioFileIdx;
	char AudioState;
	char AudioPlayFlag;
    char opt_orientation;
}S_LiftAppCtxt_t;




/* Sample app APIs for widgets */

void SAMAPP_CoPro_Widget_Calibrate();
void SAMAPP_CoPro_Loadimage();
void SAMAPP_Aud_Music_Player_Streaming();
void SAMAPP_Aud_Music_Player();
void Loadimage_display();
void FT_LiftApp_Portrait();
int16_t linear(int16_t p1,int16_t p2,uint16_t t,uint16_t rate);
void display_logo();
void SAMAPP_Touch();
void SAMAPP_Sound();
//uint32_t play_music(char *pFileName,uint32_t dstaddr,uint8_t Option,uint32_t Buffersz);
uint32_t play_music(FT_SDFile &r /*char *pFileName*/,uint32_t dstaddr,uint8_t Option,uint32_t Buffersz);
void font_display(int16_t BMoffsetx, int16_t BMoffsety,uint32_t string_length,char *string_display);

void SAMAPP_PowerMode();
void SAMAPP_BootupConfig();
int16_t Ft_App_TrsMtrxTranslate(Ft_App_GraphicsMatrix_t *pMatrix,float x,float y,float *pxres,float *pyres);
int16_t Ft_App_UpdateTrsMtrx(Ft_App_GraphicsMatrix_t *pMatrix,Ft_App_PostProcess_Transform_t *ptrnsmtrx);
int16_t Ft_App_TrsMtrxLoadIdentity(Ft_App_GraphicsMatrix_t *pMatrix);
int16_t Ft_App_TrsMtrxRotate(Ft_App_GraphicsMatrix_t *pMatrix,int32_t th);

int32_t Ft_App_LoadRawAndPlay(char *filename,int32_t DstAddr,uint8_t Option, int32_t Buffersz);
int32_t FT_LiftApp_FTransition(S_LiftAppCtxt_t *pLACtxt,int32_t dstaddr/*, uint8_t opt_landscape*/);
int32_t play_audio(FT_SDFile &aud_reader);
void Load_afile(uint32_t add, FT_SDFile &r,uint8_t noofsectors);
void Playback(uint8_t play,uint8_t vol);
void Ft_Play_Sound(uint8_t sound,uint8_t vol,uint8_t midi);
void Init_Audio();
//int32_t FT_LiftApp_FTransition_portrait(S_LiftAppCtxt_t *pLACtxt,int32_t dstaddr);
int32_t Ft_App_AudioPlay(S_LiftAppCtxt_t *pCtxt,int32_t DstAddr,int32_t BuffChunkSz);
int32_t test_sdcard();

#endif /* _FT_APP_LIFT_H_ */

/* Nothing beyond this */








