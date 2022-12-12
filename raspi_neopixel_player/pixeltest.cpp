//////////////////////////////////////////////////
//                                              //
// SDL2 Render Threading                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: pixeltest.cpp                         //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 27.11.2022                //
//                                              //
//                                              //
//////////////////////////////////////////////////

#include "pixeltest.h"

PixelTest::PixelTest(int xw, int yw, uint32_t *buffer)
{
	this->xw = xw;
	this->yw = yw;
	this->buffer = buffer;

	led_count = xw * yw;

	counter = 50*1;
	color = 0;

	colors[0] = 0x000000ff;	// rot
	colors[1] = 0x0000ff00;	// grün
	colors[2] = 0x00ff0000;	// blau
}

PixelTest::~PixelTest()
{

}

void PixelTest::Init()
{

}

void PixelTest::Render()
{
	for(int i=0; i<led_count; i++)
		buffer[i] = colors[color];

	counter--;
	if(counter == 0)
	{
		counter = 50;
		color++;
		if(color == 3)
			color = 0;
	}
}
