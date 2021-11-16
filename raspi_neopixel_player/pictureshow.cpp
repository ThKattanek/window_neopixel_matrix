#include "pictureshow.h"

PictureShow::PictureShow(int xw, int yw, uint32_t *buffer)
{
	this->xw = xw;
	this->yw = yw;
	this->buffer = buffer;

	this->led_count = xw * yw;
	this->row_pointers = nullptr;

	picture_buffer = new uint32_t[led_count];
}

PictureShow::~PictureShow()
{
	delete [] picture_buffer;
}

bool PictureShow::LoadPNG(char *filename)
{
	FILE *fp = fopen(filename, "rb");
	if(fp == nullptr)
		return false;

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png)
	{
		fclose(fp);
		return false;
	}

	png_infop info = png_create_info_struct(png);
	if(!png)
	{
		fclose(fp);
		return false;
	}

	if(setjmp(png_jmpbuf(png)))
	{
		fclose(fp);
		return false;
	}

	png_init_io(png, fp);

	png_read_info(png, info);

	width      = png_get_image_width(png, info);
	height     = png_get_image_height(png, info);
	color_type = png_get_color_type(png, info);
	bit_depth  = png_get_bit_depth(png, info);

	cout << "width = " << width << endl;
	cout << "height = " << height << endl;
	cout << "color_type = " << (uint16_t)color_type << endl;
	cout << "bit_depth = " << (uint16_t)bit_depth << endl;

	if(width != xw || height != yw)
		return false;

	// Read any color_type into 8bit depth, RGBA format.
	// See http://www.libpng.org/pub/png/libpng-manual.txt

	if(bit_depth == 16)
		png_set_strip_16(png);

	if(color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);

	// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
	if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png);

	if(png_get_valid(png, info, PNG_INFO_tRNS))
	  png_set_tRNS_to_alpha(png);

	// These color_type don't have an alpha channel then fill it with 0xff.
	if(color_type == PNG_COLOR_TYPE_RGB ||
	   color_type == PNG_COLOR_TYPE_GRAY ||
	   color_type == PNG_COLOR_TYPE_PALETTE)
	  png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

	if(color_type == PNG_COLOR_TYPE_GRAY ||
	   color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	  png_set_gray_to_rgb(png);

	png_read_update_info(png, info);

	if (row_pointers) abort();

	row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
	for(int y = 0; y < height; y++)
	{
		row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
	}

	png_read_image(png, row_pointers);

	int c = 0;
	for(int y=0; y<height; y++)
	{
		for(int x=0; x<width*4; x+=4)
		{
			picture_buffer[c++] = row_pointers[y][x] | row_pointers[y][x+1] << 8 | row_pointers[y][x+2] << 16;
		}
	}

	free(row_pointers);

	fclose(fp);
	return true;
}

void PictureShow::Render()
{
	for(int i=0; i<led_count; i++)
		buffer[i] = picture_buffer[i];
}
