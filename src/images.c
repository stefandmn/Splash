
/**
 * Clue Media Experience 
 * images.c - implementation of image functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "images.h"
#include "buffer.h"
#include "utils.h"


__u16 red[256], green[256], blue[256];
struct fb_cmap map332 = {0, 256, red, green, blue, NULL};
__u16 red_b[256], green_b[256], blue_b[256];
struct fb_cmap map_back = {0, 256, red_b, green_b, blue_b, NULL};


CmdData SetImageProps(CmdData data)
{
	char tmpval[255];
	
	// image file
	if (strcmp(data.value, "") == 0)
	{
		strcpy(data.value, RESPATH);
		strcat(data.value, "/");
		strcat(data.value, DEF_IMG);
	}
	else
	{
		if(access(data.value, F_OK ) != -1)
		{
			strcpy(data.value, realpath(data.value, NULL));
		}
		else
		{
			if(!startsWith(data.value, "/") && !startsWith(data.value, "./"))
			{
				if(!startsWith(data.value, "images/"))
				{
					strcpy(tmpval, data.value);
					strcpy(data.value, RESPATH);
					strcat(data.value, "/images/");
					strcat(data.value, tmpval);
				}
				else
				{
					strcpy(tmpval, data.value);
					strcpy(data.value, RESPATH);
					strcat(data.value, "/");
					strcat(data.value, tmpval);
				}
			}

			//check if the image has supported extention (PNG)
			if(!endsWith(data.value, ".png") && !endsWith(data.value, ".jpg") && !endsWith(data.value, ".bmp")) strcat(data.value, ".png");
		}
	}
	
	// rotate
	if(hasDataProperty(data, "rotate"))
	{
		strcpy(tmpval, getDataProperty(data, "rotate"));
		if(strcmp(tmpval, "") == 0) data = setIntDataProperty(data, "rotate", 0);
	}
	else data = setIntDataProperty(data, "rotate", 0);
	
	// enlarge
	if(hasDataProperty(data, "enlarge"))
	{
		strcpy(tmpval, getDataProperty(data, "enlarge"));
		if(strcmp(tmpval, "") == 0) data = setBoolDataProperty(data, "enlarge", false);
	}
	else data = setBoolDataProperty(data, "enlarge", false);

	// stretch
	if(hasDataProperty(data, "stretch"))
	{
		strcpy(tmpval, getDataProperty(data, "stretch"));
		if(strcmp(tmpval, "") == 0) data = setBoolDataProperty(data, "stretch", false);
	}
	else data = setBoolDataProperty(data, "stretch", false);
	
	return data;	
}

void DrawImage(CmdData data)
{
	DEBUG("Drawing Image from command: [%s] >> x=%d, y=%d, rotate=%d, enlarge=%d, stretch=%d",
			data.value, data.xpoint, data.ypoint,
			getIntDataProperty(data, "rotate"),
			getIntDataProperty(data, "enlarge"),
			getIntDataProperty(data, "stretch"));

	__drawImage(data.value, data.xpoint, data.ypoint,
			getIntDataProperty(data, "rotate"),
			getIntDataProperty(data, "stretch"),
			getIntDataProperty(data, "enlarge"), false);
}

void OverdrawImage(CmdData data)
{
	DEBUG("Overdrawing Image for removal: [%s] >> x=%d, y=%d, rotate=%d, enlarge=%d, stretch=%d",
			data.value, data.xpoint, data.ypoint,
			getIntDataProperty(data, "rotate"),
			getIntDataProperty(data, "enlarge"),
			getIntDataProperty(data, "stretch"));

	__drawImage(data.value, data.xpoint, data.ypoint,
			getIntDataProperty(data, "rotate"),
			getIntDataProperty(data, "stretch"),
			getIntDataProperty(data, "enlarge"), true);
}

int GetImageSize(char *imgfile, int *x, int *y)
{
	if(isPngImage(imgfile)) return GetPngImageSize(imgfile, x, y);
	else if(isJpgImage(imgfile)) return GetJpgImageSize(imgfile, x, y);
	else if(isBmpImage(imgfile)) return GetBmpImageSize(imgfile, x, y);
}

int GetImageBuffer(char *imgfile, unsigned char *buffer, unsigned char **alpha, int x,int y)
{
	if(isPngImage(imgfile)) return GetPngImageBuffer(imgfile, buffer, alpha, x, y);
	else if(isJpgImage(imgfile)) return GetJpgImageBuffer(imgfile, buffer, alpha, x, y);
	else if(isBmpImage(imgfile)) return GetBmpImageBuffer(imgfile, buffer, alpha, x, y);
}

void __drawImage(char *imgfile, int x_offs, int y_offs, int rotate, bool stretch, bool enlarge, bool cleanup)
{
	struct ImageInfo imginfo;
	unsigned char *image = NULL;
	unsigned char *alpha = NULL;
	unsigned char *fbbuff = NULL;
	
	int x_size, y_size, screen_width, screen_height;
	int x_pan = 0, y_pan = 0;
	int xc, yc;
	
	unsigned long count;
	unsigned long i;
	int bp = 0;

	u_int8_t  *c_fbbuff;
	u_int16_t *s_fbbuff;
	u_int32_t *i_fbbuff;
	
	unsigned char *fbptr;
	unsigned char *imptr;	

	unsigned int x_stride = (fbs.finfo.line_length * 8) / fbs.vinfo.bits_per_pixel;	


	if(GetImageSize(imgfile, &x_size, &y_size) == STATUS_OK)
	{
		if(!(image = (unsigned char*) malloc(x_size * y_size * 3)))
		{
			ERROR("Out of memory: %s", imgfile);
			goto error;
		}
		
		DEBUG("Detecting image size: %dx%d", x_size, y_size);

		if(GetImageBuffer(imgfile, image, &alpha, x_size, y_size) != STATUS_OK)
		{
			ERROR("Image data is corrupted: %s", imgfile);
			goto error;
		}
		
		DEBUG("Reading image buffer: %s", imgfile);
	}
	else goto error;

	screen_width = fbs.vinfo.xres;
	screen_height = fbs.vinfo.yres;
	imginfo.do_free = 0;
	
	if(imginfo.do_free)
	{
		free(imginfo.rgb);
		free(imginfo.alpha);
	}

	imginfo.width = x_size;
	imginfo.height = y_size;
	imginfo.rgb = image;
	imginfo.alpha = alpha;
	imginfo.do_free = 0;

	if(rotate)
	{
		DEBUG("Applying image rotate: %dx", rotate);
		doRotate(&imginfo, rotate);
	}

	if(stretch)
	{
		DEBUG("Displaying stretched image in full screen: %dx%d", screen_width, screen_height);
		doEnlarge(&imginfo, screen_width, screen_height, 1);
	}
	else if(enlarge)
	{
		DEBUG("Displaying enlarged image in full screen: %dx%d", screen_width, screen_height);
		doEnlarge(&imginfo, screen_width, screen_height, 0);
	}

	if(x_offs == 0)
	{
		if(imginfo.width < screen_width)
			x_offs = (screen_width - imginfo.width) / 2;
		else
			x_offs = 0;
	}

	if(y_offs == 0)
	{
		if(imginfo.height < screen_height)
			y_offs = (screen_height - imginfo.height) / 2;
		else
			y_offs = 0;
	}

	/* correct panning */
	if(x_pan > imginfo.width - x_stride) x_pan = 0;
	if(y_pan > imginfo.height - fbs.vinfo.yres) y_pan = 0;

	/* correct offset */
	if(x_offs + imginfo.width > x_stride) x_offs = 0;
	if(y_offs + imginfo.height > fbs.vinfo.yres) y_offs = 0;
	
	if(!cleanup)
	{
		count = imginfo.width * imginfo.height;
		DEBUG("Preparing Image buffer at %dx%d, with dimension %dx%d", x_offs, y_offs, imginfo.width, imginfo.height);

		switch(fbs.vinfo.bits_per_pixel)
		{
			case 8:
				bp = 1;
				c_fbbuff = (unsigned char *) malloc(count * sizeof(unsigned char));
				for(i = 0; i < count; i++)
					c_fbbuff[i] = (((imginfo.rgb[i*3] >> 5) & 7) << 5) | 
						(((imginfo.rgb[i*3+1] >> 5) & 7) << 2) | 
						((imginfo.rgb[i*3+2] >> 6) & 3);
				fbbuff = (char *) c_fbbuff;
				break;
			case 15:
				bp = 2;
				s_fbbuff = (unsigned short *) malloc(count * sizeof(unsigned short));
				for(i = 0; i < count ; i++)
					s_fbbuff[i] = (((imginfo.rgb[i*3] >> 3) & 31) << 10) | 
						(((imginfo.rgb[i*3+1] >> 3) & 31) << 5) | 
						((imginfo.rgb[i*3+2] >> 3) & 31);
				fbbuff = (char *) s_fbbuff;
				break;
			case 16:
				bp = 2;
				s_fbbuff = (unsigned short *) malloc(count * sizeof(unsigned short));
				for(i = 0; i < count ; i++)
					s_fbbuff[i] = (((imginfo.rgb[i*3] >> 3) & 31) << 11) | 
						(((imginfo.rgb[i*3+1] >> 2) & 63) << 5) | 
						((imginfo.rgb[i*3+2] >> 3) & 31);
				fbbuff = (char *) s_fbbuff;
				break;
			case 24:
			case 32:
				bp = 4;
				i_fbbuff = (unsigned int *) malloc(count * sizeof(unsigned int));
				for(i = 0; i < count ; i++)
					i_fbbuff[i] = ((imginfo.rgb[i*3] << 16) & 0xFF0000) | 
							((imginfo.rgb[i*3+1] << 8) & 0xFF00) | 
							(imginfo.rgb[i*3+2] & 0xFF);
				fbbuff = (char *) i_fbbuff;
				break;
			default:
				ERROR("Unsupported video mode! You've got: %d bpp", fbs.vinfo.bits_per_pixel);
				goto error;
		}

		xc = (imginfo.width > x_stride) ? x_stride : imginfo.width;
		yc = (imginfo.height > fbs.vinfo.yres_virtual) ? fbs.vinfo.yres_virtual : imginfo.height;

		if(bp == 1)
		{
			int j;
			int rs, gs, bs;
			int r = 8, g = 8, b = 4;

			DEBUG("Adapting buffer map for low resolution image..");

			if (ioctl(fbs.fbf, FBIOGETCMAP, &map_back) < 0)
			{
				ERROR("Error getting colormap");
				goto error;
			}

			map332.red = red;
			map332.green = green;
			map332.blue = blue;

			rs = 256 / (r - 1);
			gs = 256 / (g - 1);
			bs = 256 / (b - 1);

			for (j = 0; j < 256; j++)
			{
				map332.red[j]   = (rs * ((j / (g * b)) % r)) * 255;
				map332.green[j] = (gs * ((j / b) % g)) * 255;
				map332.blue[j]  = (bs * ((j) % b)) * 255;
			}		

			if (ioctl(fbs.fbf, FBIOPUTCMAP, &map332) < 0)
			{
				ERROR("Error putting colormap");
				goto error;
			}		
		}

		fbptr = fbs.fbp + ((y_offs + fbs.vinfo.yoffset) * x_stride + x_offs) * bp;
		imptr = fbbuff + (y_pan * imginfo.width + x_pan) * bp;

		if(imginfo.alpha)
		{
			unsigned char * alphaptr;
			int from, to, x;

			DEBUG("Applying alpha layer..");
			alphaptr = imginfo.alpha + (y_pan * imginfo.width + x_pan);

			for(i = 0; i < yc; i++, fbptr += x_stride * bp, imptr += imginfo.width * bp, alphaptr += imginfo.width)
			{
				for(x = 0; x<xc; x++)
				{
					int v;

					from = to = -1;
					for(v = x; v<xc; v++)
					{
						if(from == -1)
						{
							if(alphaptr[v] > 0x80) from = v;
						}
						else
						{
							if(alphaptr[v] < 0x80)
							{
								to = v;
								break;
							}
						}
					}

					if(from == -1)
						break;

					if(to == -1) to = xc;

					memcpy(fbptr + (from * bp), imptr + (from * bp), (to - from - 1) * bp);
					x += to - from - 1;
				}
			}
		}
		else
		{
			for(i = 0; i < yc; i++, fbptr += x_stride * bp, imptr += imginfo.width * bp)
			{
				memcpy(fbptr, imptr, xc * bp);
			}
		}

		if(bp == 1)
		{
			if (ioctl(fbs.fbf, FBIOPUTCMAP, map_back) < 0)
			{
				ERROR("Error putting colormap");
				goto error;
			}		
		}

		DEBUG("Image representation has been done, deallocating mapping..");
		munmap(fbs.fbp, x_stride * fbs.vinfo.yres_virtual * bp);
	}
	else
	{
		int x, y;
		DEBUG("Overdrawing image buffer at position %dx%d, having dimension %dx%d", x_offs, y_offs, imginfo.width, imginfo.height);

		// Figure out where in memory to put the pixel
		for (y = y_offs; y < y_offs + imginfo.height; y++)
		{
			for (x = x_offs; x < x_offs + imginfo.width; x++)
			{
				// Draw rectangle borders
				if ((y < y_offs) || (y > y_offs + imginfo.height - 1) ||
					(x < x_offs) || (x > x_offs + imginfo.width - 1)) __drawPixel(x, y, 0);

				// Fill rectangle
				if ((y >= y_offs) && (y <= y_offs + imginfo.height - 1) &&
					(x >= x_offs) && (x <= x_offs + imginfo.width - 1)) __drawPixel(x, y, 0);
			}
		}		
	}
	
error:
	if(fbbuff != NULL) free(fbbuff);
	if(image != NULL) free(image);
	if(alpha != NULL) free(alpha);
	
	if(imginfo.do_free)
	{
		free(imginfo.rgb);
		free(imginfo.alpha);
	}

	DEBUG("Image displayed, memory deallocated");
}

unsigned char* SimpleResize(unsigned char * orgin, int ox, int oy, int dx, int dy)
{
	unsigned char *cr, *p, *l;
	int i, j, k, ip;
	
	assert(cr = (unsigned char*)malloc(dx*dy*3));
	l = cr;
	
	for(j=0; j<dy; j++, l+=dx*3)
	{
		p = orgin + (j * oy/dy * ox * 3);
		for(i=0, k=0; i<dx; i++, k+=3)
		{
			ip = i * ox/dx * 3;
			l[k] = p[ip];
			l[k+1] = p[ip+1];
			l[k+2] = p[ip+2];
		}
	}
	
	return cr;
}

unsigned char* AlphaResize(unsigned char * alpha, int ox, int oy, int dx, int dy)
{
	unsigned char *cr,*p,*l;
	int i,j,k;
	
	assert(cr = (unsigned char*)malloc(dx*dy));
	l = cr;
	
	for(j=0; j<dy; j++,l+=dx)
	{
		p = alpha + (j * oy/dy * ox);
		for(i=0,k=0; i<dx; i++)
			l[k++] = p[i*ox/dx];
	}
	
	return cr;
}

unsigned char* SimpleRotate(unsigned char *i, int ox, int oy, int rot)
{
	unsigned char * n, * p;
	int x, y;
	
	assert(n = (unsigned char*) malloc(ox * oy * 3));
	
	switch(rot)
	{
		case 1: /* 90 deg right */
			p = n + (oy - 1) * 3;
			for(y = 0; y<oy; y++, p -= 3)
			{
				unsigned char * r = p;
				for(x = 0; x<ox; x++, r += oy * 3)
				{
					r[0] = *(i++);
					r[1] = *(i++);
					r[2] = *(i++);
				}
			}
			break;
		case 2: /* 180 deg */
			i += ox * oy * 3; p = n;
			for(y = ox * oy; y > 0; y--)
			{
				i -= 3;
				p[0] = i[0];
				p[1] = i[1];
				p[2] = i[2];
				p += 3;
			}
			break;
		case 3: /* 90 deg left */
			p = n;
			for(y = 0; y<oy; y++, p += 3)
			{
				unsigned char * r = p + ((ox * 3) * oy);
				for(x = 0; x<ox; x++)
				{
					r -= oy * 3;
					r[0] = *(i++);
					r[1] = *(i++);
					r[2] = *(i++);
				}
			}
			break;
	}
	
	return n;
}

unsigned char * AlphaRotate(unsigned char *i, int ox, int oy, int rot)
{
	unsigned char * n, * p;
	int x, y;
	assert(n = (unsigned char*) malloc(ox * oy));
	
	switch(rot)
	{
		case 1: /* 90 deg right */
			p = n + (oy - 1);
			for(y = 0; y<oy; y++, p --)
			{
				unsigned char * r = p;
				for(x = 0; x<ox; x++, r += oy)
					*r = *(i++);
			}
			break;
		case 2: /* 180 deg */
			i += ox * oy; p = n;
			for(y = ox * oy; y > 0; y--)
				*(p++) = *(i--);
			break;
		case 3: /* 90 deg left */
			p = n;
			for(y = 0; y<oy; y++, p++)
			{
				unsigned char * r = p + (ox * oy);
				for(x = 0; x<ox; x++)
				{
					r -= oy;
					*r = *(i++);
				}
			}
			break;
	}
	
	return n;
}

void doRotate(struct ImageInfo *i, int rot)
{
	if(rot)
	{
		int t;
		unsigned char *image, *alpha = NULL;

		image = SimpleRotate(i->rgb, i->width, i->height, rot);
		
		if(i->alpha)
		{
			alpha = AlphaRotate(i->alpha, i->width, i->height, rot);
		}
		
		if(i->do_free)
		{
			free(i->alpha);
			free(i->rgb);
		}

		i->rgb = image;
		i->alpha = alpha;
		i->do_free = 1;

		if(rot & 1)
		{
			t = i->width;
			i->width = i->height;
			i->height = t;
		}
	}
}

void doEnlarge(struct ImageInfo *i, int screen_width, int screen_height, int ignoreaspect)
{
	if(((i->width > screen_width) || (i->height > screen_height)) && (!ignoreaspect)) return;
	
	if((i->width < screen_width) || (i->height < screen_height))
	{
		int xsize = i->width, ysize = i->height;
		unsigned char * image, * alpha = NULL;

		if(ignoreaspect)
		{
			if(i->width < screen_width)
				xsize = screen_width;
			if(i->height < screen_height)
				ysize = screen_height;

			goto have_sizes;
		}

		if((i->height * screen_width / i->width) <= screen_height)
		{
			xsize = screen_width;
			ysize = i->height * screen_width / i->width;
			goto have_sizes;
		}

		if((i->width * screen_height / i->height) <= screen_width)
		{
			xsize = i->width * screen_height / i->height;
			ysize = screen_height;
			goto have_sizes;
		}
		
		return;
		
have_sizes:
		image = SimpleResize(i->rgb, i->width, i->height, xsize, ysize);
		if(i->alpha) alpha = AlphaResize(i->alpha, i->width, i->height, xsize, ysize);

		if(i->do_free)
		{
			free(i->alpha);
			free(i->rgb);
		}

		i->rgb = image;
		i->alpha = alpha;
		i->do_free = 1;
		i->width = xsize;
		i->height = ysize;
	}
}
