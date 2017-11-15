
/**
 * Clue Media Experience
 *  
 * shapes.h - file containing primitive drawing operation functions.
 *
 * The library has the possibility to draw a circle, a rectangle, a point,
 * and other primitive operations and also, you can update the screen 
 * using the flip function.
 */


#ifndef __SHAPES_H__
#define __SHAPES_H__


#include "utils.h"


#define DEFSHP_COLOR	"#96C2E5"


/**
 * Update command properties with missing attributes, depending by
 * the command that has to be executed.
 * 
 * @param data command data structure
 * @return improved command data structure
 */
CmdData SetShapeProps(CmdData data);

void DrawShape(CmdData data);

void OverdrawShape(CmdData data);

void DrawLine(CmdData data);

void OverdrawLine(CmdData data);

void DrawRectangle(CmdData data);

void OverdrawRectangle(CmdData data);

void DrawCircle(CmdData data);

void OverdrawCircle(CmdData data);

void DrawProgressBar(CmdData data);

void OverdrawProgressBar(CmdData data);

#endif