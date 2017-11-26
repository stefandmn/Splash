/**
 * Clue Media Experience
 *  
 * buffer.h - file containing basic graphical functions.
 *
 * File containing initialization functions and main structures 
 * for buffer management.
 */

#ifndef __BUFFER_H__
#define __BUFFER_H__


#include "utils.h"


#define DEF_DEV		"/dev/fb0"


/**
 * Open frame buffer console/device and initializing pointers and structures
 * for graphical manipulation
 */
void OpenBuffer(void);

/**
 * Close frame buffer console/device and dropping all the pointers and structures
 * used by graphical objects.
 */
void CloseBuffer(void);

/**
 * Apply black color to the frame console
 */
void ResetScreen(void);

/**
 * Adapt object position
 * 
 * @param x Input horizontal position
 * @param y Input vertical position
 * @param width Width of graphical object
 * @param height Height of graphical object
 */
void SetPosition(int *x, int *y, int width, int height);

#endif