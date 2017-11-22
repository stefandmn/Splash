
/**
 * Clue Media Experience
 * 
 * utils.c - implementation of utilities functions
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <malloc.h>
#include <stdarg.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/vt.h>
#include <linux/kd.h>

#include "utils.h"


/**
 * Errors collector static function to identify the most appropriate error through the
 * resource allocation process.
 *
 * @param existing_error existing error code
 * @param existing_error_str existing error in string format
 * @param new_error new found error code
 * @param new_error_str new fund error in string format
 *
 * @return the closest error found
 */
static inline const char* getErrorString(int existing_error, const char* const existing_error_str, int new_error, const char* const new_error_str)
{
	// Only report the first error found - any error that follows is probably just a cascading effect.
	const char* error_str = (char*) ((~(intptr_t) existing_error & (intptr_t) new_error & (intptr_t) new_error_str)
		| ((intptr_t) existing_error & (intptr_t) existing_error_str));

	return (error_str);
}

/**
 * Open a graphical console initializing all structures and
 * the entire memory that will be used by the console
 *
 * @param vt virtual terminal structure
 * @return 0 if the opening is happening in the right way and
 *		  -1 if an error occurs during initialization process
 */
int OpenGraphicsOnConsole(Console* vt)
{
	const char* error_str = NULL;
	int error = 0;

	/*
	   Open the current tty
	     "In Linux the PC monitor is usually called the console and has several device special files associated
	     with it: vc/0 (tty0), vc/1 (tty1), vc/2 (tty2), etc. When you log in you are on vc/1. To go to vc/2
	     (on the same screen) press down the 2 keys Alt(left)-F3. For vc/3 use Left Alt-F3, etc. These (vc/1,
	     vc/2, vc/3, etc.) are called "virtual terminals". vc/0 (tty0) is just an alias for the current virtual
	     terminal and it's where messages from the system are sent. Thus messages from the system will be seen
	     on the console (monitor) regardless of which virtual terminal it is displaying."
	 */
	const int cur_tty = open("/dev/tty0", O_RDWR);
	const int open_cur_tty_error = (cur_tty >> ((sizeof (int)*8) - 1));
	const char* open_cur_tty_error_str = "Could not open /dev/tty0. Check permissions.";

	error_str = getErrorString(error, error_str, open_cur_tty_error, open_cur_tty_error_str);
	error = error | open_cur_tty_error;

	struct vt_stat vts;

	const int get_state_error = ioctl(cur_tty, VT_GETSTATE, &vts);
	const char* get_state_error_str = "VT_GETSTATE failed on /dev/tty0";

	error_str = getErrorString(error, error_str, get_state_error, get_state_error_str);
	error = error | get_state_error;

	vt->prev_tty_ndx = vts.v_active;

	/*
	 "VT_OPENQRY
	     This call is used to find an available VT. The argument
	     to the ioctl is a pointer to an integer. The integer
	     will be filled in with the number of the first avail-
	     able VT that no other process has open (and hence, is
	     available to be opened). If there are no available
	     VTs, then -1 will be filled in."
	*/
	const int open_query_error = ioctl(cur_tty, VT_OPENQRY, vt->tty_ndx);
	const char* open_query_error_str = "No open ttys available";

	error_str = getErrorString(error, error_str, open_query_error, open_query_error_str);
	error = error | open_query_error;

	const int close_cur_tty_error = close(cur_tty);
	const char* close_cur_tty_error_str = "Could not close parent tty";

	error_str = getErrorString(error, error_str, close_cur_tty_error, close_cur_tty_error_str);
	error = error | close_cur_tty_error;

	char tty_file_name[11];

	(void) snprintf(tty_file_name, 11, "/dev/tty%d", vt->tty_ndx);

	const int tty = open(tty_file_name, O_RDWR);
	const int open_tty_error = (cur_tty >> ((sizeof (int)*8) - 1));
	const char* open_tty_error_str = "Could not open tty";

	error_str = getErrorString(error, error_str, open_tty_error, open_tty_error_str);
	error = error | open_tty_error;

	vt->tty = tty;

	/*
	 "VT_ACTIVATE
	    This call has the effect of making the VT specified in
	    the argument the active VT. The VT manager will cause
	    a switch to occur in the same manner as if a hotkey had
	    initiated the switch.  If the specified VT is not open
	    or does not exist the call will fail and errno will be
	    set to ENXIO."

	 "VT_WAITACTIVE
	    If the specified VT is already active, this call
	    returns immediately. Otherwise, it will sleep until
	    the specified VT becomes active, at which point it will
	    return."
	*/
	const int activate_tty_error = ioctl(vt->tty, VT_ACTIVATE, vt->tty_ndx);
	const char* activate_tty_error_str = "Could not activate tty";

	error_str = getErrorString(error, error_str, activate_tty_error, activate_tty_error_str);
	error = error | activate_tty_error;

	const int waitactive_tty_error = ioctl(vt->tty, VT_WAITACTIVE, vt->tty_ndx);
	const char* waitactive_tty_error_str = "Could not switch to tty";

	error_str = getErrorString(error, error_str, waitactive_tty_error, waitactive_tty_error_str);
	error = error | waitactive_tty_error;

	/*
	  "KDSETMODE
	   This call is used to set the text/graphics mode to the VT.

	      KD_TEXT indicates that console text will be displayed on the screen
	      with this VT. Normally KD_TEXT is combined with VT_AUTO mode for
	      text console terminals, so that the console text display will
	      automatically be saved and restored on the hot key screen switches.

	      KD_GRAPHICS indicates that the user/application, usually Xserver,
	      will have direct control of the display for this VT in graphics
	      mode. Normally KD_GRAPHICS is combined with VT_PROCESS mode for
	      this VT indicating direct control of the display in graphics mode.
	      In this mode, all writes to this VT using the write system call are
	      ignored, and the user is responsible for saving and restoring the
	      display on the hot key screen switches."
	 Save the current VT mode. This is most likely KD_TEXT.
	*/
	const int kdgetmode_error = ioctl(vt->tty, KDGETMODE, vt->prev_kdmode);
	const char* kdgetmode_error_str = "Could not get mode for tty";

	error_str = getErrorString(error, error_str, kdgetmode_error, kdgetmode_error_str);
	error = error | kdgetmode_error;

	// Set VT to GRAPHICS (user draw) mode

	const int kdsetmode_graphics_error = ioctl(vt->tty, KDSETMODE, KD_GRAPHICS);
	const char* kdsetmode_graphics_error_str = "Could not set graphics mode for tty";

	error_str = getErrorString(error, error_str, kdsetmode_graphics_error, kdsetmode_graphics_error_str);
	error = error | kdsetmode_graphics_error;

	/*
	 If vt blanking is active, for example when running this program from a remote terminal,
	 setting KD_GRAPHICS will not disable the blanking. Reset to KD_TEXT from KD_GRAPHICS will
	 force disable blanking. Then return to KD_GRAPHICS for drawing.

	 Note: KD_TEXT (default) to KD_TEXT will do nothing, so blanking will not be disable unless
	 the mode is changing. i.e. the initial set to KD_GRAPHICS above is useful.
	*/
	const int kdsetmode_text_error = ioctl(vt->tty, KDSETMODE, KD_TEXT);
	const char* kdsetmode_text_error_str = "Could not set text mode for tty";

	error_str = getErrorString(error, error_str, kdsetmode_text_error, kdsetmode_text_error_str);
	error = error | kdsetmode_text_error;

	const int kdsetmode_graphics_reset_error = ioctl(vt->tty, KDSETMODE, KD_GRAPHICS);
	const char* kdsetmode_graphics_reset_error_str = "Could not reset graphics mode for tty";

	error_str = getErrorString(error, error_str, kdsetmode_graphics_reset_error, kdsetmode_graphics_reset_error_str);
	error = error | kdsetmode_graphics_reset_error;

	if (error == -1)
	{
		WARN(" Error in opening graphics in console: %s", error_str);
	}

	return error;
}

/**
 * Close the graphical console by releasing all structures and
 * the entire memory allocated and used by the console
 *
 * @param vt virtual terminal structure
 * @return 0 if the closing is happening in the right way and
 *		  -1 if an error occurs during closing process
 */
int CloseConsole(Console* vt)
{
	const char* error_str = NULL;
	int error = 0;

	// Reset previous mode on tty (likely KD_TEXT)
	const int kdsetmode_error = ioctl(vt->tty, KDSETMODE, vt->prev_kdmode);
	const char* kdsetmode_error_str = "Could not reset previous mode for tty";

	error_str = getErrorString(error, error_str, kdsetmode_error, kdsetmode_error_str);
	error = error | kdsetmode_error;

	// Restore previous tty
	const int activate_tty_error = ioctl(vt->tty, VT_ACTIVATE, vt->prev_tty_ndx);
	const char* activate_tty_error_str = "Could not activate previous tty";

	error_str = getErrorString(error, error_str, activate_tty_error, activate_tty_error_str);
	error = error | activate_tty_error;

	const int waitactive_tty_error = ioctl(vt->tty, VT_WAITACTIVE, vt->prev_tty_ndx);
	const char* waitactive_tty_error_str = "Could not switch to previous tty";

	error_str = getErrorString(error, error_str, waitactive_tty_error, waitactive_tty_error_str);
	error = error | waitactive_tty_error;

	// Close tty
	const int close_tty_error = close(vt->tty);
	const char* close_tty_error_str = "Could not close tty";

	error_str = getErrorString(error, error_str, close_tty_error, close_tty_error_str);
	error = error | close_tty_error;

	if (error == -1)
	{
		WARN(" Error closing console: %s", error_str);
	}

	return error;
}

unsigned long hex2dec(char* hexstr)
{
	int i, decval = 0;
	unsigned long decnum = 0;
	
	// Find Hexadecimal Number
	for (i = 0; i < strlen(hexstr); i++)
	{
		if(hexstr[i] >= '0' && hexstr[i] <= '9')
		{
			decval = hexstr[i] - 48;
		}
		else if(hexstr[i] >= 'a' && hexstr[i] <= 'f')
		{
			decval = hexstr[i] - 97 + 10;
		}
		else if(hexstr[i] >= 'A' && hexstr[i] <= 'F')
		{
			decval = hexstr[i] - 65 + 10;
		}

		decnum += decval * pow(16, strlen(hexstr) - i-1);
	}

	return decnum;
}

/**
 * Create as 16bit color map from a RGB representation.
 *
 * @param r red color
 * @param g green color
 * @param b blue color
 * @return 16bit color map
 */
unsigned int get16bitColorFromRGB(int r, int g, int b)
{
	return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}

/**
 * Convert a 32bit color map into 16bit
 * 
 * @param color 32bit color map
 * @return 16bit color map
 */
unsigned int get16bitColorFrom32bit(int32_t color)
{
	return ((color >> 16) & 0xFF) << 11 | ((color >> 8) & 0xFF) << 5 | (color & 0xFF);
}

/**
 * Create as 16bit color map from a HEX representation.
 *
 * @param color hex color representation
 * @return 16bit color map
 */
unsigned int get16bitColorFromHEX(char *color)
{
	char *value, *tmp;
	int r = 0, g = 0, b = 0;

	value = malloc(sizeof(char) * 6);
	tmp = malloc(sizeof(char) * 3);
	
	if(color != NULL && strcmp(color, "") > 0)
	{
		if(color[0] == '#') strcpy(value, color + 1);
			else strcpy(value, color);

		if(sizeof(value) < 6) strncat(value, "000000", 6 - strlen(value));

		strncpy(tmp, value, 2);
		tmp[2] = '\0';
		r = hex2dec(tmp);

		strncpy(tmp, value + 2, 2);
		tmp[2] = '\0';
		g = hex2dec(tmp);

		strncpy(tmp, value + 4, 2);
		tmp[2] = '\0';
		b = hex2dec(tmp);
	}
	
	free(value);
	free(tmp);

	return get16bitColorFromRGB(r, g, b);
}

/**
 * Load graphical resource from a file
 *
 * @param filename file name
 * @return Resource structure containing all the details and references about
 *			specified graphical resource.
 */
File getFile(char *filename)
{
	struct stat fileinfo;
	File file;
	int i, rc;

	if(filename != NULL)
	{
		//get real file name
		if(startsWith(filename, "/"))
		{
			strcpy(file.path, filename);
		}
		else
		{
			rc = stat(filename, &fileinfo);

			if (rc)
			{
				strcpy(file.path, RESPATH);
				if(!endsWith(RESPATH, "/")) strcat(file.path, "/");
				strcat(file.path, filename);
			}
			else
			{
				strcpy(file.path, realpath(filename, NULL));
			}
		}

		rc = stat(file.path, &fileinfo);

		if (rc)
		{
			file.data = NULL;
			file.size = -1;
			ERROR("Can not open resource file: %s", file.path);
		}
		else
		{
			file.size = fileinfo.st_size;
			file.data = (unsigned char*) malloc(file.size + 100);
			int fd = open(file.path, O_RDONLY);

			i = 0;
			while (i < file.size)
			{
				rc = read(fd, file.data + i, file.size - i);
				i += rc;
			}

			close(fd);
		}
	}
	else
	{
		file.data = NULL;
		file.size = -1;
	}

	return file;
}

/**
 * Logger implementation to be used for the entire project
 *
 * @param filename source code that uses the logger
 * @param line line number from source code that is publishing the log message
 * @param fmt log message
 * @param ... other parameters that are appended to the log message (through the formatting process)
 */
void Logger(char* level, char* filename, int line, char *fmt, ...)
{
	va_list list;
	char *p, *r;
	int e;
	FILE *fp;
	time_t t;
	char *buf;	

	if ((MSG_LEVEL == MSG_LEVEL_ALL && strcmp(level, "ALL")) ||
		(MSG_LEVEL == MSG_LEVEL_DEBUG && strcmp(level, "DEBUG")) || 
		(MSG_LEVEL == MSG_LEVEL_INFO && strcmp(level, "INFO")) || 
		(MSG_LEVEL == MSG_LEVEL_WARN && strcmp(level, "WARN")) || 
		(MSG_LEVEL == MSG_LEVEL_ERROR && strcmp(level, "ERROR")))
	{
		if (!(fp = fopen(LOGFILE, "a"))) return;

		time(&t);
		buf = (char*) malloc(strlen(ctime(&t)) + 1);
		snprintf(buf, strlen(ctime(&t)), "%s ", ctime(&t));	

		fprintf(fp, "%s %s [%d] [%s: %d]: ", buf, level, getpid(), filename, line);

		va_start(list, fmt);

		for (p = fmt; *p; ++p)
		{
			if (*p != '%')//If simple string
			{
				fputc(*p, fp);
			}
			else
			{
				switch (*++p)
				{
					case 's': /* string */
					{
						r = va_arg(list, char *);
						if(r != NULL)
							fprintf(fp, "%s", r);
						else
							fprintf(fp, "%s", "NULL");
						continue;
					}
					case 'd': /* integer */
					{
						e = va_arg(list, int);
						fprintf(fp, "%d", e);
						continue;
					}
					case 'b': /* boolean */
					{
						e = va_arg(list, int);
						fprintf(fp, "%d", e == 1 ? "TRUE" : "FALSE");
						continue;
					}
					default:
						fputc(*p, fp);
				}
			}
		}

		va_end(list);
		fputc('\n', fp);

		fflush(fp);
		fclose(fp);
		free(buf);
	}	
}

/**
 * Check is a string pointer starts with a specific sub-string (managed as pointer)
 *
 * @param base string pointer to look in
 * @param str string pointer to find it
 * @return true if the second string parameter is the prefix of the first string parameter
 */
bool startsWith(char* base, char* str)
{
	return (strstr(base, str) - base) == 0;
}

/**
 * Check is a string pointer ends with a specific sub-string (managed as pointer)
 *
 * @param base string pointer to look in
 * @param str string pointer to find it
 * @return true if the second string parameter is the suffix of the first string parameter
 */
bool endsWith(char* base, char* str)
{
	int blen = strlen(base);
	int slen = strlen(str);

	return (blen >= slen) && (0 == strcmp(base + blen - slen, str));
}

/**
 * Check if a string pointer is part of another string pointer
 *
 * @param base string pointer to look in
 * @param str string pointer to find it
 * @param startIndex position in the first string pointer where to start searching
 * @return -1 if the second string is not found in the first string starting from the specified position or
 *			>=0 if the string is found from the specified position
 */
int indexOfShift(char* base, char* str, int startIndex)
{
	int result;
	int baselen = strlen(base);

	// str should not longer than base
	if (strlen(str) > baselen || startIndex > baselen)
	{
		result = -1;
	}
	else
	{
		if (startIndex < 0)
		{
			startIndex = 0;
		}

		char* pos = strstr(base + startIndex, str);

		if (pos == NULL)
		{
			result = -1;
		}
		else
		{
			result = pos - base;
		}
	}

	return result;
}

/**
 * Check if a string pointer is part of another string pointer
 *
 * @param base string pointer to look in
 * @param str string pointer to find it
 * @return -1 if the second string is not found in the first string or
 *			>=0 if the string is found from the beginning from of the string (first index is considered 0)
 */
int indexOf(char* base, char* str)
{
	return indexOfShift(base, str, 0);
}

/**
 * Check if a string pointer is part of another string pointer
 *
 * @param base string pointer to look in
 * @param str string pointer to find it
 * @return -1 if the second string is not found in the first string or
 *			>=0 if the string is found for the first time from the ending of the string (last string index is considered string length - 1)
 */
int lastIndexOf(char* base, char* str)
{
	int result;

	// str should not longer than base
	if (strlen(str) > strlen(base))
	{
		result = -1;
	}
	else
	{
		int start = 0;
		int endinit = strlen(base) - strlen(str);
		int end = endinit;
		int endtmp = endinit;

		while (start != end)
		{
			start = indexOfShift(base, str, start);
			end = indexOfShift(base, str, end);

			// not found from start
			if (start == -1)
			{
				end = -1; // then break;
			}
			else if (end == -1)
			{
				// found from start but not found from end
				// move end to middle
				if (endtmp == (start + 1))
				{
					end = start; // then break;
				}
				else
				{
					end = endtmp - (endtmp - start) / 2;

					if (end <= start) end = start + 1;

					endtmp = end;
				}
			}
			else
			{
				// found from both start and end move start to end and
				// move end to base - strlen(str)
				start = end;
				end = endinit;
			}
		}

		result = start;
	}

	return result;
}

/**
 * Perform string pointer trimming (removing white spaces from the beginning and to the end of the string)
 *
 * @param str string pointer to be trimmed
 * @return trimmed string pointer
 */
char* strtrim(char *str)
{
	char *end;

	// Trim leading space
	while(isspace(*str)) str++;

	if(*str == 0)  // All spaces?
		return str;

	// Trim trailing space
	end = str + strlen(str) - 1;
	while(end > str && isspace(*end)) end--;

	// Write new null terminator
	*(end+1) = 0;

	return str;
}

/**
 *  Returns Min value between two integers
 *
 * @param a first value to be compared
 * @param b second value to be compared
 * @return the value that is minimum
 */
int min(int a, int b)
{
	if (a < b) return a;
		else return b;
}

/**
 *  Returns Max value between two integers
 *
 * @param a first value to be compared
 * @param b second value to be compared
 * @return the value that is maximum
 */
int max(int a, int b)
{
	if (a > b) return a;
		else return b;
}

/**
 * Check if the input structure for command representation contains a specific
 * property name.
 *
 * @param data structure for command representation
 * @param name property name
 * @return true if the property is defined in the input structure
 */
bool hasDataProperty(CmdData data, char* name)
{
	char *token;
	bool found = false;

	token = strtok (data.props, ",");

	while (token != NULL && !found)
	{
		if (indexOf(strtrim(token), name) == 0)
		{
			found = true;
		}
		else
		{
			token = strtok (NULL, ",");
		}
	}

	return found;
}

/**
 * Set the specified property and related value into the input structure for command representation.
 *
 * @param data structure for command representation
 * @param name property name
 * @param value property value
 */
CmdData setDataProperty(CmdData data, char* name, char* value)
{
	char *token;
	char props[1000];
	bool found = false;

	strcpy(props, "");
	token = strtok (data.props, ",");

	while (token != NULL)
	{
		if(strcmp(props, "") != 0) strcat(props, ",");

		if (indexOf(strtrim(token), name) == 0)
		{
			strcat(props, name);
			strcat(props, "=");
			strcat(props, value);
			found = true;
		}
		else
		{
			strcat(props, token);
		}


		token = strtok (NULL, ",");
	}

	if(!found)
	{
		if(strcmp(props, "") != 0) strcat(props, ",");
		strcat(props, name);
		strcat(props, "=");
		strcat(props, value);
	}

	strcpy(data.props, props);
	return data;
}

/**
 * Set the specified property and related integer value into the input structure
 * for command representation.
 *
 * @param data structure for command representation
 * @param name property name
 * @param value property integer value
 */
CmdData setIntDataProperty(CmdData data, char* name, int value)
{
	char number[10];

	sprintf(number,"%d",value);
	return setDataProperty(data, name, number);
}

/**
 * Set the specified property and related boolean value into the input structure
 * for command representation.
 *
 * @param data structure for command representation
 * @param name property name
 * @param value property boolean value
 */
CmdData setBoolDataProperty(CmdData data, char* name, bool value)
{
	if(value)
	{
		return setDataProperty(data, name, "true");
	}
	else
	{
		return setDataProperty(data, name, "false");
	}
}

/**
 * Get the property value for the specified property name found in the input structure
 * for command representation
 *
 * @param data input structure for command representation
 * @param name property name
 * @return property value
 */
char* getDataProperty(CmdData data, char* name)
{
	char *value;
	char *token1, *token2;
	bool found = false;

	token1 = strtok (data.props, ",");

	while (token1 != NULL && !found)
	{
		if (indexOf(strtrim(token1), name) == 0)
		{
			found = true;
		}
		else
		{
			token1 = strtok (NULL, ",");
		}
	}

	if (found)
	{
		token2 = strtok (token1, "=");
		token2 = strtok (NULL, "=");
		
		if(token2 != NULL && strcmp(strtrim(token2), "") != 0)
		{
			value = malloc(sizeof(char) * (strlen(token2)+1));
			strcpy(value, token2);
			value[strlen(token2)] = '\0';

			return value;
		}
		else return NULL;
	}
	
	return NULL;
}

/**
 * Get the integer value for the specified property name found in the input structure
 * for command representation
 *
 * @param data input structure for command representation
 * @param name property name
 * @return integer property value
 */
int getIntDataProperty(CmdData data, char* name)
{
	int value;
	char* txtval = getDataProperty(data, name);

	if (txtval != NULL) 
	{
		value = atoi(txtval);
		free(txtval);
		
		return value;
	}
	else return NULL;
}

/**
 * Get the boolean value for the specified property name found in the input structure
 * for command representation
 *
 * @param data input structure for command representation
 * @param name property name
 * @return boolean property value
 */
bool getBoolDataProperty(CmdData data, char* name)
{
	bool value = false;
	char* txtval = getDataProperty(data, name);

	if(txtval != NULL) 
	{
		value = strcmp(txtval, "true") == 0 || strcmp(txtval, "on") == 0 || strcmp(txtval, "1") == 0;
		free(txtval);
	}
	
	return value;
}

/**
 * Get the color value for the specified property name found in the input structure for
 * command representation. The returned color is a 16bit color map from a HEX representation.
 *
 * @param color property name
 * @return 16bit color map
 */
unsigned int getColorDataProperty(CmdData data, char* name)
{
	unsigned int value = -1;
	char* txtval = getDataProperty(data, name);

	if(txtval != NULL) 
	{
		value = get16bitColorFromHEX(txtval);
		free(txtval);
	}
	
	return value;
}
