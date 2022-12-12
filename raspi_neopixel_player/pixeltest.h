//////////////////////////////////////////////////
//                                              //
// SDL2 Render Threading                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: pixeltest.h                           //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 27.11.2022                //
//                                              //
//                                              //
//////////////////////////////////////////////////

#ifndef PIXELTEST_H
#define PIXELTEST_H


#include <cstdint>
class PixelTest
{
public:
	PixelTest(int xw, int yw, uint32_t *buffer);
	~PixelTest();

	void Init();
	void Render();

private:
	int xw, yw, led_count;
	uint32_t *buffer;
	
	int counter;
	uint32_t colors[3];
	int color;
};

#endif // PIXELTEST_H
