/**
 *  The MIT License (MIT)
 *
 *  Copyright (c) 2013 Paulo Sérgio Borges de Oliveira Filho
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#include "radio.h"
#include "log.h"

#define PDU_MAX_LEN			39
#define SCAN_INTERVAL_MS		2000
#define SCAN_WINDOW_MS			1000
#define ACCESS_ADDRESS			0x8E89BED6UL
#define CRC_INIT_ADV			0x555555UL

#define DUMP_RAW_DATA(data, len)					\
	do {								\
		for (i = 0; i < len; i++) {				\
			log_uart("0x%02X ", data[i]);			\
		}							\
		log_uart("\r\n");					\
	} while (0)

static uint16_t timer_scan_window, timer_scan_interval;
static uint8_t pdu[PDU_MAX_LEN];

static uint16_t channels = { 37, 38, 39 };
static uint8_t curr = 0;

void stop_scan(void *userdata)
{
	radio_off();
}

void handle_packet(uint8_t *data, uint8_t len, void *user_data)
{
	stop_scan();

	DBG("Received packet: ");
	DUMP_RAW_DATA(data, len);
}

void scan(void *user_data)
{
	timer_start(&timer_scan_interval, SCAN_INTERVAL_MS, scan, NULL);
	timer_start(&timer_scan_window, SCAN_WINDOW_MS, stop_scan, NULL);

	radio_recv(channels[curr], ACCESS_ADDRESS, CRC_INIT_ADV, pdu,
					sizeof(pdu), handle_packet, NULL);

	curr = (curr + 1) % sizeof(channels);
}

int main(void)
{
	radio_init();

	scan();

	while (1);

	return 0;
}
