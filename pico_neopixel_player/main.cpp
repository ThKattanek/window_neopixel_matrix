/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "hardware/pio.h"
#include "pico/time.h"
#include "ws2812.pio.h"

#include "../raspi_neopixel_player/plasma.h"
#include "../raspi_neopixel_player/cometrain.h"
#include "../raspi_neopixel_player/fire.h"
#include "../raspi_neopixel_player/game_of_life.h"
#include "../raspi_neopixel_player/pixeltest.h"

#define IS_RGBW false

#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN PICO_DEFAULT_WS2812_PIN
#else

// default to pin 2 if the board doesn't have a default WS2812 pin defined
#define WS2812_PIN 15
#endif

#define WIDTH		25
#define HEIGHT      30
#define LED_COUNT   (WIDTH * HEIGHT)
#define FPS			50					// 50 Frame per second
#define FRAME_TIMME (1000 / FPS)		// Frametime in ms
#define EFFECT_TIME	20					// for 20sek

enum effects{PIXELTEST, PLASMA, GAMEOFLIFE, FIRE, COMETRAIN, EFFECT_COUNT};

int current_effect = PIXELTEST;
int effect_time_counter = EFFECT_TIME * FPS;

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

void buffer_clear(uint32_t* buffer)
{
	for(int i=0; i<LED_COUNT; i++)
	{
		buffer[i] = 0x00000000;
	}
}

void buffer_to_matrix(uint32_t* buffer, uint32_t* matrix)
{
	int x = WIDTH-1;
	int y = HEIGHT-1;

	bool y_direction = false;

	for(int i=0; i<LED_COUNT; i++)
	{
		matrix[i] = buffer[y*WIDTH+x];

		if(y_direction)
		{
			y++;
			if(y>=HEIGHT)
			{
				y=HEIGHT-1;
				x--;
				y_direction = false;
			}
		}
		else
		{
			y--;
			if(y<0)
			{
				y=0;
				x--;
				y_direction = true;
			}
		}
	}
}

void matrix_to_leds(uint32_t* matrix)
{
	for(int i=0; i<LED_COUNT; i++)
		put_pixel(((matrix[i] & 0x000000ff) << 8) |
				  ((matrix[i] & 0x0000ff00) << 8) |
				  ((matrix[i] & 0x00ff0000) >> 16));
}

int main()
{
    // todo get free sm
	PIO pio = pio0;
	int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

	ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

	uint32_t* buffer = new uint32_t[WIDTH * HEIGHT];
	uint32_t* matrix = new uint32_t[WIDTH * HEIGHT];

	// init effects
	Plasma plasma(WIDTH, HEIGHT, buffer);
	plasma.Init();

	Cometrain cometrain(WIDTH, HEIGHT, buffer);
	cometrain.Init();

	Fire fire(WIDTH, HEIGHT, buffer);
	fire.Init();

	GameOfLife game_of_life(WIDTH, HEIGHT, buffer);

	PixelTest pixel_test(WIDTH, HEIGHT, buffer);
	pixel_test.Init();

	while (1)
	{
		buffer_clear(buffer);

		switch(current_effect)
		{
		case PLASMA:
			plasma.Render();
			break;

		case COMETRAIN:
			cometrain.Render();
			break;

		case FIRE:
			fire.Render();
			break;

		case GAMEOFLIFE:
			game_of_life.Render();
			break;

		case PIXELTEST:
			pixel_test.Render();
			break;
		}

		buffer_to_matrix(buffer, matrix);
		matrix_to_leds(matrix);

		effect_time_counter--;
		if(effect_time_counter == 0)
		{
			effect_time_counter = EFFECT_TIME * FPS;
			current_effect++;
			if(current_effect == EFFECT_COUNT)
				current_effect = PLASMA;
			switch(current_effect)
			{
			case PLASMA:
				plasma.Init();
				break;

			case COMETRAIN:
				cometrain.Init();
				break;

			case FIRE:
				fire.Init();
				break;

			case GAMEOFLIFE:
				game_of_life.Init();
				break;

			case PIXELTEST:
				pixel_test.Init();
				break;
			}
		}

		sleep_ms(20);
    }

	delete[] buffer;
	delete[] matrix;
}

