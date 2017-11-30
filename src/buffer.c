
/**
 * Clue Media Experience 
 * buffer.c - implementation of frame buffer functions
 */


#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <linux/fb.h>
#include <sys/mman.h>

#include "buffer.h"


static unsigned short ored[256], ogreen[256], oblue[256];
static struct fb_cmap ocmap = {0, 256, ored, ogreen, oblue};


/**
 * Open frame buffer console/device and initializing pointers and structures
 * for graphical manipulation
 */
void OpenBuffer(void)
{
	unsigned long page_mask;

	if(getenv("FRAMEBUFFER"))
	{
		if (!(fbs.fbf = fopen(getenv("FRAMEBUFFER"), "w+")))
		{
			ERROR("Cannot open FrameBuffer device: %s", getenv("FRAMEBUFFER"));
			exit(1);
		}		
	}
	else
	{
		if (!(fbs.fbf = fopen(DEF_DEV, "w+")))
		{
			ERROR("Cannot open FrameBuffer device: %s", DEF_DEV);
			exit(1);
		}		
	}

	if (ioctl(fileno(fbs.fbf), FBIOGET_VSCREENINFO, &fbs.oinfo) == -1)
	{
		ERROR("Error reading variable information");
		exit(3);
	}

	if (ioctl(fileno(fbs.fbf), FBIOGET_FSCREENINFO, &fbs.finfo) == -1)
	{
		ERROR("Error reading fixed information");
		exit(2);
	}

	if (fbs.oinfo.bits_per_pixel == 8 || fbs.finfo.visual == FB_VISUAL_DIRECTCOLOR)
	{
		if (ioctl(fileno(fbs.fbf), FBIOGETCMAP, &ocmap) == -1)
		{
			ERROR("ERROR: ioctl FBIOGETCMAP");
			exit(1);
		}
	}

	if (ioctl(fileno(fbs.fbf), FBIOGET_VSCREENINFO, &fbs.vinfo) == -1)
	{
		ERROR("ERROR: ioctl FBIOGET_VSCREENINFO");
		exit(1);
	}

	if (ioctl(fileno(fbs.fbf), FBIOGET_FSCREENINFO, &fbs.finfo) == -1)
	{
		ERROR("ERROR: ioctl FBIOGET_FSCREENINFO");
		exit(1);
	}

	page_mask = getpagesize() - 1;
	fbs.fboffset = (unsigned long) (fbs.finfo.smem_start) & page_mask;
	fbs.fbp = (char *) mmap(0, fbs.finfo.smem_len + fbs.fboffset, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(fbs.fbf), 0);

	if ((int) fbs.fbp == -1)
	{
		ERROR("Failed to map FrameBuffer device to memory");
		exit(4);
	}
}

/**
 * Close frame buffer console/device and dropping all the pointers and structures
 * used by graphical objects.
 */
void CloseBuffer(void)
{
	if (fbs.fbf) 
	{
		fclose(fbs.fbf);
	}
}

/**
 * Apply black color to the frame console
 */
void ResetScreen(void)
{
	if (fbs.fbp)
	{
		unsigned int i, *p, c = 0;
		size_t len = fbs.finfo.line_length * fbs.vinfo.yres;

		i = (c & 0xff) << 8;
		i |= i << 16;
		len >>= 2;

		for (p = fbs.fbp + fbs.fboffset; len--; p++) *p = i;
	}	
}

void SetPosition(int *x, int *y, int width, int height)
{
	int ratio = 1, parts = 1;
	
	if(*x <= 0)
	{
		if(*x >=-2 || *x <= -100000) ratio = 2;
		else if(*x >= -10 && *x < -2) ratio = abs(*x);
		else if(*x > -1000 && *x < -10)
		{
			ratio = (int) abs(*x) % 10;
			parts = (int) abs(*x) / 10;
		}
		else if(*x > -100000 && *x < -1000)
		{
			ratio = (int) abs(*x) % 10;
			parts = (int) abs(*x) / 10;
			
			ratio = ratio + 10*((int) parts % 10);
			parts = (int) parts / 10;
		}
			
		if(width < fbs.vinfo.xres)
			*x = parts * (fbs.vinfo.xres - width) / ratio;
		else
			*x = 0;
	}

	if(*y <= 0)
	{
		if(*y >=-2 || *y <= -100000) ratio = 2;
		else if(*y >= -10 && *y < -2) ratio = abs(*y);
		else if(*y > -1000 && *y < -10)
		{
			ratio = (int) abs(*y) % 10;
			parts = (int) abs(*y) / 10;
		}
		else if(*y > -100000 && *y < -1000)
		{
			ratio = (int) abs(*y) % 10;
			parts = (int) abs(*y) / 10;
			
			ratio = ratio + 10*((int) parts % 10);
			parts = (int) parts / 10;
		}

		if(height < fbs.vinfo.yres)
			*y = parts * (fbs.vinfo.yres - height) / ratio;
		else
			*y = 0;
	}
}
