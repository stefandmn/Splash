
/**
 * Clue Media Experience 
 * text.c - implementation of font and text drawing functions
 */


#include <stdio.h>
#include <linux/fb.h>
#include <ft2build.h>
#include <freetype.h>

#include "text.h"


CmdData SetTextProps(CmdData data)
{
	char value[1000], tmpval[255];
	
	//font name/file property
	if(hasDataProperty(data, "fontfile"))
	{
		strcpy(tmpval, getDataProperty(data, "fontfile"));

		if(strcmp(tmpval, "") > 0)
		{
			if(access(tmpval, F_OK ) != -1 )
			{
				strcpy(value, realpath(tmpval, NULL));
			}
			else
			{
				if(!startsWith(tmpval, "/") && !startsWith(tmpval, "./"))
				{
					if(!startsWith(tmpval, "fonts/"))
					{
						strcpy(value, RESPATH);
						strcat(value, "/fonts/");
						strcat(value, tmpval);
					}
					else
					{
						strcpy(value, RESPATH);
						strcat(value, "/");
						strcat(value, tmpval);
					}
				}
				else
				{
					strcat(value, tmpval);
				}

				//check if the font has supported extention (TTF)
				if(!endsWith(value, ".ttf")) strcat(value, ".ttf");
			}			
		}
		else 
		{
			strcpy(value, RESPATH);
			strcat(value, "/");
			strcat(value, DEFTXT_FPATH);
		}

		data = setDataProperty(data, "fontfile", value);
	}
	else 
	{
		strcpy(value, RESPATH);
		strcat(value, "/");
		strcat(value, DEFTXT_FPATH);
			
		data = setDataProperty(data, "fontfile", value);
	}

	//font size
	if(hasDataProperty(data, "fontsize"))
	{
		strcpy(value, getDataProperty(data, "fontsize"));
		if(strcmp(value, "") == 0) data = setIntDataProperty(data, "fontsize", DEFTXT_SIZE);
	}
	else data = setIntDataProperty(data, "fontsize", DEFTXT_SIZE);

	//font color
	if(hasDataProperty(data, "fontcolor"))
	{
		strcpy(value, getDataProperty(data, "fontcolor"));
		if(strcmp(value, "") == 0) data = setDataProperty(data, "fontcolor", DEFTXT_COLOR);
	}
	else data = setDataProperty(data, "fontcolor", DEFTXT_COLOR);

	//font spacing
	if(hasDataProperty(data, "fontspacing"))
	{
		strcpy(value, getDataProperty(data, "fontspacing"));
		if(strcmp(value, "") == 0) data = setIntDataProperty(data, "fontspacing", DEFTXT_SPACE);
	}
	else data = setIntDataProperty(data, "fontspacing", DEFTXT_SPACE);

	return data;
}

/**
 * Draw a text message using specific coordinates
 * 
 * @param data command data structure containing text message and coordinates
 */
void DrawText(CmdData data)
{
	DEBUG("Drawing Text message from command: [%s] >> x=%d, y=%d, font=%s, size=%d, color=%d, spacing=%d",
			data.value, data.xpoint, data.ypoint,
			getDataProperty(data, "fontfile"),
			getIntDataProperty(data, "fontsize"),
			getColorDataProperty(data, "fontcolor"),
			getIntDataProperty(data, "fontspacing") );

	__drawText(data.value, data.xpoint, data.ypoint,
			getDataProperty(data, "fontfile"),
			getIntDataProperty(data, "fontsize"),
			getColorDataProperty(data, "fontcolor"),
			getIntDataProperty(data, "fontspacing") );
}

/**
 * Remove a text message using specific coordinates
 * 
 * @param data command data structure containing text message and coordinates
 */
void OverdrawText(CmdData data)
{
	DEBUG("Overdrawing Text message for removal: [%s] >> x=%d, y=%d,", data.value, data.xpoint, data.ypoint);
	
	__drawText(data.value, data.xpoint, data.ypoint,
			getDataProperty(data, "fontfile"),
			getIntDataProperty(data, "fontsize"),
			getColorFromRGBA(0, 0, 0, 255),
			getIntDataProperty(data, "fontspacing"),
			getIntDataProperty(data, "fontdecoration") );
}

/**
 * Draw a text message using specific coordinates
 * 
 * @param text text message
 * @param xc horizontal position
 * @param yc vertical position
 * @param fontfile font file to be used to draw the letters
 * @param size the size of the font
 * @param rgba the text color
 * @param spacing spacing between letters
 */
void __drawText(char *text, int xc, int yc, char *fontfile, int size, int32_t rgba, int spacing)
{
	int i=0, n, y, x, id, rc;
	int charWidth = 0, charHeight = 0, textWidth = 0;
	
	struct stat fileinfo;
	unsigned long fontsize;
	unsigned char *fontdata;

	if(fontfile != NULL)
	{
		rc = stat(fontfile, &fileinfo);

		if (rc)
		{
			ERROR("Can not open resource file: %s", fontfile);
			fontdata = NULL;
			fontsize = -1;
		}
		else
		{
			fontsize = fileinfo.st_size;
			fontdata = (unsigned char*) malloc(fontsize + 100);
			int fd = open(fontfile, O_RDONLY);

			while (i < fontsize)
			{
				rc = read(fd, fontdata + i, fontsize - i);
				i += rc;
			}

			close(fd);
		}
	}
	else
	{
		ERROR("Invalid font file path: %s", fontfile);		
		fontdata = NULL;
		fontsize = -1;
	}	

	if(fontsize > 0 && fontdata!= NULL)
	{
		FT_Library library;
		FT_Face face;
		FT_GlyphSlot slot;
		FT_Matrix matrix;
		FT_Vector pen;
		FT_Error error;
		FT_Glyph_Metrics *metrics;
		int tsize = strlen(text);

		FT_Init_FreeType(&library);
		FT_New_Memory_Face(library, fontdata, fontsize, 0, &face);
		FT_Set_Char_Size(face, size * 64, 0, 72, 0);
		FT_Set_Pixel_Sizes(face, 0, size);
		FT_UInt glyph_index;
	
		for (n = 0; n < tsize; n++)
		{
			glyph_index = FT_Get_Char_Index(face, text[n]);
			FT_Load_Glyph(face, glyph_index, 0);
			metrics = &face->glyph->metrics;
			FT_Render_Glyph(face->glyph, ft_render_mode_normal);

			if ((metrics->horiBearingY / 64) > charHeight)
			{
				charHeight = (metrics->horiBearingY / 64);
			}

			if (face->glyph->bitmap.width > charWidth)
			{
				charWidth = face->glyph->bitmap.width;
			}
			
			if (text[n] == ' ')
			{
				textWidth += charWidth;
			}
			else textWidth += face->glyph->bitmap.width + spacing;
		}

		//adapt text position
		SetPosition(&xc, &yc, textWidth, charHeight);

		//detect text size for potential trim and adjust text width
		if(xc + textWidth > fbs.vinfo.xres)
		{
			tsize = tsize*(fbs.vinfo.xres-xc)/textWidth;
			text[tsize-2] = '.';
			text[tsize-1] = '.';
			textWidth = fbs.vinfo.xres-xc;
		}
		
		DEBUG("Preparing Message buffer at %dx%d, with dimension %dx%d", xc, yc, charWidth, charHeight);
		yc += charHeight;

		for (n = 0; n < tsize; n++)
		{
			if (text[n] == ' ')
			{
				xc += charWidth;
			}
			else
			{
				glyph_index = FT_Get_Char_Index(face, text[n]);
				FT_Load_Glyph(face, glyph_index, 0);
				metrics = &face->glyph->metrics;
				FT_Render_Glyph(face->glyph, ft_render_mode_normal);

				for (y = 0; y < face->glyph->bitmap.rows; y++)
				{
					for (x = 0; x < face->glyph->bitmap.width; x++)
					{
						if (face->glyph->bitmap.buffer[y * face->glyph->bitmap.width + x] > 0)
						{
							if (x + xc >= 0 && y + yc - (metrics->horiBearingY / 64) >= 0)
							{
								long int location = (x + xc + fbs.vinfo.xoffset) * (fbs.vinfo.bits_per_pixel / 8) + (y + yc - (metrics->horiBearingY / 64) + fbs.vinfo.yoffset) * fbs.finfo.line_length;
								SetColor(location, rgba);								
							}
						}
					}
				}

				xc += face->glyph->bitmap.width + spacing;
			}
		}

		free(fontdata);
		FT_Done_Face(face);
		FT_Done_FreeType(library);
	}
	
	if(fontfile != NULL) free(fontfile);
}

