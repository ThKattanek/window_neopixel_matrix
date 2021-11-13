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
};

#endif // PLASMA_H
