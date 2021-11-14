#ifndef COMETRAIN_H
#define COMETRAIN_H

#include <inttypes.h>
#include <iostream>

#define COMET_COUNT 20

using namespace std;

struct comet{
	int posx;
	float posy;
	uint32_t color;
	float life;
	float life_speed;
	int len;
	float speed;
	bool dead;
};

class Cometrain
{
public:
	Cometrain(int xw, int yw, uint32_t* buffer);
	~Cometrain();

	void Init();
	void Render();

private:
	void DrawPixel(int x, int y, uint32_t color);
	void BornComet(comet &c);
	bool DrawComet(comet &c);

	int xw, yw, led_count;
	uint32_t *buffer;

	comet c1[COMET_COUNT];
};

#endif // COMETRAIN_H
