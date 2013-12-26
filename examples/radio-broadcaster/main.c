/**
 *  The MIT License (MIT)
 *
 *  Copyright (c) 2013 Paulo B. de Oliveira Filho <pauloborgesfilho@gmail.com>
 *  Copyright (c) 2013 Claudio Takahasi <claudio.takahasi@gmail.com>
 *  Copyright (c) 2013 João Paulo Rechi Vita <jprvita@gmail.com>
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

#include <string.h>
#include <stdint.h>

#include "timer.h"
#include "radio.h"
#include "log.h"

#define ADV_CHANNEL_AA			0x8E89BED6
#define ADV_CHANNEL_CRC			0x555555

#define ADV_EVENT			100
#define ADV_INTERVAL			5

static const uint8_t pdu[] = {	0x42, 0x06, 0xAA, 0xBB,
				0xCC, 0xDD, 0xEE, 0xFF };

static uint8_t channels[] = { 37, 38, 39 };
static uint8_t idx = 0;

static int16_t adv_event;
static int16_t adv_interval;

void adv_interval_timeout(void *user_data)
{
	radio_send(channels[idx++], ADV_CHANNEL_AA, ADV_CHANNEL_CRC, pdu,
								sizeof(pdu));

	if (idx < 3)
		timer_start(adv_interval, ADV_INTERVAL, NULL);
}

void adv_event_timeout(void *user_data)
{
	idx = 0;

	radio_send(channels[idx++], ADV_CHANNEL_AA, ADV_CHANNEL_CRC, pdu,
								sizeof(pdu));
	timer_start(adv_interval, ADV_INTERVAL, NULL);
}

int main(void)
{
	log_init();
	timer_init();
	radio_init();

	adv_interval = timer_create(TIMER_SINGLESHOT, adv_interval_timeout);
	adv_event = timer_create(TIMER_REPEATED, adv_event_timeout);

	DBG("Advertising with:");
	DBG("Time between PDUs: %u ms", ADV_INTERVAL);
	DBG("T_advEvent:        %u ms", ADV_EVENT);

	radio_send(channels[idx], ADV_CHANNEL_AA, ADV_CHANNEL_CRC, pdu,
								sizeof(pdu));
	timer_start(adv_interval, ADV_INTERVAL, NULL);
	timer_start(adv_event, ADV_EVENT, NULL);

	while (1);

	return 0;
}
