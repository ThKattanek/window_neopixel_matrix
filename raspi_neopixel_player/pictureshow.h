#ifndef PICTURESHOW_H
#define PICTURESHOW_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <inttypes.h>
#include <png.h>

using namespace std;

class PictureShow
{
public:

	PictureShow(int xw, int yw, uint32_t* buffer);
	~PictureShow();

	bool LoadPNG(char *filename);

	void Render();

private:

	int xw, yw, led_count;
	uint32_t *buffer;
	uint32_t *picture_buffer;

	int width, height;
	png_byte color_type;
	png_byte bit_depth;
	png_bytep *row_pointers;
};

#endif // PICTURESHOW_H
