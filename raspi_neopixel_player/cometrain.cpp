#include "cometrain.h"

Cometrain::Cometrain(int xw, int yw, uint32_t *buffer)
{
	this->xw = xw;
	this->yw = yw;
	this->buffer = buffer;

	led_count = xw * yw;
}

Cometrain::~Cometrain()
{

}

void Cometrain::Init()
{
	for(int i=0; i<COMET_COUNT; i++)
		BornComet(c1[i]);
}

void Cometrain::Render()
{
	for(int i=0; i<COMET_COUNT; i++)
		DrawComet(c1[i]);
}

void Cometrain::DrawPixel(int x, int y, uint32_t color)
{
	if(x < 0 || x >= xw) return;
	if(y < 0 || y >= yw) return;

	buffer[y * xw + x] = color;
}

void Cometrain::BornComet(comet &c)
{
	c.color = 0x00ffffff;
	c.posy = 0;
	c.posx = rand() % xw;
	c.life = 1.0f;
	c.life_speed = ((rand() % 5) + 1) / 100.0f;
	c.len = (rand() % 5) + 5;
	c.speed = ((rand() % 2) + 1) - 0.5f;
	c.dead = false;
}

bool Cometrain::DrawComet(comet &c)
{
	if(c.dead) return true;

	uint8_t r = c.color & 0xff;
	uint8_t g = (c.color >> 8) & 0xff;
	uint8_t b = (c.color >> 16) & 0xff;

	uint8_t r1 = r * c.life;
	uint8_t g1 = g * c.life;
	uint8_t b1 = b * c.life;

	uint32_t color;

	float f = c.life / c.len;
	for(int i = 0; i < c.len; i++)
	{
		r1 = r * f * (c.len - i);
		g1 = g * f * (c.len - i);
		b1 = b * f * (c.len - i);

		if(i>0)
		{
			r1 *= 0.4f;
			g1 *= 0.4f;
			b1 *= 0.4f;
		}

		color = r1 | g1 << 8 | b1 << 16;

		DrawPixel(c.posx, c.posy - i, color);
	}

	c.posy += c.speed;

	c.life -= c.life_speed;
	if(c.life <= 0.0f)
	{
		BornComet(c);
		return true;
	}
	return false;
}
