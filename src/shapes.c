
/**
 * Clue Media Experience 
 * shapes.c - implementation of drawing functions
 */

#include "shapes.h"
#include "buffer.h"


CmdData SetShapeProps(CmdData data)
{
	char tmpval[255];
	
	if(indexOf(strtrim(data.value), "line") >= 0)
	{
		// linecolor
		if(hasDataProperty(data, "linecolor"))
		{
			strcpy(tmpval, getDataProperty(data, "linecolor"));
			if(strcmp(tmpval, "") == 0) data = setDataProperty(data, "linecolor", DEFSHP_COLOR);
		}
		else data = setDataProperty(data, "linecolor", DEFSHP_COLOR);
		
		// x2point
		if(hasDataProperty(data, "x2point"))
		{
			strcpy(tmpval, getDataProperty(data, "x2point"));
			if(strcmp(tmpval, "") == 0) data = setIntDataProperty(data, "x2point", fbs.vinfo.xres);
		}
		else data = setIntDataProperty(data, "x2point", fbs.vinfo.xres);

		// y2point
		if(hasDataProperty(data, "y2point"))
		{
			strcpy(tmpval, getDataProperty(data, "y2point"));
			if(strcmp(tmpval, "") == 0) data = setIntDataProperty(data, "y2point", fbs.vinfo.yres);
		}
		else data = setIntDataProperty(data, "y2point", fbs.vinfo.yres);
	}
	else if(strcmp(strtrim(data.value), "rectangle") == 0)
	{
		// linecolor
		if(hasDataProperty(data, "linecolor"))
		{
			strcpy(tmpval, getDataProperty(data, "linecolor"));
			if(strcmp(tmpval, "") == 0) data = setDataProperty(data, "linecolor", DEFSHP_COLOR);
		}
		else data = setDataProperty(data, "linecolor", DEFSHP_COLOR);
		
		// width
		if(hasDataProperty(data, "width"))
		{
			strcpy(tmpval, getDataProperty(data, "width"));
			if(strcmp(tmpval, "") == 0) data = setIntDataProperty(data, "width", fbs.vinfo.xres/10);
		}
		else data = setIntDataProperty(data, "width", fbs.vinfo.xres/10);

		// height
		if(hasDataProperty(data, "height"))
		{
			strcpy(tmpval, getDataProperty(data, "height"));
			if(strcmp(tmpval, "") == 0) data = setIntDataProperty(data, "height", fbs.vinfo.yres/10);
		}
		else data = setIntDataProperty(data, "height", fbs.vinfo.yres/10);
		
		// linewidth
		if(hasDataProperty(data, "linewidth"))
		{
			strcpy(tmpval, getDataProperty(data, "linewidth"));
			if(strcmp(tmpval, "") == 0) data = setIntDataProperty(data, "linewidth", 1);
		}
		else data = setIntDataProperty(data, "linewidth", 1);		
	}
	else if(strcmp(strtrim(data.value), "circle") == 0)
	{
		// linecolor
		if(hasDataProperty(data, "linecolor"))
		{
			strcpy(tmpval, getDataProperty(data, "linecolor"));
			if(strcmp(tmpval, "") == 0) data = setDataProperty(data, "linecolor", DEFSHP_COLOR);
		}
		else data = setDataProperty(data, "linecolor", DEFSHP_COLOR);
		
		// radius
		if(hasDataProperty(data, "radius"))
		{
			strcpy(tmpval, getDataProperty(data, "radius"));
			if(strcmp(tmpval, "") == 0) data = setIntDataProperty(data, "radius", fbs.vinfo.yres/10);
		}
		else data = setIntDataProperty(data, "radius", fbs.vinfo.yres/10);
		
		// linewidth
		if(hasDataProperty(data, "linewidth"))
		{
			strcpy(tmpval, getDataProperty(data, "linewidth"));
			if(strcmp(tmpval, "") == 0) data = setIntDataProperty(data, "linewidth", 1);
		}
		else data = setIntDataProperty(data, "linewidth", 1);		
	}
	else if(strcmp(strtrim(data.value), "progressbar") == 0)
	{
		// linecolor
		if(hasDataProperty(data, "linecolor"))
		{
			strcpy(tmpval, getDataProperty(data, "linecolor"));
			if(strcmp(tmpval, "") == 0) data = setDataProperty(data, "linecolor", DEFSHP_COLOR);
		}
		else data = setDataProperty(data, "linecolor", DEFSHP_COLOR);
		
		// barcolor
		if(hasDataProperty(data, "barcolor"))
		{
			strcpy(tmpval, getDataProperty(data, "barcolor"));
			if(strcmp(tmpval, "") == 0) data = setDataProperty(data, "barcolor", "#FFFFFF");
		}
		else data = setDataProperty(data, "barcolor", "#FFFFFF");
		
		// width
		if(hasDataProperty(data, "width"))
		{
			strcpy(tmpval, getDataProperty(data, "width"));
			if(strcmp(tmpval, "") == 0) data = setIntDataProperty(data, "width", fbs.vinfo.xres/3);
		}
		else data = setIntDataProperty(data, "width", fbs.vinfo.xres/3);

		// height
		if(hasDataProperty(data, "height"))
		{
			strcpy(tmpval, getDataProperty(data, "height"));
			if(strcmp(tmpval, "") == 0) data = setIntDataProperty(data, "height", fbs.vinfo.yres/20);
		}
		else data = setIntDataProperty(data, "height", fbs.vinfo.yres/20);
		
		// linewidth
		if(hasDataProperty(data, "linewidth"))
		{
			strcpy(tmpval, getDataProperty(data, "linewidth"));
			if(strcmp(tmpval, "") == 0) data = setIntDataProperty(data, "linewidth", 1);
		}
		else data = setIntDataProperty(data, "linewidth", 1);
		
		// percent
		if(hasDataProperty(data, "percent"))
		{
			strcpy(tmpval, getDataProperty(data, "percent"));
			if(strcmp(tmpval, "") == 0) data = setIntDataProperty(data, "percent", 0);
		}
		else data = setIntDataProperty(data, "percent", 0);		
	}
	
	
	return data;
}

void DrawShape(CmdData data)
{
	if(indexOf(strtrim(data.value), "line") >= 0)
	{
		DrawLine(data);
	}
	else if(strcmp(strtrim(data.value), "rectangle") == 0)
	{
		DrawRectangle(data);
	}
	else if(strcmp(strtrim(data.value), "circle") == 0)
	{
		DrawCircle(data);
	}
	else if(strcmp(strtrim(data.value), "progressbar") == 0)
	{
		DrawProgressBar(data);
	}
}

void OverdrawShape(CmdData data)
{
	if(indexOf(strtrim(data.value), "line") >= 0)
	{
		WipeLine(data);
	}
	else if(strcmp(strtrim(data.value), "rectangle") == 0)
	{
		WipeRectangle(data);
	}
	else if(strcmp(strtrim(data.value), "circle") == 0)
	{
		WipeCircle(data);
	}
	else if(strcmp(strtrim(data.value), "progressbar") == 0)
	{
		WipeProgressBar(data);
	}
}

void DrawLine(CmdData data)
{
	DEBUG("Drawing Line shape from command: [%s] >> x=%d, y=%d, x2=%d, y2=%d, linecolor=%d", 
		data.value, data.xpoint, data.ypoint,
		getIntDataProperty(data, "x2point"), 
		getIntDataProperty(data, "y2point"), 
		getColorDataProperty(data, "linecolor") );
	
	if(strcmp(strtrim(data.value), "line") == 0)
	{
		__drawLine(data.xpoint, data.ypoint, 
			getIntDataProperty(data, "x2point"),
			getIntDataProperty(data, "y2point"),
			getColorDataProperty(data, "linecolor"));
	}
	else if(strcmp(strtrim(data.value), "hline") == 0)
	{
		__drawHorizontalLine(data.xpoint, data.ypoint, 
			getIntDataProperty(data, "x2point"),
			getColorDataProperty(data, "linecolor"));
	}
	else if(strcmp(strtrim(data.value), "vline") == 0)
	{
		__drawVerticalLine(data.xpoint, data.ypoint, 
			getIntDataProperty(data, "y2point"),
			getColorDataProperty(data, "linecolor"));
	}
}

void WipeLine(CmdData data)
{
	DEBUG("Overdrawing Line shape for removal: [%s] >> x=%d, y=%d, x2=%d, y2=%d", data.value, data.xpoint, data.ypoint, getIntDataProperty(data, "x2point"), getIntDataProperty(data, "y2point") );
	
	if(strcmp(strtrim(data.value), "line") == 0)
	{
		__drawLine(data.xpoint, data.ypoint, 
			getIntDataProperty(data, "x2point"),
			getIntDataProperty(data, "y2point"),
			getColorFromRGBA(0, 0, 0, 255));
	}
	else if(strcmp(strtrim(data.value), "hline") == 0)
	{
		__drawHorizontalLine(data.xpoint, data.ypoint, 
			getIntDataProperty(data, "x2point"),
			getColorFromRGBA(0, 0, 0, 255));
	}
	else if(strcmp(strtrim(data.value), "vline") == 0)
	{
		__drawVerticalLine(data.xpoint, data.ypoint, 
			getIntDataProperty(data, "y2point"),
			getColorFromRGBA(0, 0, 0, 255));
	}
}

void DrawRectangle(CmdData data)
{
	DEBUG("Drawing Rectangle shape from command: [%s] >> x=%d, y=%d, width=%d, height=%d, linecolor=%x, linewidth=%x, fillcolor=%x", 
		data.value, data.xpoint, data.ypoint,
		getIntDataProperty(data, "width"), 
		getIntDataProperty(data, "height"), 
		getColorDataProperty(data, "linecolor"),
		getIntDataProperty(data, "linewidth"),
		getColorDataProperty(data, "fillcolor"));
	
	__drawRectangle(data.xpoint, data.ypoint,
		getIntDataProperty(data, "width"), 
		getIntDataProperty(data, "height"), 
		getColorDataProperty(data, "linecolor"),
		getIntDataProperty(data, "linewidth"),
		getColorDataProperty(data, "fillcolor"));
}

void WipeRectangle(CmdData data)
{
	DEBUG("Overdrawing Rectangle shape for removal: [%s] >> x=%d, y=%d, width=%d, height=%d", data.value, data.xpoint, data.ypoint, getIntDataProperty(data, "width"), getIntDataProperty(data, "height"));
	
	__drawRectangle(data.xpoint, data.ypoint,
		getIntDataProperty(data, "width"), 
		getIntDataProperty(data, "height"), 
		getColorFromRGBA(0, 0, 0, 255),
		getIntDataProperty(data, "linewidth"),
		getColorFromRGBA(0, 0, 0, 255));
}

void DrawCircle(CmdData data)
{
	DEBUG("Drawing Circle shape from command: [%s] >> x=%d, y=%d, radius=%d, linecolor=%d, linewidth=%d, fillcolor=%d", 
		data.value, data.xpoint, data.ypoint,
		getIntDataProperty(data, "radius"), 
		getColorDataProperty(data, "linecolor"),
		getIntDataProperty(data, "linewidth"),
		getColorDataProperty(data, "fillcolor"));
	
	__drawCircle(data.xpoint, data.ypoint,
		getIntDataProperty(data, "radius"), 
		getColorDataProperty(data, "linecolor"), 
		getIntDataProperty(data, "linewidth"),
		getColorDataProperty(data, "fillcolor"));
}

void WipeCircle(CmdData data)
{
	DEBUG("Overdrawing Circle shape from command: [%s] >> x=%d, y=%d, radius=%d", data.value, data.xpoint, data.ypoint, getIntDataProperty(data, "radius"));
	
	__drawCircle(data.xpoint, data.ypoint,
		getIntDataProperty(data, "radius"), 
		getColorFromRGBA(0, 0, 0, 255), 
		getIntDataProperty(data, "linewidth"),
		getColorFromRGBA(0, 0, 0, 255));
}

void DrawProgressBar(CmdData data)
{
	DEBUG("Drawing ProgressBar shape from command: [%s] >> x=%d, y=%d, width=%d, height=%d, percent=%d, linecolor=%d, linewidth=%d, barcolor=%d", 
		data.value, data.xpoint, data.ypoint,
		getIntDataProperty(data, "width"), 
		getIntDataProperty(data, "height"), 
		getIntDataProperty(data, "percent"), 
		getColorDataProperty(data, "linecolor"),
		getIntDataProperty(data, "linewidth"),
		getColorDataProperty(data, "barcolor"));
	
	__drawProcessBar(data.xpoint, data.ypoint,
		getIntDataProperty(data, "width"), 
		getIntDataProperty(data, "height"), 
		getIntDataProperty(data, "percent"), 
		getColorDataProperty(data, "linecolor"),
		getIntDataProperty(data, "linewidth"),
		getColorDataProperty(data, "barcolor"),
		getColorDataProperty(data, "backgroundcolor"));
}

void WipeProgressBar(CmdData data)
{
	DEBUG("Overdrawing ProgressBar shape for removal: [%s] >> x=%d, y=%d, width=%d, height=%d", data.value, data.xpoint, data.ypoint, getIntDataProperty(data, "width"), getIntDataProperty(data, "height"));
	
	__drawRectangle(data.xpoint, data.ypoint,
		getIntDataProperty(data, "width"), 
		getIntDataProperty(data, "height"), 
		getColorFromRGBA(0, 0, 0, 255),
		getIntDataProperty(data, "linewidth"),
		getColorFromRGBA(0, 0, 0, 255));
}


int __drawPixel(int x, int y, int32_t rgba)
{
	long int location;

	if (x < 0 || y < 0 || x >= fbs.vinfo.xres || y >= fbs.vinfo.yres) return;

	location = (x + fbs.vinfo.xoffset) * (fbs.vinfo.bits_per_pixel / 8) + (y + fbs.vinfo.yoffset) * fbs.finfo.line_length;
	SetColor(location, rgba);
}

void __drawHorizontalLine(int x, int y, int x2, long color)
{
	int xx;
	
	//adapt line position and dimension
	if(x2 < 0) x2 = fbs.vinfo.xres + x2;
	SetPosition(&x, &y, x2-x, 1);
		
	for (xx = x; xx < x2; xx++)
	{
		__drawPixel(xx, y, color);
	}
}

void __drawVerticalLine(int x, int y, int y2, int32_t color)
{
	int yy;
	
	//adapt line position and dimension
	if(y2 < 0) y2 = fbs.vinfo.xres + y2;
	SetPosition(&x, &y, 1, y2-y);
	
	for (yy = y; yy < y2; yy++)
	{
		__drawPixel(x, yy, color);
	}
}

void __drawLine(int x1, int y1, int x2, int y2, int32_t color)
{
	int i, dx, dy, sdx, sdy, dxabs, dyabs, x, y, px, py;

	dx = x2 - x1; //Delta x
	dy = y2 - y1; //Delta y
	dxabs = abs(dx); //Absolute delta
	dyabs = abs(dy); //Absolute delta
	sdx = (dx > 0) ? 1 : -1; //signum function
	sdy = (dy > 0) ? 1 : -1; //signum function
	x = dyabs >> 1;
	y = dxabs >> 1;
	px = x1;
	py = y1;

	if (dxabs >= dyabs)
	{
		for (i = 0; i < dxabs; i++)
		{
			y += dyabs;
			if (y >= dxabs)
			{
				y -= dxabs;
				py += sdy;
			}

			px += sdx;
			__drawPixel(px, py, color);
		}
	}
	else
	{
		for (i = 0; i < dyabs; i++)
		{
			x += dxabs;
			if (x >= dyabs)
			{
				x -= dyabs;
				px += sdx;
			}

			py += sdy;
			__drawPixel(px, py, color);
		}
	}
}

void __drawCircle(int xc, int yc, int radius, int32_t linecolor, int linewidth, int32_t fillcolor)
{
	int width;
	
	//adapt circle position
	SetPosition(&xc, &xc, radius, radius);
	
	for (width = 0; width < linewidth; width++)
	{
		int error = -radius + width;
		int x = radius - width;
		int y = 0;
		
		while (x >= y)
		{
			__drawPixel(xc + x, yc + y, linecolor);
			__drawPixel(xc - x, yc + y, linecolor);
			__drawPixel(xc + x, yc - y, linecolor);
			__drawPixel(xc - x, yc - y, linecolor);
			__drawPixel(xc + y, yc + x, linecolor);
			__drawPixel(xc - y, yc + x, linecolor);
			__drawPixel(xc + y, yc - x, linecolor);
			__drawPixel(xc - y, yc - x, linecolor);

			error += y;
			y++;
			error += y;

			if (error >= 0)
			{
				error += -x;
				x--;
				error += -x;
			}
		}
	}

	if (fillcolor >= 0)
	{
		int x = radius;
		for (width = linewidth; x > 0; width++)
		{
			int error = -radius + width;
			x = radius - width;
			int y = 0;
			
			while (x >= y)
			{
				__drawPixel(xc + x, yc + y, fillcolor);
				__drawPixel(xc - x, yc + y, fillcolor);
				__drawPixel(xc + x, yc - y, fillcolor);
				__drawPixel(xc - x, yc - y, fillcolor);
				__drawPixel(xc + y, yc + x, fillcolor);
				__drawPixel(xc - y, yc + x, fillcolor);
				__drawPixel(xc + y, yc - x, fillcolor);
				__drawPixel(xc - y, yc - x, fillcolor);				

				error += y;
				y++;
				error += y;

				if (error >= 0)
				{
					error += -x;
					x--;
					error += -x;
				}
			}

		}
	}
}

void __drawRectangle(int xc, int yc, int width, int height, int32_t linecolor, int linewidth, int32_t fillcolor)
{
	int x, y;
	
	//adapt rectangle position
	SetPosition(&xc, &yc, width, height);
	
	// Figure out where in memory to put the pixel
	for (y = yc; y < yc + height; y++)
	{
		for (x = xc; x < xc + width; x++)
		{
			// Draw rectangle borders
			if ((y < yc + linewidth) || (y > yc + height - linewidth - 1) ||
				(x < xc + linewidth) || (x > xc + width - linewidth - 1)) __drawPixel(x, y, linecolor);
			
			// Fill rectangle
			if ((fillcolor != NULL) && (y >= yc + linewidth) && (y <= yc + height - linewidth - 1) &&
				(x >= xc + linewidth) && (x <= xc + width - linewidth - 1)) __drawPixel(x, y, fillcolor);
		}
	}
}

int __drawProcessBar(int xc, int yc, int width, int height, int percent, int32_t linecolor, int linewidth, int32_t barcolor, int32_t backgroundcolor)
{
	//adapt progressbar position
	SetPosition(&xc, &yc, width, height);
	
	if(percent >= 0)
	{
		__drawRectangle(xc, yc, width, height, linecolor, linewidth, NULL);
	}
	
	if(percent != 0)
	{
		__drawRectangle(xc + linewidth, yc + linewidth, ((width - linewidth - linewidth) * abs(percent)) / 100, height - (2 * linewidth), barcolor, 0, barcolor);
	}

	if (backgroundcolor != NULL && percent >= 0)
	{
		__drawRectangle(xc + linewidth + ((width * abs(percent)) / 100), yc + linewidth, ((width * (100 - abs(percent))) / 100) - linewidth - linewidth, height - linewidth - linewidth, backgroundcolor, 0, backgroundcolor);
	}
}
