/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>

#include "hardware/pio.h"
#include "hardware/adc.h"
#include "pico/stdio.h"
#include "pico/time.h"
#include "ws2812.pio.h"

#define IS_RGBW false
#define NUM_PIXELS 4

#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN PICO_DEFAULT_WS2812_PIN
#else

// default to pin 2 if the board doesn't have a default WS2812 pin defined
#define WS2812_PIN 3
#endif

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

int main() {

    stdio_init_all();
    printf("WS2812 Smoke Test, using pin %d", WS2812_PIN);

    // todo get free sm
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

	ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);


	// ADC init
	adc_init();
	adc_gpio_init(26);

	// Select ADC input 0 (GPIO26)
	adc_select_input(0);

	float brightness = 0;
	const float conversion_factor = 1.0f / (1 << 12);

	while (1)
	{
		uint16_t result = adc_read();
		brightness = result * conversion_factor;
		if(brightness > 1.0f)
			brightness = 1.0f;
		if(brightness < 0.0f)
			brightness = 0.0f;


		//brightness = 1.0f;

		uint8_t r = 0xff * brightness;
		uint8_t g = 0xff * brightness;
		uint8_t b = 0xff * brightness;

		for(int i = 0; i<216; i++)
		{

		put_pixel(g << 16);	//g
		put_pixel(r << 8);	//r
		put_pixel(b);	//b
		}

		sleep_ms(20);

    }
}

