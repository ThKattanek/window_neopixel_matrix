/*
 * newtest.c
 *
 * Copyright (c) 2014 Jeremy Garff <jer @ jers.net>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 *     1.  Redistributions of source code must retain the above copyright notice, this list of
 *         conditions and the following disclaimer.
 *     2.  Redistributions in binary form must reproduce the above copyright notice, this list
 *         of conditions and the following disclaimer in the documentation and/or other materials
 *         provided with the distribution.
 *     3.  Neither the name of the owner nor the names of its contributors may be used to endorse
 *         or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

static char VERSION[] = "XX.YY.ZZ";

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <stdarg.h>
#include <getopt.h>

#include <ws2811/clk.h>
#include <ws2811/gpio.h>
#include <ws2811/dma.h>
#include <ws2811/pwm.h>

#include "version.h"

#include <ws2811/ws2811.h>

// effects
#include "./plasma.h"
#include "./cometrain.h"
#include "./pictureshow.h"
#include "./fire.h"
#include "./pixeltest.h"

#define ARRAY_SIZE(stuff)       (sizeof(stuff) / sizeof(stuff[0]))

// defaults for cmdline options
#define TARGET_FREQ             WS2811_TARGET_FREQ
#define GPIO_PIN                18
#define DMA                     10
//#define STRIP_TYPE            WS2811_STRIP_RGB		// WS2812/SK6812RGB integrated chip+leds
#define STRIP_TYPE              WS2811_STRIP_GBR		// WS2812/SK6812RGB integrated chip+leds
//#define STRIP_TYPE            SK6812_STRIP_RGBW		// SK6812RGBW (NOT SK6812RGB)

#define WIDTH                   25
#define HEIGHT                  30
#define LED_COUNT               (WIDTH * HEIGHT)
#define EFFECT_TIME		5// for 20sek
#define FPS			50	// 50 Frame per second

int width = WIDTH;
int height = HEIGHT;
int led_count = LED_COUNT;

int clear_on_exit = 0;

enum effects{PLASMA, FIRE, COMETRAIN, PICTURESHOW, PIXELTEST, EFFECT_COUNT};

int current_effect = PIXELTEST;
int effect_time_counter = EFFECT_TIME * FPS;

ws2811_t ledstring =
{
	.freq = TARGET_FREQ,
	.dmanum = DMA,
	.channel =
	{
		[0] =
		{
			GPIO_PIN,
            0,
            LED_COUNT,
            STRIP_TYPE,
            nullptr,
			100
		},
		[1] =
		{
			0,
            0,
			0,
            0,
            0,
			0
		},
	},
};

ws2811_led_t *matrix;
ws2811_led_t *buffer;

static uint8_t running = 1;

void buffer_clear()
{
	for(int i=0; i<LED_COUNT; i++)
	{
		buffer[i] = 0x00000000;
	}
}

void buffer_to_matrix()
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

void matrix_to_leds(void)
{

	int x, y;

	for (x = 0; x < width; x++)
	{
		for (y = 0; y < height; y++)
		{
			ledstring.channel[0].leds[(y * width) + x] = matrix[(y * width) + x];
		}
	}
}

void matrix_clear(void)
{
	int x, y;

	for (y = 0; y < (height ); y++)
	{
		for (x = 0; x < width; x++)
		{
			matrix[y * width + x] = 0x000000ff;
		}
	}
}

static void ctrl_c_handler(int signum)
{
	(void)(signum);
	running = 0;
}

static void setup_handlers(void)
{
    /*
	struct sigaction sa =
	{
		.sa_handler = ctrl_c_handler,
	};

	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	*/
}


void parseargs(int argc, char **argv, ws2811_t *ws2811)
{
	int index;
	int c;

	static struct option longopts[] =
	{
		{"help", no_argument, 0, 'h'},
		{"dma", required_argument, 0, 'd'},
		{"gpio", required_argument, 0, 'g'},
		{"invert", no_argument, 0, 'i'},
		{"clear", no_argument, 0, 'c'},
		{"strip", required_argument, 0, 's'},
		{"height", required_argument, 0, 'y'},
		{"width", required_argument, 0, 'x'},
		{"version", no_argument, 0, 'v'},
		{0, 0, 0, 0}
	};

	while (1)
	{

		index = 0;
		c = getopt_long(argc, argv, "cd:g:his:vx:y:", longopts, &index);

		if (c == -1)
			break;

		switch (c)
		{
		case 0:
			/* handle flag options (array's 3rd field non-0) */
			break;

		case 'h':
			fprintf(stderr, "%s version %s\n", argv[0], VERSION);
			fprintf(stderr, "Usage: %s \n"
				"-h (--help)    - this information\n"
				"-s (--strip)   - strip type - rgb, grb, gbr, rgbw\n"
				"-x (--width)   - matrix width (default 8)\n"
				"-y (--height)  - matrix height (default 8)\n"
				"-d (--dma)     - dma channel to use (default 10)\n"
				"-g (--gpio)    - GPIO to use\n"
				"                 If omitted, default is 18 (PWM0)\n"
				"-i (--invert)  - invert pin output (pulse LOW)\n"
				"-c (--clear)   - clear matrix on exit.\n"
				"-v (--version) - version information\n"
				, argv[0]);
			exit(-1);

		case 'D':
			break;

		case 'g':
			if (optarg) {
				int gpio = atoi(optarg);
/*
	PWM0, which can be set to use GPIOs 12, 18, 40, and 52.
	Only 12 (pin 32) and 18 (pin 12) are available on the B+/2B/3B
	PWM1 which can be set to use GPIOs 13, 19, 41, 45 and 53.
	Only 13 is available on the B+/2B/PiZero/3B, on pin 33
	PCM_DOUT, which can be set to use GPIOs 21 and 31.
	Only 21 is available on the B+/2B/PiZero/3B, on pin 40.
	SPI0-MOSI is available on GPIOs 10 and 38.
	Only GPIO 10 is available on all models.

	The library checks if the specified gpio is available
	on the specific model (from model B rev 1 till 3B)

*/
				ws2811->channel[0].gpionum = gpio;
			}
			break;

		case 'i':
			ws2811->channel[0].invert=1;
			break;

		case 'c':
			clear_on_exit=1;
			break;

		case 'd':
			if (optarg) {
				int dma = atoi(optarg);
				if (dma < 14) {
					ws2811->dmanum = dma;
				} else {
					printf ("invalid dma %d\n", dma);
					exit (-1);
				}
			}
			break;

		case 'y':
			if (optarg) {
				height = atoi(optarg);
				if (height > 0) {
					ws2811->channel[0].count = height * width;
				} else {
					printf ("invalid height %d\n", height);
					exit (-1);
				}
			}
			break;

		case 'x':
			if (optarg) {
				width = atoi(optarg);
				if (width > 0) {
					ws2811->channel[0].count = height * width;
				} else {
					printf ("invalid width %d\n", width);
					exit (-1);
				}
			}
			break;

		case 's':
			if (optarg) {
				if (!strncasecmp("rgb", optarg, 4)) {
					ws2811->channel[0].strip_type = WS2811_STRIP_RGB;
				}
				else if (!strncasecmp("rbg", optarg, 4)) {
					ws2811->channel[0].strip_type = WS2811_STRIP_RBG;
				}
				else if (!strncasecmp("grb", optarg, 4)) {
					ws2811->channel[0].strip_type = WS2811_STRIP_GRB;
				}
				else if (!strncasecmp("gbr", optarg, 4)) {
					ws2811->channel[0].strip_type = WS2811_STRIP_GBR;
				}
				else if (!strncasecmp("brg", optarg, 4)) {
					ws2811->channel[0].strip_type = WS2811_STRIP_BRG;
				}
				else if (!strncasecmp("bgr", optarg, 4)) {
					ws2811->channel[0].strip_type = WS2811_STRIP_BGR;
				}
				else if (!strncasecmp("rgbw", optarg, 4)) {
					ws2811->channel[0].strip_type = SK6812_STRIP_RGBW;
				}
				else if (!strncasecmp("grbw", optarg, 4)) {
					ws2811->channel[0].strip_type = SK6812_STRIP_GRBW;
				}
				else {
					printf ("invalid strip %s\n", optarg);
					exit (-1);
				}
			}
			break;

		case 'v':
			fprintf(stderr, "%s version %s\n", argv[0], VERSION);
			exit(-1);

		case '?':
			/* getopt_long already reported error? */
			exit(-1);

		default:
			exit(-1);
		}
	}
}

int main(int argc, char *argv[])
{
	ws2811_return_t ret;

	sprintf(VERSION, "%d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);

	parseargs(argc, argv, &ledstring);

	buffer = (ws2811_led_t*)malloc(sizeof(ws2811_led_t) * width * height);
	matrix = (ws2811_led_t*)malloc(sizeof(ws2811_led_t) * width * height);

	setup_handlers();

	if ((ret = ws2811_init(&ledstring)) != WS2811_SUCCESS)
	{
		fprintf(stderr, "ws2811_init failed: %s\n", ws2811_get_return_t_str(ret));
		return ret;
	}
	
	// init effects
	Plasma plasma(WIDTH, HEIGHT, buffer);
    plasma.Init();

	Cometrain cometrain(WIDTH, HEIGHT, buffer);
	cometrain.Init();

	PictureShow pictureshow(WIDTH, HEIGHT, buffer);
	pictureshow.LoadPNG("/home/pi/neopixel_images/baum3.png");

	Fire fire(WIDTH, HEIGHT, buffer);
	fire.Init();

	PixelTest pixel_test(WIDTH, HEIGHT, buffer);
	pixel_test.Init();

	while (running)
	{
		buffer_clear();

		// render effects

		switch(current_effect)
		{
		case PLASMA:
			plasma.Render();
			break;

		case COMETRAIN:
			cometrain.Render();
			break;

		case PICTURESHOW:
			pictureshow.Render();
			break;
		case FIRE:
			fire.Render();
			break;

		case PIXELTEST:
			pixel_test.Render();
			break;
		}

		effect_time_counter--;
		if(effect_time_counter == 0)
		{
			effect_time_counter = EFFECT_TIME * FPS;
			current_effect++;
			if(current_effect == EFFECT_COUNT)
				current_effect = PLASMA;
		}

		buffer_to_matrix();
		matrix_to_leds();
		if ((ret = ws2811_render(&ledstring)) != WS2811_SUCCESS)
		{
			fprintf(stderr, "ws2811_render failed: %s\n", ws2811_get_return_t_str(ret));
			break;
		}

		// 15 frames /sec
		usleep(1000000 / FPS);
	}

	if (clear_on_exit) {

	matrix_clear();
	ws2811_render(&ledstring);

	}

	ws2811_fini(&ledstring);

	printf ("\n");
	return ret;
}
