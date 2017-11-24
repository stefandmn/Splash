
/**
 * Clue Media Experience
 *  
 * images.h - file containing image functions.
 *
 * The library has the possibility to render PNG, PPM and BMP images.
 */

#ifndef __IMAGES_H__
#define __IMAGES_H__


#include "utils.h"


#define DEF_IMG		"images/splash.png"

#define PAN_STEPPING 20
#define PNG_BYTES_TO_CHECK 4

#define STATUS_OK				0
#define STATUS_ACCESS_ERR		1	/* read/access error */
#define STATUS_FORMAT_ERR		2	/* file format error */


struct ImageInfo
{
	int width, height;
	unsigned char *rgb;
	unsigned char *alpha;
	int do_free;
};

/**
 * Update command properties with missing attributes, depending by
 * the command that has to be executed.
 * 
 * @param data command data structure
 * @return improved command data structure
 */
CmdData SetImageProps(CmdData data);

void DrawImage(CmdData data);

void OverdrawImage(CmdData data);

int GetImageSize(char *imgfile, int *x, int *y);

int GetImageBuffer(char *imgfile, unsigned char *buffer, unsigned char **alpha, int x,int y);

bool isPngImage(char *imgfile);

int GetPngImageSize(char *imgfile, int *x, int *y);

int GetPngImageBuffer(char *imgfile, unsigned char *buffer, unsigned char ** alpha, int x, int y);

bool isBmpImage(char *imgfile);

int GetBmpImageSize(char *imgfile, int *x, int *y);

int GetBmpImageBuffer(char *imgfile, unsigned char *buffer, unsigned char **alpha, int x, int y);

bool isJpgImage(char *imgfile);

int GetJpgImageSize(char *imgfile, int *x, int *y);

int GetJpgImageBuffer(char *imgfile, unsigned char *buffer, unsigned char ** alpha, int x, int y);

void doRotate(struct ImageInfo *i, int rot);

void doFitToScreen(struct ImageInfo *i, int screen_width, int screen_height, int ignoreaspect, int cal);

void doEnlarge(struct ImageInfo *i, int screen_width, int screen_height, int ignoreaspect);

#endif