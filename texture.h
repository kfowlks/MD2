#ifndef _C99_TXR_H
#define _C99_TXR_H

#ifdef WIN32
#ifndef uint16
#define uint16 unsigned int
#endif

#ifndef uint32
#define uint32 unsigned long
#endif
#endif

#include <GL/gl.h>

struct texture {
	uint16 *pixbuf;
	char filename[30];
	uint32 txraddr;
	GLint txr;
	int xsize;
	int ysize;
};

void pset(struct texture tex, int x, int y, uint16 col);
uint16 color_fg(int color, int alpha);
void load_floortex();
void jpeg_to_texture(char *filename, int pos);

#define CLEAR_PIXEL color_fg(255)

#endif