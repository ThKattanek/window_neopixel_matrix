#include "plasma.h"

#include <math.h>

Plasma::Plasma(int xw, int yw, uint32_t *buffer)
{
	this->xw = xw;
	this->yw = yw;
	this->buffer = buffer;

	led_count = xw * yw;
}

Plasma::~Plasma()
{
}

void Plasma::Init()
{
	float rad;

	/*create sin lookup table */
	for (int i = 0; i < 512; i++)
	{
		rad =  ((float)i * 0.703125) * 0.0174532; /* 360 / 512 * degree to rad, 360 degrees spread over 512 values to be able to use AND 512-1 instead of using modulo 360*/
		aSin[i] = sin(rad) * 1024; /*using fixed point math with 1024 as base*/
	}

	/* create palette */
	for (int i = 0; i < 64; ++i)
	  {
		colors[i] = (i << 2);	// red
		colors[i] |= (255 - ((i << 2) + 1)) << 8;	// green
		colors[i+64] = 255 ;	// red
		colors[i+64] |= ((i << 2) + 1) << 8; // green
		colors[i+128] = (255 - ((i << 2) + 1)) ;	// red
		colors[i+128] |= (255 - ((i << 2) + 1)) << 8;	// green
		colors[i+192] = ((i << 2) + 1) << 8; // green
	  }
}

void Plasma::Render()
{
	uint32_t *buffer = this->buffer;

	uint8_t index;
	int i;

	tpos4 = pos4;
	tpos3 = pos3;

	for (int y = 0; y < yw; ++y)
	{
		tpos1 = pos1 + 5;
		tpos2 = pos2 + 3;

		tpos3 &= 511;
		tpos4 &= 511;

		for (int x = 0; x < xw; ++x)
		{
			tpos1 &= 511;
			tpos2 &= 511;

			i = aSin[tpos1] + aSin[tpos2] + aSin[tpos3] + aSin[tpos4]; /*actual plasma calculation*/

			index = 128 + (i >> 4); /*fixed point multiplication but optimized so basically it says (x * (64 * 1024) / (1024 * 1024)), x is already multiplied by 1024*/

			*buffer++ = colors[index & 0xff];

			tpos1 += 15;
			tpos2 += 13;
		}

		tpos4 += 13;
		tpos3 += 11;
	}

	pos1 +=9;
	pos3 +=8;
}
