#ifndef _GLOBAL_H_
	#define _GLOBAL_H_
#endif

#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <locale.h>
#include <ctype.h>
#include <setjmp.h>
#include <stdbool.h>
#include <linux/fb.h>
#include <linux/vt.h>
#include <linux/kd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <getopt.h>
#include <jpeglib.h>
#include <ft2build.h>
#include <libgen.h>
#include <freetype.h>
#include "splash.h"
#include "utils.h"

struct fb_var_screeninfo vinfo;
struct fb_var_screeninfo ovinfo;
struct fb_fix_screeninfo finfo;
static unsigned short ored[256], ogreen[256], oblue[256];
static struct fb_cmap ocmap = {0, 256, ored, ogreen, oblue};
FILE *fbfd;
char *fbp = 0;
int fboffset = 0;
cp_vt vt;

//Paths and files
char PATH[255];
char fileLog[255];


void setFrameBuffer(void)
{
	unsigned long page_mask;

	if (!(fbfd = fopen("/dev/fb0", "w+")))
	{
		LOGGER("ERROR: cannot open FrameBuffer device");
		exit(1);
	}

	if (ioctl(fileno(fbfd), FBIOGET_VSCREENINFO, &ovinfo) == -1)
	{
		LOGGER("ERROR: reading variable information");
		exit(3);
	}

	if (ioctl(fileno(fbfd), FBIOGET_FSCREENINFO, &finfo) == -1)
	{
		LOGGER("ERROR: reading fixed information");
		exit(2);
	}

	if (ovinfo.bits_per_pixel == 8 || finfo.visual == FB_VISUAL_DIRECTCOLOR)
	{
		if (ioctl(fileno(fbfd), FBIOGETCMAP, &ocmap) == -1)
		{
			LOGGER("ERROR: ioctl FBIOGETCMAP");
			exit(1);
		}
	}

	if (ioctl(fileno(fbfd), FBIOGET_VSCREENINFO, &vinfo) == -1)
	{
		LOGGER("ERROR: ioctl FBIOGET_VSCREENINFO");
		exit(1);
	}

	if (ioctl(fileno(fbfd), FBIOGET_FSCREENINFO, &finfo) == -1)
	{
		LOGGER("ERROR: ioctl FBIOGET_FSCREENINFO");
		exit(1);
	}

	page_mask = getpagesize() - 1;
	fboffset = (unsigned long) (finfo.smem_start) & page_mask;
	fbp = (char *) mmap(0, finfo.smem_len + fboffset, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(fbfd), 0);

	if ((int) fbp == -1)
	{
		LOGGER("ERROR: failed to map FrameBuffer device to memory");
		exit(4);
	}
}

void closeFrameBuffer(void)
{
	if (fbfd) fclose(fbfd);
}

void setBlackScreen(void)
{
	fbMemset(fbp + fboffset, 0, finfo.line_length * vinfo.yres);
}

ResFile getResourceFile(char *filename)
{
	struct ResFile file;
	struct stat fileinfo;
	int i, rc;
	
	//get real file name
	if(startsWith(filename, "/")) 
	{
		strcpy(file.name, filename);
	}
	else 
	{
		strcpy(file.name, PATH);
		if(!endsWith(PATH, "/")) strcat(file.name, "/");
		strcat(file.name, filename);
	}
	
	rc = stat(file.name, &fileinfo);
	
	if (rc)
	{
		LOGGER("ERROR: Can not open file : %s", file.name);
		file.size = -1;
		exit(1);
	}
	else
	{
		file.size = fileinfo.st_size;
		file.data = (unsigned char*) malloc(file.size + 100);
		int fd = open(file.name, O_RDONLY);

		i = 0;
		while (i < file.size)
		{
			rc = read(fd, file.data + i, file.size - i);
			i += rc;
		}

		close(fd);
	}

	return file;
}

unsigned short int createColor16bit(int r, int g, int b)
{
	return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}

void drawPixel(int x, int y, unsigned short int color)
{
	long int location;

	if (x < 0 || y < 0) return;

	location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) + (y + vinfo.yoffset) * finfo.line_length;

	if ((fbp + location)) * ((unsigned short int*) (fbp + location)) = color;
}

void drawJPEG(int xc, int yc, char *filename)
{
	int i;
	unsigned long location = 0;
	unsigned char *raw_image = NULL;
	unsigned char a, r, g, b;
	int ir, ig, ib;
	struct jpeg_decompress_struct jpegInfo;
	struct jpeg_error_mgr jpegErr;
	struct ResFile file;

	file = getResourceFile(filename);
	
	if(file.size >= 0)
	{
		JSAMPROW row_pointer[1];
		jpegInfo.err = jpeg_std_error(&jpegErr);
		jpeg_create_decompress(&jpegInfo);
		jpeg_mem_src(&jpegInfo, file.data, file.size);
		jpeg_read_header(&jpegInfo, TRUE);
		jpeg_start_decompress(&jpegInfo);

		raw_image = (unsigned char*) malloc(jpegInfo.output_width * jpegInfo.output_height * jpegInfo.num_components);
		row_pointer[0] = (unsigned char *) malloc(jpegInfo.output_width * jpegInfo.num_components);

		int x = 1, y = 1, line;
		while (jpegInfo.output_scanline < jpegInfo.image_height)
		{
			jpeg_read_scanlines(&jpegInfo, row_pointer, 1);

			for (i = 0; i < jpegInfo.image_width * jpegInfo.num_components; i++)
			{
				raw_image[location++] = row_pointer[0][i];
			}
		}

		for (line = jpegInfo.image_height - 1; line >= 0; line--)
		{
			for (x = 0; x < jpegInfo.image_width; x++)
			{
				b = *(raw_image + (x + line * jpegInfo.image_width) * jpegInfo.num_components + 2);
				g = *(raw_image + (x + line * jpegInfo.image_width) * jpegInfo.num_components + 1);
				r = *(raw_image + (x + line * jpegInfo.image_width) * jpegInfo.num_components + 0);

				drawPixel(x + xc, line + yc, createColor16bit((((int) r & 0xFF) << 0), (((int) g & 0xFF) << 0), (((int) b & 0xFF) << 0)));
			}
		}

		jpeg_finish_decompress(&jpegInfo);
		jpeg_destroy_decompress(&jpegInfo);
		free(row_pointer[0]);
		free(file.data);
		free(raw_image);
	}
}

void drawText(int xc, int yc, char *fontfile, int size, char *text, unsigned short int color, double spacing, int decoration)
{
	int n, y, x, id;
	double z;
	int fontHeight = 0, fontWidth = 0;
	struct ResFile file;

	file = getResourceFile(fontfile);

	if(file.size >= 0)
	{
	
		FT_Library library;
		FT_Face face;
		FT_GlyphSlot slot;
		FT_Matrix matrix;
		FT_Vector pen;
		FT_Error error;
		FT_Glyph_Metrics *metrics;

		FT_Init_FreeType(&library);
		FT_New_Memory_Face(library, file.data, file.size, 0, &face);
		FT_Set_Char_Size(face, size * 64, 0, 72, 0);
		FT_Set_Pixel_Sizes(face, 0, size);
		FT_UInt glyph_index;

		for (n = 0; n < strlen(text); n++)
		{
			glyph_index = FT_Get_Char_Index(face, text[n]);
			FT_Load_Glyph(face, glyph_index, 0);
			metrics = &face->glyph->metrics;
			FT_Render_Glyph(face->glyph, ft_render_mode_normal);

			if ((metrics->horiBearingY / 64) > fontHeight)
			{
				fontHeight = (metrics->horiBearingY / 64);
			}

			if (face->glyph->bitmap.width > fontWidth)
			{
				fontWidth = face->glyph->bitmap.width;
			}
		}

		yc += fontHeight;

		for (n = 0; n <= strlen(text); n++)
		{
			if (text[n] == ' ')
			{
				xc += fontWidth;
			}
			else
			{
				glyph_index = FT_Get_Char_Index(face, text[n]);
				FT_Load_Glyph(face, glyph_index, 0);
				metrics = &face->glyph->metrics;
				FT_Render_Glyph(face->glyph, ft_render_mode_normal);

				if (decoration == 1)
				{
					z = face->glyph->bitmap.rows * 0.25;
				}

				for (y = 0; y < face->glyph->bitmap.rows; y++)
				{
					for (x = 0; x < face->glyph->bitmap.width; x++)
					{
						if (face->glyph->bitmap.buffer[y * face->glyph->bitmap.width + x] > 0)
						{
							drawPixel(x + xc + z, y + yc - (metrics->horiBearingY / 64), color);
						}
					}

					if (decoration == 1)
					{
						z -= 0.25;
					}
				}

				xc += face->glyph->bitmap.width + z + spacing;
			}
		}

		free(file.data);
		FT_Done_Face(face);
		FT_Done_FreeType(library);
	}
}

void fbMemset(void *addr, int c, size_t len)
{
	unsigned int i, *p;

	i = (c & 0xff) << 8;
	i |= i << 16;
	len >>= 2;

	for (p = addr; len--; p++) *p = i;	
}

void setMem(ArgData data)
{
	int ShmID;
	key_t ShmKEY;
	struct ArgData *ShmDAP;
	
	if(data.status == DEF_READY)
	{
		ShmKEY = ftok(".", 'x');
		ShmID = shmget(ShmKEY, sizeof(struct ArgData), IPC_CREAT | 0666);
		
		if (ShmID < 0) 
		{
			 LOGGER("    ERROR: Shared memory can not be reserved for writing");
			 return false;
		}

		ShmDAP = (struct ArgData *) shmat(ShmID, NULL, 0);
		
		if ((int) ShmDAP == -1) 
		{
			 LOGGER("    ERROR: Shared memory cannot be attached for writing");
			 return false;
		}
		else LOGGER("    Attached shared memory for writing");
		
		//set data
		ShmDAP->id = data.id;
		strcpy(ShmDAP->cmd, data.cmd);
		LOGGER("    Write data: %d = %s", data.id, data.cmd);
		
		//set transaction status
		data.status = DEF_EMPTY;
		ShmDAP->status = DEF_READY;
				
		shmdt((void *) ShmDAP);
		LOGGER("    Detached shared memory for writing");
	}
	else LOGGER("    Data is not prepared for writing; skipping it..");
}

ArgData getMem(void)
{
	int ShmID;
	key_t ShmKEY;
	ArgData data;
	struct ArgData *ShmDAP;

	data.status = DEF_EMPTY;
	
	ShmKEY = ftok(".", 'x');
	ShmID = shmget(ShmKEY, sizeof(struct ArgData), 0666);

	if (ShmID < 0) 
	{
		 LOGGER("    ERROR: Shared memory can not be reserved for reading");
		 return data;
	}

	ShmDAP = (struct ArgData *) shmat(ShmID, NULL, 0);

	if ((int) ShmDAP == -1) 
	{
		 LOGGER("    ERROR: Shared memory cannot be attached for reading");
		 return data;
	}
	else LOGGER("    Attached shared memory for reading");

	//set data
	strcpy(data.cmd, ShmDAP->cmd);
	data.id = ShmDAP->id;
	data.reset = 0;

	//set transaction status
	data.status = DEF_INPUT;
	ShmDAP->status = DEF_EMPTY;
	LOGGER("    Read data: %d = %s", data.id, data.cmd);

	shmdt((void *) ShmDAP);
	LOGGER("    Detached shared memory for reading");
	
	return data;
}

void delMem(void)
{
	int ShmID;
	key_t ShmKEY;
	
	ShmKEY = ftok(".", 'x');
	ShmID = shmget(ShmKEY, sizeof(struct ArgData), IPC_RMID | 0666);

	if (ShmID >= 0) 
	{
		shmctl(ShmID, IPC_RMID, NULL);
		LOGGER("    Released shared memory reference for writing");
	}
}

bool isMemSet(void)
{
	int ShmID;
	key_t ShmKEY;

	ShmKEY = ftok(".", 'x');
	ShmID = shmget(ShmKEY, sizeof(struct ArgData), 0666);

	if (ShmID < 0) return false;
		else return true;
}

ArgData getArg(int argc, char argv[])
{
	ArgData data;
	
	//Command line arguments
	static struct option options[] = 
	{
		{"picture", optional_argument, NULL, 'p'},
		{"message", optional_argument, NULL, 'm'},
		{"console", no_argument, NULL, 'c'},	
		{"exit", optional_argument, NULL, 'x'},	
	};

	//Store command line argument into a structure
	int opt = getopt_long(argc, argv, "p:m:c:x", options, NULL);

	if (opt == -1) 
	{
		data.id = 0;
		strcpy(data.cmd, "");
		data.status = DEF_EMPTY;
	}
	else
	{
		data.id = (int) opt;

		if (optarg != NULL)
		{
			if(sizeof (optarg) < 255) strcpy(data.cmd, optarg);
				else strncpy(data.cmd, optarg, 255);
		}
		else strcpy(data.cmd, "");
		
		data.reset = 0;
		data.status = DEF_INPUT;
	}

	return data;
}

ArgData parse(ArgData data)
{
	char part1[100], part2[100], part3[100], part4[100];
	
	if (data.status == DEF_INPUT)
	{
		switch (data.id)
		{
			case 'p':		
				if (strcmp(data.cmd, ""))
				{
					if (indexOf(data.cmd, "@") > 0)
					{					
						strncpy(part1, data.cmd, indexOf(data.cmd, "@"));
						strcpy(part3, data.cmd + indexOf(data.cmd, "@") + 1);
						
						if(part1[0] == '*')
						{
							data.reset = 1;
							strcpy(part1, part1 + 1);
						}
						else if(part3[0] == '*')
						{
							data.reset = 1;
							strcpy(part3, part3 + 1);
						}
						else data.reset = 0;
						
						//detect coordinates (x, y))
						if(indexOf(part1, "x") > 0)
						{
							strncpy(part2, part1, indexOf(part1, "x"));						
							data.pointx = atoi(part2);

							strcpy(part2, part1 + indexOf(part1, "x") + 1);						
							data.pointy = atoi(part2);
						}
						else 
						{
							data.pointx = atoi(part1);
							data.pointy = 0;
						}
					}
					else 
					{
						strcpy(part3, data.cmd);
						
						if(part3[0] == '*')
						{
							data.reset = 1;
							strcpy(part3, part3 + 1);
						}
					}
					
					//if is not a complete path or if is not part of distribution location add the distribution resource prefix
					if(!startsWith(part3, "/") && !startsWith(part3, "images/")) 
					{
						strcpy(data.data, "images/");
						strcat(data.data, part3);
					}
					else strcpy(data.data, part3);
					
					//check if the image has supported extention (JPG)
					if(!endsWith(data.data, ".jpg")) strcat(data.data, ".jpg");
				}
				else strcpy(data.data, DEF_IMG);

				LOGGER("    Discovered parameters: --picture = %s", data.data);
				LOGGER("                              @point = (%d,%d)", data.pointx, data.pointy);
				LOGGER("                              *reset = %d", data.reset);
				data.status = DEF_READY;
				break;

			case 'm':			
				if (strcmp(data.cmd, ""))
				{
					if (indexOf(data.cmd, "@") > 0)
					{					
						strncpy(part4, data.cmd, indexOf(data.cmd, "@"));
						strcpy(data.data, data.cmd + indexOf(data.cmd, "@") + 1);
						
						if(part4[0] == '*')
						{
							data.reset = 0;
							strcpy(part4, part4 + 1);
						}
						else if(data.data[0] == '*')
						{
							data.reset = 0;
							strcpy(data.data, data.data + 1);
						}
						else data.reset = 1;
						
						//check if the coordinates includes also the font details (size and font name))
						if (indexOf(part4, ":") >= 0) strncpy(part1, part4, indexOf(part4, ":"));
							else strcpy(part1, part4);
						
						//detect coordinates (x, y))
						if(indexOf(part1, "x") > 0)
						{
							strncpy(part2, part1, indexOf(part1, "x"));						
							data.pointx = atoi(part2);

							strcpy(part2, part1 + indexOf(part1, "x") + 1);						
							data.pointy = atoi(part2);
						}
						else if (strcmp(part1, ""))
						{
							data.pointx = atoi(part1);
							data.pointy = 0;
						}
						
						// detect font details (size and name)
						if (indexOf(part4, ":") >= 0) 
						{
							strcpy(part1, part4 + indexOf(part4, ":") + 1);

							if(indexOf(part1, ":") > 0)
							{
								strncpy(part3, part1, indexOf(part1, ":"));
								strcpy(part2, part1 + indexOf(part1, ":") + 1);
								data.fsize = atoi(part2);
							}
							else 
							{
								if(atoi(part1) > 0) 
								{
									data.fsize = atoi(part1);
									strcpy(part3, DEF_TTF);
								}
								else strcpy(part3, part1);
							}

							//if is not a complete path or if is not part of distribution location add the distribution resource prefix
							if(!startsWith(part3, "/") && !startsWith(part3, "fonts/")) 
							{
								strcpy(data.fname, "fonts/");
								strcat(data.fname, part3);
							}
							else  strcpy(data.fname, part3);

							//check if the font has supported extention (TTF)
							if(!endsWith(data.fname, ".ttf")) strcat(data.fname, ".ttf");
						}
						else
						{
							strcpy(data.fname, DEF_TTF);
							data.fsize = DEF_SIZE;
						}
					}
					else 
					{
						if(data.data[0] == '*')
						{
							data.reset = 0;
							strcpy(data.data, data.data + 1);
						}
						else data.reset = 1;
						
						strcpy(data.data, data.cmd);
						
						strcpy(data.fname, DEF_TTF);
						data.fsize = DEF_SIZE;
					}
				}
				else 
				{
					strcpy(data.data[0], "");
					data.fname[0] = DEF_TTF;
					data.fsize = DEF_SIZE;
				}
				
				LOGGER("    Discovered parameters: --message = %s", data.data);
				LOGGER("                              @point = (%d,%d)", data.pointx, data.pointy);
				LOGGER("                              :font  = %s:%d", data.fname, data.fsize);
				LOGGER("                              *reset = %d", data.reset);
				data.status = DEF_READY;
				break;
				
			case 'x':
				if (strcmp(data.cmd, ""))
				{
					data.delay = atoi(data.cmd);
				}
				else data.delay = 0;
				
				LOGGER("    Discovered parameter: --exit = %d", data.delay);
				data.status = DEF_READY;
				break;
		}
	}
	else LOGGER("    Data structure is not empty");
	
	return data;
}

int main(int argc, char **argv)
{
	ArgData data, prevdata;

	//set file channel
	setlocale(LC_ALL, "");
	sprintf(fileLog, "%s%s%s", "/var/log/", basename(argv[0]), ".log");
	
	//Extract absolute path of executable in order to identify related resources: picture and font files
	readlink("/proc/self/exe", PATH, sizeof (PATH));
	sprintf(PATH, "%s%s", dirname(dirname(PATH)), "/share/splash");	

	//read input arguments
	LOGGER("Reading input data");
	data = getArg(argc, argv);

	if(!isMemSet())
	{

		remove(fileLog);
		LOGGER("*** New process starts NEW workflow");
		
		if(data.id == 'c')
		{
			LOGGER("Uses the current graphical console and set frame buffer");

			//initialize the screen
			setFrameBuffer();

			data.status = DEF_READY;
		}
		else
		{
			LOGGER("Creates new graphical console and set frame buffer");

			//open graphical console
			openGraphicsOnConsole(&vt);

			//initialize the screen
			setFrameBuffer();

			//set black screen
			setBlackScreen();
		}
	}
	else
	{
		LOGGER("*** New process runs OPEN workflow");
		
		//initialize the screen
		setFrameBuffer();	

		//read old data from memory and parse it
		LOGGER("Reading memory buffer");
		prevdata = getMem();
		LOGGER("Parsing memory data");
		prevdata = parse(prevdata);

		//remove old message
		if(prevdata.status == DEF_READY && prevdata.id == 'm' && prevdata.reset > 0)
		{
			LOGGER("Removing old text message from screen");
			drawText(prevdata.pointx, prevdata.pointy, prevdata.fname, prevdata.fsize, prevdata.data, createColor16bit(0, 0, 0), 3, 0);
		}
	}
	
	//parse arguments 
	LOGGER("Parsing input data");
	data = parse(data);
		
	//process PIC-UPDATE event
	if(data.status == DEF_READY && data.id == 'p')
	{
		LOGGER("Request for Picture..");
		
		//set black screen
		if(data.reset > 0) 
		{
			LOGGER("    Set black screen");
			setBlackScreen();
		}

		//display new image
		drawJPEG(data.pointx, data.pointy, data.data);
	}

	//process MSG-UPDATE event
	if(data.status == DEF_READY && data.id == 'm')
	{
		LOGGER("Request for Message..");
		
		//display new message
		drawText(data.pointx, data.pointy, data.fname, data.fsize, data.data, createColor16bit(150, 194, 229), 3, 0);
	}
	
	//process EXIT-REQUEST event
	if(data.status == DEF_READY && data.id == 'x')
	{
		LOGGER("Request for Exit..");

		if(data.delay > 0)
		{
			LOGGER("    Sleep %d seconds for exit", data.delay);
			sleep(data.delay);
		}
		
		//delete reserved memory buffer
		LOGGER("Deleting memory buffer");
		delMem();
		
		//clear screen
		LOGGER("Clearing frame buffer and closing graphical console");
		openGraphicsOnConsole(&vt);
		closeConsole(&vt);
		
		closeFrameBuffer();
		LOGGER("Workflow EXIT.");
	}
	else
	{
		// write the current transaction in memory
		LOGGER("Writing memory buffer");
		setMem(data);
		
		LOGGER("Process DONE.");
	}
	
	exit(0);
}
