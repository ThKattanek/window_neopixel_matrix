#ifndef FIRE_H
#define FIRE_H

#include<inttypes.h>

class Fire
{
public:
	Fire(int xw, int yw, uint32_t* buffer);
	~Fire();

	void Init();
	void Render();

private:

	int xw, yw, led_count;
	uint32_t *buffer;

	uint8_t *fire;
	uint32_t colors[256];
};

#endif // FIRE_H
