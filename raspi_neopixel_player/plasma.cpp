#include "plasma.h"

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
	int c = 0;
	int b = 0;

	for(int i=0; i<led_count; i++)
	{
		switch(b)
		{
		case 0:
			buffer[i] = 0x0000ff00;
			break;
		case 1:
			buffer[i] = 0x000000ff;
			break;
		case 2:
			buffer[i] = 0x00ff0000;
			break;
		default:
			break;
		}

		c++;
		if(c == 5)
		{
			b++;
			if(b == 3) b = 0;
			c = 0;
		}
	}
}

void Plasma::Render()
{
	uint32_t tmp = buffer[0];

	for(int i=0; i<led_count; i++)
	{
		buffer[i] = buffer[i+1];
	}
	buffer[led_count-1] = tmp;
}
