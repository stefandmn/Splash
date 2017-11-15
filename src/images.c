
/**
 * Clue Media Experience 
 * images.c - implementation of image functions
 */

#include "images.h"


CmdData SetImageProps(CmdData data)
{
	char tmpval[255];
	
	// image file
	if (strcmp(data.value, "") == 0)
	{
		strcpy(data.value, DEF_IMG);
	}
	else
	{
		if(access(data.value, F_OK ) != -1)
		{
			strcpy(data.value, realpath(data.value, NULL));
		}
		else
		{
			if(!startsWith(data.value, "/"))
			{
				if(!startsWith(data.value, "images/"))
				{
					strcpy(tmpval, data.value);
					strcpy(data.value, RESPATH);
					strcpy(data.value, "images/");
					strcat(data.value, tmpval);
				}
				else
				{
					strcpy(tmpval, data.value);
					strcpy(data.value, RESPATH);
					strcat(data.value, tmpval);
				}
			}

			//check if the image has supported extention (PNG)
			if(!endsWith(data.value, ".png")) strcat(data.value, ".png");
		}
	}
	
	return data;	
}

void DrawImage(CmdData data)
{

}

void DrawJpgImage(CmdData data)
{
	
}

void DrawPngImage(CmdData data)
{
	
}

void DrawBmpImage(CmdData data)
{
	
}