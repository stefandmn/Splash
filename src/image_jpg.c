
/**
 * Clue Media Experience 
 * image_jpg.c - implementation of JPG image format functions
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <jpeglib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <setjmp.h>
#include <unistd.h>

#include "images.h"

struct JpgError
{
	struct jpeg_error_mgr pub;
	jmp_buf envbuffer;
};

void JpgErrorExit(j_common_ptr cinfo)
{
	struct JpgError *mptr;
	mptr = (struct JpgError*) cinfo->err;
	(*cinfo->err->output_message) (cinfo);
	longjmp(mptr->envbuffer, 1);
}

bool isJpgImage(char *imgfile)
{
	int fd;
	unsigned char id[10];
	
	fd = open(imgfile, O_RDONLY);
	if (fd == -1) return false;
	read(fd, id, 10);
	close(fd);
	
	if (id[6] == 'J' && id[7] == 'F' && id[8] == 'I' && id[9] == 'F') return true;
	if (id[0] == 0xff && id[1] == 0xd8 && id[2] == 0xff) return true;
	
	return false;
}

int GetJpgImageSize(char *imgfile, int *x, int *y)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_decompress_struct *ciptr;
	struct JpgError emgr;
	int px, py;
	FILE *fh;

	ciptr = &cinfo;
	if (!(fh = fopen(imgfile, "rb"))) return STATUS_ACCESS_ERR;

	ciptr->err = jpeg_std_error(&emgr.pub);
	emgr.pub.error_exit = JpgErrorExit;
	
	if (setjmp(emgr.envbuffer))
	{
		jpeg_destroy_decompress(ciptr);
		fclose(fh);
		return STATUS_FORMAT_ERR;
	}

	jpeg_create_decompress(ciptr);
	jpeg_stdio_src(ciptr, fh);
	jpeg_read_header(ciptr, TRUE);
	ciptr->out_color_space = JCS_RGB;
	jpeg_start_decompress(ciptr);
	px = ciptr->output_width;
	py = ciptr->output_height;
	*x = px;
	*y = py;
	jpeg_destroy_decompress(ciptr);
	fclose(fh);
	
	return STATUS_OK;
}

int GetJpgImageBuffer(char *imgfile, unsigned char *buffer, unsigned char ** alpha, int x, int y)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_decompress_struct *ciptr;
	struct JpgError emgr;
	unsigned char *bp;
	int px, c;
	FILE *fh;
	JSAMPLE *lb;

	ciptr = &cinfo;
	if (!(fh = fopen(imgfile, "rb"))) return STATUS_ACCESS_ERR;
	ciptr->err = jpeg_std_error(&emgr.pub);
	emgr.pub.error_exit = JpgErrorExit;
	
	if (setjmp(emgr.envbuffer))
	{
		jpeg_destroy_decompress(ciptr);
		fclose(fh);
		return STATUS_FORMAT_ERR;
	}

	jpeg_create_decompress(ciptr);
	jpeg_stdio_src(ciptr, fh);
	jpeg_read_header(ciptr, TRUE);
	ciptr->out_color_space = JCS_RGB;
	jpeg_start_decompress(ciptr);

	px = ciptr->output_width;
	c = ciptr->output_components;

	if (c == 3)
	{
		lb = (JSAMPLE*) (*ciptr->mem->alloc_small)((j_common_ptr) ciptr, JPOOL_PERMANENT, c * px);
		bp = buffer;
		while (ciptr->output_scanline < ciptr->output_height)
		{
			jpeg_read_scanlines(ciptr, &lb, 1);
			memcpy(bp, lb, px * c);
			bp += px*c;
		}
	}

	jpeg_finish_decompress(ciptr);
	jpeg_destroy_decompress(ciptr);
	
	fclose(fh);
	return STATUS_OK;
}
