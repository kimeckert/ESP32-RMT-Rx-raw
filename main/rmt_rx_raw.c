/* RMT receive example, displaying raw RMT data
 * 
 * This code was adapted from example: rmt_nec_tx_rx
 * That software is distributed under Public Domain (or CC0 licensed, at your option.)
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "driver/periph_ctrl.h"
#include "soc/rmt_reg.h"

// FreeRTOS function
#define INCLUDE_vTaskDelay 1

#define rrmt_item32_tIMEOUT_US  9500   /*!< RMT receiver timeout value */

// LED on Huzzah32 board
const int LED_BUILTIN = 13;

// structure used to initialize RMT inputs
// NOTE: tag is used on the monitor outputs to distinguish between channels
struct rmt_rx_inits {
	rmt_channel_t  channel;
	gpio_num_t     gpio_num;
	uint8_t        clk_div;
	uint8_t        mem_block_num;
	bool           config_filter_en;
	uint8_t        config_filter_tics_thresh;
	uint16_t       config_idle_threshold;
	char           tag[10];
}rx_inputs[2] = {
	{ 0, 21, 80, 1, false, 0, 50000, "Ch0" },
	{ 1, 14, 80, 1, false, 0, 50000, "Ch1" }
};

// initialize RMT receive channels
void rx_channels_init() {
    rmt_config_t rmt_rx;
    size_t i;
    size_t len = sizeof(rx_inputs) / sizeof( rx_inputs[0] );
    
    for ( i=0; i<len; i++ ) {
		rmt_rx.channel       = rx_inputs[i].channel;
		rmt_rx.gpio_num      = rx_inputs[i].gpio_num;
		rmt_rx.clk_div       = rx_inputs[i].clk_div;
		rmt_rx.mem_block_num = rx_inputs[i].mem_block_num;
		rmt_rx.rmt_mode      = RMT_MODE_RX;
		
		rmt_rx.rx_config.filter_en           = rx_inputs[i].config_filter_en;
		rmt_rx.rx_config.filter_ticks_thresh = rx_inputs[i].config_filter_tics_thresh;
		rmt_rx.rx_config.idle_threshold      = rx_inputs[i].config_idle_threshold;
		
		rmt_config(&rmt_rx);
		rmt_driver_install(rx_inputs[i].channel, 1000, 0);
	}
}

// initialize visible LED on ESP32 board
static void visible_led_init() {
    gpio_pad_select_gpio(LED_BUILTIN);
	gpio_set_direction(LED_BUILTIN, GPIO_MODE_OUTPUT);
}

/* Converts the RMT level, duration into a positive or negative integer
 * Compatible with the ESP32-RMT-server application
 * Note: most IR receivers have active-low outputs, where the
 *   ESP32-RMT-server application has active-high oututs
 * This function inverts the RMT receive level so the text output is
 *   compatible with ESP32-RMT-server application
 */
int dur( uint32_t level, uint32_t duration ) {
	if ( level == 0 ) { return duration; }
	else { return -1.0 * duration; }
}

// RMT receiver task
static void rmt_example_nec_rx_task() {
	size_t num_channels = sizeof(rx_inputs) / sizeof( rx_inputs[0] );
	size_t c, i;
    size_t rx_size = 0;
    rmt_item32_t* items = NULL;
	
    // define ringbuffer handles
    RingbufHandle_t rb[num_channels];
	
    // start receiving IR data, initialize ringbuffer handles
    for ( c=0; c<num_channels; c++ ) {
		rmt_rx_start(rx_inputs[c].channel, 1);
		rmt_get_ringbuf_handle(rx_inputs[c].channel, &rb[c]);
	}
    
    // loop forever
    while (1) {
		// check each receive channel
		for ( c=0; c<num_channels; c++ ) {
			items = (rmt_item32_t*) xRingbufferReceive(rb[c], &rx_size, 10);
			if(items) {
				// turn on visible led
				gpio_set_level(LED_BUILTIN, 1);
				
				// print the RMT received durations to the monitor
				printf( "  %s received %i items\n", rx_inputs[c].tag, rx_size/4 );
				for ( i=0; i<rx_size/4; i++ ) {
					if ( i>0 ) { printf(","); }
					printf( "%i", dur( items[i].level0, items[i].duration0 ) );
					printf(",%i", dur( items[i].level1, items[i].duration1 ) );
				}
				printf("\n");
				
				// turn off visible led
				gpio_set_level(LED_BUILTIN, 0);
				
				// free up data space
				vRingbufferReturnItem(rb[c], (void*) items);
			}
		}
		// delay 100 milliseconds. No need to overheat the processor
		vTaskDelay( 100 / portTICK_PERIOD_MS );
	}
}

void app_main() {
	// initialize hardware
    rx_channels_init();
    visible_led_init();
    
    // start receive processing task
    xTaskCreate(rmt_example_nec_rx_task, "rmt_nec_rx_task", 2048, NULL, 10, NULL);
}
