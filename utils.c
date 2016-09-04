#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#endif

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <linux/vt.h>
#include <linux/kd.h>
#include "utils.h"

extern char fileLog[255];
extern int child;
extern int keepReopen;

static inline const char* getErrorString(int existing_error, const char* const existing_error_str, int new_error, const char* const new_error_str) 
{
	// Only report the first error found - any error that follows is probably just a cascading effect.
	const char* error_str = (char*) ((~(intptr_t) existing_error & (intptr_t) new_error & (intptr_t) new_error_str)
		| ((intptr_t) existing_error & (intptr_t) existing_error_str));

	return (error_str);
}

int openGraphicsOnConsole(cp_vt* vt) 
{
	const char* error_str = NULL;
	int error = 0;

	// Open the current tty
	//     "In Linux the PC monitor is usually called the console and has several device special files associated
	//     with it: vc/0 (tty0), vc/1 (tty1), vc/2 (tty2), etc. When you log in you are on vc/1. To go to vc/2
	//     (on the same screen) press down the 2 keys Alt(left)-F3. For vc/3 use Left Alt-F3, etc. These (vc/1,
	//     vc/2, vc/3, etc.) are called "virtual terminals". vc/0 (tty0) is just an alias for the current virtual
	//     terminal and it's where messages from the system are sent. Thus messages from the system will be seen
	//     on the console (monitor) regardless of which virtual terminal it is displaying."
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

	// "VT_OPENQRY
	//     This call is used to find an available VT. The argument 
	//     to the ioctl is a pointer to an integer. The integer
	//     will be filled in with the number of the first avail-
	//     able VT that no other process has open (and hence, is
	//     available to be opened). If there are no available
	//     VTs, then -1 will be filled in."
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

	// "VT_ACTIVATE
	//    This call has the effect of making the VT specified in
	//    the argument the active VT. The VT manager will cause
	//    a switch to occur in the same manner as if a hotkey had
	//    initiated the switch.  If the specified VT is not open
	//    or does not exist the call will fail and errno will be
	//    set to ENXIO."
	//
	// "VT_WAITACTIVE
	//    If the specified VT is already active, this call
	//    returns immediately. Otherwise, it will sleep until
	//    the specified VT becomes active, at which point it will
	//    return."
	const int activate_tty_error = ioctl(vt->tty, VT_ACTIVATE, vt->tty_ndx);
	const char* activate_tty_error_str = "Could not activate tty";

	error_str = getErrorString(error, error_str, activate_tty_error, activate_tty_error_str);
	error = error | activate_tty_error;

	const int waitactive_tty_error = ioctl(vt->tty, VT_WAITACTIVE, vt->tty_ndx);
	const char* waitactive_tty_error_str = "Could not switch to tty";

	error_str = getErrorString(error, error_str, waitactive_tty_error, waitactive_tty_error_str);
	error = error | waitactive_tty_error;

	//  "KDSETMODE
	//   This call is used to set the text/graphics mode to the VT.
	//
	//      KD_TEXT indicates that console text will be displayed on the screen
	//      with this VT. Normally KD_TEXT is combined with VT_AUTO mode for
	//      text console terminals, so that the console text display will
	//      automatically be saved and restored on the hot key screen switches.
	//
	//      KD_GRAPHICS indicates that the user/application, usually Xserver,
	//      will have direct control of the display for this VT in graphics
	//      mode. Normally KD_GRAPHICS is combined with VT_PROCESS mode for
	//      this VT indicating direct control of the display in graphics mode.
	//      In this mode, all writes to this VT using the write system call are
	//      ignored, and the user is responsible for saving and restoring the
	//      display on the hot key screen switches."
	// Save the current VT mode. This is most likely KD_TEXT.
	const int kdgetmode_error = ioctl(vt->tty, KDGETMODE, vt->prev_kdmode);
	const char* kdgetmode_error_str = "Could not get mode for tty";

	error_str = getErrorString(error, error_str, kdgetmode_error, kdgetmode_error_str);
	error = error | kdgetmode_error;

	// Set VT to GRAPHICS (user draw) mode

	const int kdsetmode_graphics_error = ioctl(vt->tty, KDSETMODE, KD_GRAPHICS);
	const char* kdsetmode_graphics_error_str = "Could not set graphics mode for tty";

	error_str = getErrorString(error, error_str, kdsetmode_graphics_error, kdsetmode_graphics_error_str);
	error = error | kdsetmode_graphics_error;

	// If vt blanking is active, for example when running this program from a remote terminal, 
	// setting KD_GRAPHICS will not disable the blanking. Reset to KD_TEXT from KD_GRAPHICS will
	// force disable blanking. Then return to KD_GRAPHICS for drawing.
	//
	// Note: KD_TEXT (default) to KD_TEXT will do nothing, so blanking will not be disable unless
	// the mode is changing. i.e. the initial set to KD_GRAPHICS above is useful.
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
		LOGGER(" ERROR in vt_graphics_open: %s", error_str);
		return (-1);
	}

	return (0);
}

int closeConsole(cp_vt* vt) 
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
		LOGGER(" ERROR in vt_close: %s", error_str);
		return (-1);
	}

	return (0);
}

char* getTimeToString() 
{
	time_t t;
	char *buf;

	time(&t);
	buf = (char*) malloc(strlen(ctime(&t)) + 1);

	snprintf(buf, strlen(ctime(&t)), "%s ", ctime(&t));

	return buf;
}

void logger(char* filename, int line, char *fmt, ...) 
{
	va_list list;
	char *p, *r;
	int e;
	FILE *fp;

	if (!(fp = fopen(fileLog, "a"))) return;

	fprintf(fp, "%s [%d] [%s: %d]: ", getTimeToString(), getpid(), filename, line);

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

					fprintf(fp, "%s", r);
					continue;
				}
				case 'd': /* integer */
				{
					e = va_arg(list, int);

					fprintf(fp, "%d", e);
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
}

/** 
 * Detecting whether base is starts with str
 */
bool startsWith(char* base, char* str) 
{
	return (strstr(base, str) - base) == 0;
}

/** 
 * Detecting whether base is ends with str
 */
bool endsWith(char* base, char* str) 
{
	int blen = strlen(base);
	int slen = strlen(str);

	return (blen >= slen) && (0 == strcmp(base + blen - slen, str));
}

int indexOf_shift(char* base, char* str, int startIndex) 
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
 * Getting the first index of str in base
 */
int indexOf(char* base, char* str) 
{
	return indexOf_shift(base, str, 0);
}

/** 
 * Use two index to search in two part to prevent the worst case
 * (assume search 'aaa' in 'aaaaaaaa', you cannot skip three char each time)
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
			start = indexOf_shift(base, str, start);
			end = indexOf_shift(base, str, end);

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
 * Remove white spaces from beginning and from the end of an input string 
 * (referred by a dynamic pointer)
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

int min(int a, int b)
{
	if (a < b) return a;
		else return b;
}

int max(int a, int b)
{
	if (a > b) return a;
		else return b;
}