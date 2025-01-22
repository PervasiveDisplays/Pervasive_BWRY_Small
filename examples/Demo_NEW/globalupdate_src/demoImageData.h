/*
	demoImageData.h
  DEMO set of Image Data
*/

// Load Normal Update Images
#if (SCRN==154)   //1.54"
#include "image_data\154E5SE_testimage.c"
#define BW_monoBuffer        (uint8_t *) & testImage
#define frameSize	5776
#elif(SCRN==213)   //2.13"
#include "image_data\213E5SE_testimage.c"
#define frameSize	5512
#define BW_monoBuffer        (uint8_t *) & testImage
#elif(SCRN==266)   //2.66"
#include "image_data\266E5SE_testimage.c"
#define BW_monoBuffer        (uint8_t *) & testImage
#define frameSize	11248
#elif(SCRN==417)   //4.17"
#include "image_data\417E5SE_testimage.c"
#define BW_monoBuffer        (uint8_t *) & testImage
#define frameSize	30000
#endif
