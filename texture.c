#include "texture.h"
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <jpeglib.h>
#include <kos.h>

#define MAX_TEXTURES 2048

GLint texture_ram[MAX_TEXTURES];
struct texture textures[MAX_TEXTURES];

int max_pos=0;

void texture_init() {
  glGenTextures(MAX_TEXTURES,&texture_ram[0]);
}

GLint next_tex() {
	static int pos=0;
	GLint ret=texture_ram[pos];
	pos++;
	return ret;
}

int next_tex_id() {
	max_pos++;
	return max_pos;
}

struct texture *get_tex(int index) {
	return &textures[index];
}

int find_tex(char *filename) {
	int index=0;
	for(index=0;index<=max_pos;index++) {
		if(!strcmp(textures[index].filename,filename)) {
			return index;
		}
	}
	return -1;
}

void switch_tex(int pos) {
	//printf("Switching to texture %i (%i x %i)\n",pos,textures[pos].xsize,textures[pos].ysize);
	glBindTexture(GL_TEXTURE_2D, textures[pos].txr);
}

void pset(struct texture tex, int x, int y, uint16 col) {
	tex.pixbuf[x+y*(tex.xsize)]=col;
}

uint16 color_fg(int color, int alpha) {
		int r,g,b,a=alpha/*=0x0f*/;
        //color palette based on VICE's default.vpl
        switch(color) {
          case 0:
            r = 0x10; g = 0x10; b = 0x10;
            break;
          case 1:
            r = 0xFF; g = 0xFF; b = 0xFF;
            break;
          case 2:
            r = 0xE0; g = 0x40; b = 0x40;
            break;
          case 3:
            r = 0x60; g = 0xFF; b = 0xFF;
            break;
          case 4:
            r = 0xE0; g = 0x60; b = 0xE0;
            break;
          case 5:
            r = 0x40; g = 0xE0; b = 0x40;
            break;
          case 6:
            r = 0x40; g = 0x40; b = 0xE0;
	          break;
          case 7:
            r = 0xFF; g = 0xFF; b = 0x40;
            break;
          case 8:
            r = 0xE0; g = 0xA0; b = 0x40;
            break;
          case 9:
            r = 0x9C; g = 0x74; b = 0x48;
            break;
          case 10:
            r = 0xFF; g = 0xA0; b = 0xA0;
            break;
          case 11:
            r = 0x54; g = 0x54; b = 0x54;
            break;
          case 12:
            r = 0x88; g = 0x88; b = 0x88;
            break;
          case 13:
            r = 0xA0; g = 0xFF; b = 0xA0;
            break;
          case 14:
            r = 0xA0; g = 0xA0; b = 0xFF;
            break;
          case 15:
            r = 0xC0; g = 0xC0; b = 0xC0;
            break;
          default: //transparent
            r = 0; g = 0; b = 0; a=0x00;
            break;
        }
#ifdef WIN32
        return  (((r >> 3) & 0x1f) << 11)
                | (((g >> 2) & 0x3f) << 5)
                | (((b >> 3) & 0x1f) << 0);
#else
				return (a << 12) |
               ((r >> 1) << 8) |
               ((g >> 2) << 4) |
               ((b >> 1) << 0);
#endif
}

struct texture jpeg_tex;
int jpeg_y=0;

/* total crap, assumes a 640x480 or less jpeg */
void put_scanline_someplace(char *buffer, int row_stride, struct texture txr)
{
    int i;
    for(i=0; i<row_stride/3; i++)
        pset(txr,i,jpeg_y,((buffer[i*3]>>3)<<11) + ((buffer[i*3+1]>>2)<<5) + (buffer[i*3+2]>>3));

    jpeg_y--;
}

void jpeg_to_texture(char *filename,int pos) {
  struct jpeg_decompress_struct cinfo;
    
    struct jpeg_error_mgr jerr;
    /* More stuff */
    FILE * infile;                /* source file */
    JSAMPARRAY buffer;            /* Output row buffer */
    int row_stride;               /* physical row width in output buffer */
    char newfile[256];
   if ((infile = fopen(filename, "rb")) == NULL) {
	fprintf(stderr, "can't open %s\n", filename);
        return;
    }
  	if(pos>max_pos) { max_pos=pos; }
	  strcpy(textures[pos].filename,filename);
 
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    (void) jpeg_read_header(&cinfo, TRUE);
    (void) jpeg_start_decompress(&cinfo);
    row_stride = cinfo.output_width * cinfo.output_components;
    jpeg_y=cinfo.output_height-1;
		textures[pos].xsize=cinfo.output_width;
		textures[pos].ysize=cinfo.output_height;
		textures[pos].txr=next_tex();
		textures[pos].txraddr=ta_txr_allocate(cinfo.output_width * cinfo.output_height * 2);
		textures[pos].pixbuf=malloc(cinfo.output_width * cinfo.output_height * 2);
		printf("Loading %s (%i x %i) into slot %i...\n",filename,cinfo.output_width,cinfo.output_height,pos);
		buffer = (*cinfo.mem->alloc_sarray)
    ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
    while (cinfo.output_scanline < cinfo.output_height) {
	(void) jpeg_read_scanlines(&cinfo, buffer, 1);
	put_scanline_someplace(buffer[0], row_stride,textures[pos]);
    }
    (void) jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
		glBindTexture(GL_TEXTURE_2D, textures[pos].txr);
 	txr_twiddle_copy(textures[pos].pixbuf, textures[pos].xsize, textures[pos].ysize,textures[pos].txraddr, textures[pos].xsize, textures[pos].ysize, 0);
	glKosTex2D(GL_RGB565_TWID, textures[pos].xsize, textures[pos].ysize, textures[pos].txraddr);
}
