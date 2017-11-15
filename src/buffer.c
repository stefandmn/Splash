
/**
 * Clue Media Experience 
 * buffer.c - implementation of frame buffer functions
 */


#include <stdio.h>
#include <stdbool.h>
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
		if (!(fbs.fbfd = fopen(getenv("FRAMEBUFFER"), "w+")))
		{
			ERROR("Cannot open FrameBuffer device: %s", getenv("FRAMEBUFFER"));
			exit(1);
		}		
	}
	else
	{
		if (!(fbs.fbfd = fopen(DEF_DEV, "w+")))
		{
			LOGGER("Cannot open FrameBuffer device: %s", DEF_DEV);
			exit(1);
		}		
	}

	if (ioctl(fileno(fbs.fbfd), FBIOGET_VSCREENINFO, &fbs.oinfo) == -1)
	{
		ERROR("Error reading variable information");
		exit(3);
	}

	if (ioctl(fileno(fbs.fbfd), FBIOGET_FSCREENINFO, &fbs.finfo) == -1)
	{
		ERROR("Error reading fixed information");
		exit(2);
	}

	if (fbs.oinfo.bits_per_pixel == 8 || fbs.finfo.visual == FB_VISUAL_DIRECTCOLOR)
	{
		if (ioctl(fileno(fbs.fbfd), FBIOGETCMAP, &ocmap) == -1)
		{
			ERROR("ERROR: ioctl FBIOGETCMAP");
			exit(1);
		}
	}

	if (ioctl(fileno(fbs.fbfd), FBIOGET_VSCREENINFO, &fbs.vinfo) == -1)
	{
		ERROR("ERROR: ioctl FBIOGET_VSCREENINFO");
		exit(1);
	}

	if (ioctl(fileno(fbs.fbfd), FBIOGET_FSCREENINFO, &fbs.finfo) == -1)
	{
		ERROR("ERROR: ioctl FBIOGET_FSCREENINFO");
		exit(1);
	}

	page_mask = getpagesize() - 1;
	fbs.fboffset = (unsigned long) (fbs.finfo.smem_start) & page_mask;
	fbs.fbp = (char *) mmap(0, fbs.finfo.smem_len + fbs.fboffset, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(fbs.fbfd), 0);

	if ((int) fbs.fbp == -1)
	{
		LOGGER("ERROR: failed to map FrameBuffer device to memory");
		exit(4);
	}
}

/**
 * Close frame buffer console/device and dropping all the pointers and structures
 * used by graphical objects.
 */
void CloseBuffer(void)
{
	if (fbs.fbfd) 
	{
		fclose(fbs.fbfd);
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
