#include "fire.h"
#include <cstdlib>

Fire::Fire(int xw, int yw, uint32_t *buffer)
{
	this->xw = xw;
	this->yw = yw;
	this->buffer = buffer;

	led_count = xw * yw;

	fire = nullptr;

	fire = new uint8_t [xw * yw];
}

Fire::~Fire()
{
	if(fire != nullptr) delete [] fire;
}

void Fire::Init()
{
	// create fire palette
		for (int i = 0; i < 32; ++i)
		{
			/* black to blue, 32 values*/
			colors[i] = (i << 1) << 16; // blue

			/* blue to red, 32 values*/
			colors[i + 32] = (i << 3); // red
			colors[i + 32] |=  (64 - (i << 1)) << 16; // blue

			/*red to yellow, 32 values*/
			colors[i + 64] = 255;	// red
			colors[i + 64] |= (i << 3) << 8; // green

			/* yellow to white, 162 */
			colors[i + 96] = 255;	// red
			colors[i + 96] |= 255 << 8; // green
			colors[i + 96] |= (i << 2) << 16; // blue

			colors[i + 128] = 255 ; // red
			colors[i + 128] |= 255 << 8; // green
			colors[i + 128] |= (64 + (i << 2)) << 16; // blue

			colors[i + 160] = 255 ; // red
			colors[i + 160] |= 255 << 8; // green
			colors[i + 160] |= (128 + (i << 2)) << 16; // blue

			colors[i + 192] = 255 ; // red
			colors[i + 192] |= 255 << 8; // green
			colors[i + 192] |= (192 + i) << 16; // blue

			colors[i + 224] = 255 ; // red
			colors[i + 224] |= 255 << 8; // green
			colors[i + 224] |= (224 + i) << 16; // blue
		}
}

void Fire::Render()
{
	float temp;

	// draw random bottom line in fire array
	int j = xw * (yw - 1);

	for(int i = 0; i < xw ; i++)
	{
		int random = 1 + (int)(16.0 * (rand()/(RAND_MAX+1.0)));
		if (random > 9) /* the lower the value, the intenser the fire, compensate a lower value with a higher decay value*/
			fire[j + i] = 255; /*maximum heat*/
		else
			fire[j + i] = 0;
	}

	// move fire upwards, start at bottom
	for (uint8_t index = 0; index < yw-1 ; ++index) // 60
	{
		for (int i = 0; i < xw ; ++i)
		{
			if (i == 0) /* at the left border*/
			{
				temp = fire[j];
				temp += fire[j + 1];
				temp += fire[j - xw];
				temp /=3;
			}
			else if (i == xw - 1) /* at the right border*/
			{
				temp = fire[j + i];
				temp += fire[j - xw + i];
				temp += fire[j + i - 1];
				temp /= 3;
			}
			else
			{
				temp = fire[j + i];
				temp += fire[j + i + 1];
				temp += fire[j + i - 1];
				temp += fire[j - xw + i];
				temp /= 4.04;
			}
			if (temp > 1)
				temp -= 1.2; /* decay */

			fire[j - xw + i] = temp;
		}
		j -= xw;
	}

	// draw fire array to screen from bottom to top + 300

	uint32_t *image = buffer + (xw * yw);

	for(int i=0; i<led_count - xw; i++)
		buffer[i + xw] = colors[fire[i]];


	/* for (int i = yw - 3; i >= 0; --i)
	{
		for (int j = xw - 1; j >= 0; --j)
		{
			*image = colors[fire[i * xw + j]];
			image--;
		}
	}
	*/
}
