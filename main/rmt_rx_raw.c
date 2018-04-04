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

// LED on Huzzah32 board
const int LED_BUILTIN = 13;

#define RMT_RX_CHANNEL 0
#define rrmt_item32_tIMEOUT_US  9500   /*!< RMT receiver timeout value */

// RMT receiver initialization
static void rx_init()
{
    rmt_config_t rmt_rx;
    
    rmt_rx.channel       = RMT_RX_CHANNEL;
    rmt_rx.gpio_num      = 21;
    rmt_rx.clk_div       = 80;
    rmt_rx.mem_block_num = 1;
    rmt_rx.rmt_mode      = RMT_MODE_RX;
    
    rmt_rx.rx_config.filter_en = true;
    rmt_rx.rx_config.filter_ticks_thresh = 100;
    rmt_rx.rx_config.idle_threshold = 12000;
    
    rmt_config(&rmt_rx);
    rmt_driver_install(rmt_rx.channel, 1000, 0);
    
    // set up visible led on board
    gpio_pad_select_gpio(LED_BUILTIN);
	gpio_set_direction(LED_BUILTIN, GPIO_MODE_OUTPUT);
}

// Converts the RMT level, duration into a positive or negative integer
// Compatible with the ESP32-RMT-server application
// Note: most IR receivers have active-low outputs, where the
//   ESP32-RMT-server application has active-high oututs
// This function inverts the RMT receive level so the text output is
//   compatible with ESP32-RMT-server application
int duration( uint32_t level, uint32_t duration ) {
	if ( level == 0 ) { return duration; }
	else { return -1.0 * duration; }
}

// RMT receiver task
static void rmt_example_nec_rx_task()
{
    int channel = RMT_RX_CHANNEL;
    int i;
    rx_init();
    RingbufHandle_t rb = NULL;
    // get RMT RX ringbuffer
    rmt_get_ringbuf_handle(channel, &rb);
    rmt_rx_start(channel, 1);
    while(rb) {
        size_t rx_size = 0;
        //try to receive data from ringbuffer.
        //RMT driver will push all the data it receives to its ringbuffer.
        rmt_item32_t* items = (rmt_item32_t*) xRingbufferReceive(rb, &rx_size, 1000);
        if(items) {
			// turn on visible led
			gpio_set_level(LED_BUILTIN,1);
			
			// print the RMT received durations to the monitor
			printf( "  Received %i items\n", rx_size/4 );
            for ( i=0; i<rx_size/4; i++ ) {
				if ( i>0 ) { printf(","); }
				printf("%i", duration( items[i].level0, items[i].duration0 ) );
				printf(",%i", duration( items[i].level1, items[i].duration1 ) );
			}
			printf("\n");
			
            //after parsing the data, return spaces to ringbuffer.
            vRingbufferReturnItem(rb, (void*) items);
            
			// turn off visible led
			gpio_set_level(LED_BUILTIN,0);
        } else {
			printf("No items\n");
            break;
        }
    }
    vTaskDelete(NULL);
}

void app_main()
{
    xTaskCreate(rmt_example_nec_rx_task, "rmt_nec_rx_task", 2048, NULL, 10, NULL);
}
