/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"


// define GPIO pin to use
#define WS2812_PIN 28

#define LED_COUNT 12
/*
uint32_t colors[LED_COUNT] = 
{
    0x00ACCC,
    0x1D5DC5,
    0x541ECB,
    0x9C1CEE,
    0xB30D8B,
    0xBC1303,
    0xD6AB01,
    0xBEFE00,
    0x69FF03,
    0x26FF00,
    0x00FF19,
    0x00FF00
};

*/

// array of color values in RGB order, increasing intensity/brightness
uint32_t colors[LED_COUNT] = 
{
    0x001000,
    0x100000,
    0x000010,
    0x008000,
    0x800000,
    0x000080,
    0x00c000,
    0xc00000,
    0x0000c0,
    0x00ff00,
    0xFF0000,
    0x0000FF
};

uint32_t led_buffer[LED_COUNT];


int main() {

    stdio_init_all();
    printf("WS2812 Test, using pin %d", WS2812_PIN);

    // initialise the used GPIO pin to LOW
    gpio_put(WS2812_PIN, false);

    // wait for the WS2812s to reset
    sleep_us(50);

    // Choose which PIO instance to use (there are two instances)
    PIO pio = pio0;

    // Our assembled program needs to be loaded into this PIO's instruction
    // memory. This SDK function will find a location (offset) in the
    // instruction memory where there is enough space for our program. We need
    // to remember this location!
    uint offset = pio_add_program(pio, &ws2812_program);

    // Find a free state machine on our chosen PIO (erroring if there are
    // none). Configure it to run our program, and start it, using the
    // helper function we included in our .pio file.
    uint sm = pio_claim_unused_sm(pio, true);
    ws2812_program_init(pio, sm, offset, WS2812_PIN);
    

    // The state machine is now running. Any value we push to its TX FIFO will
    // appear on the LED pin.  
    
    uint32_t first = 0;

    while (true) 
    {
        // fill buffer with color values
        for (int i = 0; i < LED_COUNT; i++)
        {
            led_buffer[i] = colors[(i + first) % LED_COUNT];
        }

        // put values into TX FIFO
        for (int i=0; i < LED_COUNT; i++)
            pio_sm_put_blocking(pio, sm, led_buffer[i]);

        sleep_ms(1000);

        // set index of first color
        first = (first + 1) % LED_COUNT;
    }


}
