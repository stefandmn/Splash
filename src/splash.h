/**
 * Clue Splash - Graphical term for representation of boot process
 *
 * File: splash.h
 * Headers for application functions
 */

#include <signal.h>
#include "uthash.h"

#define DEF_TTF		"fonts/regular.ttf"
#define DEF_IMG		"images/splash.jpg"
#define DEF_SIZE	23

#define DEF_READY	 1
#define DEF_INPUT	 0
#define DEF_EMPTY	-1

//Arguments
typedef struct ArgData
{
	int id;
	char cmd[255];
	int status;
	char data[255];
	int pointx;
	int pointy;
	int fsize;
	char fname[255];
	int delay;
	int reset;	
} ArgData;

//Resources
typedef struct ResFile
{
	char name[255];
	unsigned long size;
	unsigned char *data;
} ResFile;

ResFile getResourceFile(char *filename);

void setFrameBuffer(void);
void setBlackScreen(void);
void closeFrameBuffer(void);

unsigned short int createColor16bit(int r, int g, int b);
void drawPixel(int x, int y, unsigned short int color);
void drawJPEG(int xc, int yc, char *filename);
void drawText(int xc, int yc, char *font, int size, char *text, unsigned short int color, double spacing, int decoration);

ArgData parse(ArgData data);
ArgData getArg(int argc, char argv[]);
ArgData getMem(void);
void setMem(ArgData data);
void delMem(void);
bool isMemSet(void);
