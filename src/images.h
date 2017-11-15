
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


#define DEF_IMG		"images/splash.jpg"


/**
 * Update command properties with missing attributes, depending by
 * the command that has to be executed.
 * 
 * @param data command data structure
 * @return improved command data structure
 */
CmdData SetImageProps(CmdData data);

void DrawImage(CmdData data);

void DrawJpgImage(CmdData data);

void DrawPngImage(CmdData data);

void DrawBmpImage(CmdData data);

#endif