/*
 *
 * cc twilight.c -lgl_s -lm -o twilight
 *
 * Copyright (c) 1991, 1992 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the name of Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Silicon Graphics.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL SILICON GRAPHICS BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF THE
 * POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * A background/root window that looks like the twilight sky.
 * Howard Look
 */

#include <gl/gl.h>
#include <gl/device.h>
#include <math.h>

/* prototypes */
void draw_background(float x, float y, Boolean rgb_mode);
long star_color(float y, float max_y);

/* point in y where color transition occurs */
#define TRANSITION 0.2
#define NUM_SMALL_STARS 2500
#define NUM_BIG_STARS 200

/* colors */
static long orange[] = {255,72,0};
static long blueish[] = {0,110,189};
static long black[] = {0,0,0};

static unsigned short half[16] = {
	0x5555, 0xAAAA,	0x5555, 0xAAAA,
	0x5555, 0xAAAA,	0x5555, 0xAAAA,
	0x5555, 0xAAAA,	0x5555, 0xAAAA,
	0x5555, 0xAAAA,	0x5555, 0xAAAA};

main()
{
	long gid, sizex, sizey;
	Boolean redraw_needed = FALSE;
	Boolean rgb_mode;
	short val;
	long dev;

	rgb_mode =
		getgdesc(GD_BITS_NORM_SNG_RED) &&
		getgdesc(GD_BITS_NORM_SNG_GREEN) &&
		getgdesc(GD_BITS_NORM_SNG_BLUE);

	imakebackground();
	gid = winopen("");
	if (rgb_mode)
		RGBmode();
	gconfig();

	if (!rgb_mode)
		defpattern(1,16,half);

	qenter(REDRAW,gid);

	while (1)
	{
		if (redraw_needed)
		{
			getsize(&sizex, &sizey);
			ortho2(-.5, (float)sizex-.5, -.5, (float)sizey-.5);
			viewport(0, sizex-1, 0, sizey-1);
			draw_background((float)sizex, (float)sizey, rgb_mode);
			redraw_needed = FALSE;
		}

		while (qtest() || (! redraw_needed))
		{
			dev = qread(&val);
			if (dev == REDRAW)
				redraw_needed = TRUE;
		}
	}
}


void draw_background(float x, float y, Boolean rgb_mode)
{
    int i,j;
	float v1[2],v2[2],v3[2],v4[2];

	v1[0] = v4[0] = 0.0;
	v2[0] = v3[0] = x;

	v1[1] = v2[1] = 0.0;
	v3[1] = v4[1] = y*TRANSITION;

	if (rgb_mode)
	{
		bgnpolygon();
			c3i(orange);
			v2f(v1);
			v2f(v2);
			c3i(blueish);
			v2f(v3);
			v2f(v4);
		endpolygon();

		v1[1] = v2[1] = y;
		bgnpolygon();
			c3i(blueish);
			v2f(v4);
			v2f(v3);
			c3i(black);
			v2f(v2);
			v2f(v1);
		endpolygon();
	}
	else
	{
		color(BLACK);
		clear();
		color(BLUE);
		setpattern(1);
		clear();
		setpattern(0);
	}

	srand48(0); /* seed so stars are always the same */

    bgnpoint();
		for (i = 0; i < NUM_SMALL_STARS; i++)
		{
			float v[2];

			v[0] = x*drand48();
			v[1] = y*drand48();

			if (rgb_mode)
				cpack(star_color(v[1], y));
			else
				color(WHITE);

			v2f(v);
	    }
    endpoint();

	for (i = 0; i < NUM_BIG_STARS; i++)
	{
		float v[2], size;

		v[0] = x*drand48();
		v[1] = y*drand48();
		size = drand48();

		pushmatrix();
			translate(v[0],v[1],0.0);
			scale(size,size,size);

			if (rgb_mode)
				cpack(star_color(v[1], y));
			else
				color(WHITE);

			sboxf(0.0,1.0,3.0,2.0);
			sboxf(1.0,0.0,2.0,3.0);
		popmatrix();
	}

	swapbuffers();
}


long
star_color(float y, float max_y)
{
    int r,g,b;
	float a;
	float ratio;

	if (y > max_y/2)
		return(0xFFFFFF);

    if (y < TRANSITION*max_y)
	{
		ratio = y / (max_y * TRANSITION);

		r = orange[0]*(1.0 - ratio) + blueish[0]*ratio;
		g = orange[1]*(1.0 - ratio) + blueish[1]*ratio;
		b = orange[2]*(1.0 - ratio) + blueish[2]*ratio;
    }
	else
	{
		ratio = (y - max_y*TRANSITION)/(max_y - max_y*TRANSITION);

		r = blueish[0]*(1.0 - ratio) + black[0]*ratio;
		g = blueish[1]*(1.0 - ratio) + black[1]*ratio;
		b = blueish[2]*(1.0 - ratio) + black[2]*ratio;
    }

	/* r,g and b are now the sky color at y, now blend with white */

	a = (y/(max_y/2.0));

	r = r*(1.0-a) + 255*a;
	g = g*(1.0-a) + 255*a;
	b = b*(1.0-a) + 255*a;

    return (r&0xff) + ((g&0xff)<<8) + ((b&0xff)<<16);
}
