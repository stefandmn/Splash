
/**
 * Clue Media Experience
 *  
 * font.h - file containing text functions.
 *
 * The library has the possibility to render texts with different background
 * color and foreground color.
 */


#ifndef __TEXT_H__
#define __TEXT_H__


#include "utils.h"


#define DEFTXT_FPATH	"fonts/regular.ttf"
#define DEFTXT_COLOR	"#96C2E5"
#define DEFTXT_SPACE	3
#define DEFTXT_SIZE		23

/**
 * Update command properties with missing attributes, depending by
 * the command that has to be executed.
 * 
 * @param data command data structure
 * @return improved command data structure
 */
CmdData SetTextProps(CmdData data);

/**
 * Draw a text message using specific coordinates
 * 
 * @param data command data structure containing text message and coordinates
 */
void DrawText(CmdData data);

/**
 * Remove a text message using specific coordinates
 * 
 * @param data command data structure containing text message and coordinates
 */
void OverdrawText(CmdData data);

#endif