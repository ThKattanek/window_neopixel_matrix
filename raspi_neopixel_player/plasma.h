#ifndef PLASMA_H
#define PLASMA_H

#include <inttypes.h>

class Plasma
{
public:
	Plasma(int xw, int yw, uint32_t* buffer);
	~Plasma();

	void Init();
	void Render();

private:

	int xw, yw, led_count;
	uint32_t *buffer;

	uint16_t pos1 = 0, pos2 = 0, pos3 = 0, pos4 = 0, tpos1, tpos2, tpos3, tpos4;
	int aSin[512];
	uint32_t colors[256];
};

#endif // PLASMA_H
