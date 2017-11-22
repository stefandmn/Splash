
/**
 * Clue Media Experience
 *  
 * utils.h - file containing utilities functions.
 *
 * The library has several utilities for data conversion or 
 * enrichment.
 */


#ifndef __UTILS_H__
#define __UTILS_H__


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>


/**
 * @def MSG_LEVEL_ALL
 * @hideinitializer
 * @brief Configure level message to all type of messages.
 */ 
#define	MSG_LEVEL_ALL 0

/**
 * @def MSG_LEVEL_DEBUG
 * @hideinitializer
 * @brief Configure level message to debug.
 */ 
#define	MSG_LEVEL_DEBUG	1

/**
 * @def MSG_LEVEL_INFO
 * @hideinitializer
 * @brief Configure level message to info.
 */
#define	MSG_LEVEL_INFO	2

/**
 * @def IBUTTERFREE_MSG_LEVEL_WRN
 * @hideinitializer
 * @brief Configure level message to warning.
 */ 
#define	MSG_LEVEL_WARN	3

/**
 * @def MSG_LEVEL_ERROR
 * @hideinitializer
 * @brief Configure level message to error.
 */ 
#define	MSG_LEVEL_ERROR	4

/**
 * @def MSG_LEVEL
 * @hideinitializer
 * @brief Configure the message level.
 */
#ifndef MSG_LEVEL
	#define MSG_LEVEL MSG_LEVEL_DEBUG
#endif

/**
 * Declaration of LOGGER functions (used in the entire project)
 */
#define LOGGER(...) Logger("ALL", __FILE__, __LINE__, __VA_ARGS__) 
#define DEBUG(...) Logger("DBG", __FILE__, __LINE__, __VA_ARGS__)
#define INFO(...) Logger("INF", __FILE__, __LINE__, __VA_ARGS__)
#define WARN(...) Logger("WRN", __FILE__, __LINE__, __VA_ARGS__)
#define ERROR(...) Logger("ERR", __FILE__, __LINE__, __VA_ARGS__)


/**
 * Virtual terminal structure to emulate the graphic console
 */
typedef struct
{
    int prev_tty_ndx;
    int prev_kdmode;
    int tty;
    int tty_ndx;
} Console;

/**
 * Operating data structure collected from input parameters
 */
typedef struct
{
	int id;
	char value[255];
	char props[1000];
	int xpoint;
	int ypoint;
} CmdData;

/**
 * Resources file structure to load and manipulate graphical resources
 */
typedef struct
{
	char path[255];
	unsigned long size;
	unsigned char *data;
} File;

/**
 * @brief Structure relative of screen characteristics.
 */
typedef struct 
{
	FILE *fbf;							/* Handler */
	char *fbp;							/* Buffer */
	int fboffset;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	struct fb_var_screeninfo oinfo;
} FrameBuffer;



/** Graphical terminal*/
Console console;

/** Main path of resources */
char RESPATH[255];

/** Log file path */
char LOGFILE[255];

/** Frame buffer references and resources*/
FrameBuffer fbs;


/**
 * Open a graphical console initializing all structures and
 * the entire memory that will be used by the console
 *
 * @param vt virtual terminal structure
 * @return 0 if the opening is happening in the right way and
 *		  -1 if an error occurs during initialization process
 */
int OpenGraphicsOnConsole(Console* vt);

/**
 * Close the graphical console by releasing all structures and
 * the entire memory allocated and used by the console
 *
 * @param vt virtual terminal structure
 * @return 0 if the closing is happening in the right way and
 *		  -1 if an error occurs during closing process
 */
int CloseConsole(Console* vt);

/**
 * Create as 16bit color map from a RGB representation.
 *
 * @param r red color
 * @param g green color
 * @param b blue color
 * @return 16bit color map
 */
unsigned int get16bitColorFromRGB(int r, int g, int b);

/**
 * Convert a 32bit color map into 16bit
 * 
 * @param color 32bit color map
 * @return 16bit color map
 */
unsigned int get16bitColorFrom32bit(int32_t color);

/**
 * Create as 16bit color map from a HEX representation.
 *
 * @param colorr hex color representation
 * @return 16bit color map
 */
unsigned int get16bitColorFromHEX(char *color);


/**
 * Load graphical resource from a file
 *
 * @param filename file name
 * @return Resource structure containing all the details and references about
 *			specified graphical resource.
 */
File getFile(char *filename);

/**
 * Check is a string pointer starts with a specific sub-string (managed as pointer)
 *
 * @param base string pointer to look in
 * @param str string pointer to find it
 * @return true if the second string parameter is the prefix of the first string parameter
 */
bool startsWith (char* base, char* str);

/**
 * Check is a string pointer ends with a specific sub-string (managed as pointer)
 *
 * @param base string pointer to look in
 * @param str string pointer to find it
 * @return true if the second string parameter is the suffix of the first string parameter
 */
bool endsWith (char* base, char* str);

/**
 * Check if a string pointer is part of another string pointer
 *
 * @param base string pointer to look in
 * @param str string pointer to find it
 * @param startIndex position in the first string pointer where to start searching
 * @return -1 if the second string is not found in the first string starting from the specified position or
 *			>=0 if the string is found from the specified position
 */
int indexOfShift (char* base, char* str, int startIndex);

/**
 * Check if a string pointer is part of another string pointer
 *
 * @param base string pointer to look in
 * @param str string pointer to find it
 * @return -1 if the second string is not found in the first string or
 *			>=0 if the string is found for the first time from the beginning of the string (first index is considered 0)
 */
int indexOf (char* base, char* str);

/**
 * Check if a string pointer is part of another string pointer
 *
 * @param base string pointer to look in
 * @param str string pointer to find it
 * @return -1 if the second string is not found in the first string or
 *			>=0 if the string is found for the first time from the ending of the string (last string index is considered string length - 1)
 */
int lastIndexOf (char* base, char* str);

/**
 * Perform string pointer trimming (removing white spaces from the beginning and to the end of the string)
 *
 * @param str string pointer to be trimmed
 * @return trimmed string pointer
 */
char* strtrim(char *str);

/**
 *  Returns Min value between two integers
 *
 * @param a first value to be compared
 * @param b second value to be compared
 * @return the value that is minimum
 */
int min(int a, int b);

/**
 *  Returns Max value between two integers
 *
 * @param a first value to be compared
 * @param b second value to be compared
 * @return the value that is maximum
 */
int max(int a, int b);

/**
 * Check if the input structure for command representation contains a specific
 * property name.
 *
 * @param data structure for command representation
 * @param name property name
 * @return true if the property is defined in the input structure
 */
bool hasDataProperty(CmdData data, char* name);

/**
 * Set the specified property and related value into the input structure
 * for command representation.
 *
 * @param data structure for command representation
 * @param name property name
 * @param value property value
 */
CmdData setDataProperty(CmdData data, char* name, char* value);

/**
 * Set the specified property and related integer value into the input structure
 * for command representation.
 *
 * @param data structure for command representation
 * @param name property name
 * @param value property integer value
 */
CmdData setIntDataProperty(CmdData data, char* name, int value);

/**
 * Set the specified property and related boolean value into the input structure
 * for command representation.
 *
 * @param data structure for command representation
 * @param name property name
 * @param value property boolean value
 */
CmdData setBoolDataProperty(CmdData data, char* name, bool value);

/**
 * Get the property value for the specified property name found in the input structure
 * for command representation
 *
 * @param data input structure for command representation
 * @param name property name
 * @return property value
 */
char* getDataProperty(CmdData data, char* name);

/**
 * Get the integer value for the specified property name found in the input structure
 * for command representation
 *
 * @param data input structure for command representation
 * @param name property name
 * @return integer property value
 */
int getIntDataProperty(CmdData data, char* name);

/**
 * Get the boolean value for the specified property name found in the input structure
 * for command representation
 *
 * @param data input structure for command representation
 * @param name property name
 * @return boolean property value
 */
bool getBoolDataProperty(CmdData data, char* name);

/**
 * Get the color value for the specified property name found in the input structure for
 * command representation. The returned color is a 16bit color map from a HEX representation.
 *
 * @param color property name
 * @return 16bit color map
 */
unsigned int getColorDataProperty(CmdData data, char* name);

#endif