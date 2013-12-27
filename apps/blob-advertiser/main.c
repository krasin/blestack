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

/*
 * ADV_NONCONN_IND PDU (39 octets):
 * +--------+--------+---------+--------+---------+------------+
 * | Header |  AdvA  | AD Size |  0x20  | Segment |    Page    |
 * +--------+--------+---------+--------+---------+------------+
 *  2 octets 6 octets  1 octet   1 octet  1 octet   0-28 octets
 */

#define AD_TYPE_BLOB			0x20
#define MAX_PAGE_LEN			28

#define PDU_LEN_POS			1
#define PDU_AD_LEN			8
#define PDU_SEG_DATA_POS		10
#define PDU_PAGE_POS			11

static const char data[] = "Hello world from INdT! (Instituto Nokia de Tecnologia)";

static uint8_t pdu[39] = { 0x42, 0x00, 0xAA, 0xBB, 0xCC,
			   0xDD, 0xEE, 0xFF, 0x00, 0x20 };

static uint8_t channels[] = { 37, 38, 39 };
static uint8_t idx = 0;

static uint8_t page = 0;
static uint8_t pages;

static int16_t adv_event;
static int16_t adv_interval;

void format_next_pdu(void)
{
	uint8_t plen = sizeof(data) - (page * MAX_PAGE_LEN);

	if (plen > MAX_PAGE_LEN)
		plen = MAX_PAGE_LEN;

	pdu[PDU_LEN_POS] = 9 + plen;
	pdu[PDU_AD_LEN] = 1 + plen;
	pdu[PDU_SEG_DATA_POS] = ((page + 1) << 4) | pages;

	memcpy(pdu + PDU_PAGE_POS, data + (page * MAX_PAGE_LEN), plen);
}

void adv_interval_timeout(void *user_data)
{
	radio_send(channels[idx++], ADV_CHANNEL_AA, ADV_CHANNEL_CRC, pdu,
								sizeof(pdu));

	if (idx < 3)
		timer_start(adv_interval, ADV_INTERVAL, NULL);
}

void adv_event_timeout(void *user_data)
{
	page = (page + 1) % pages;
	format_next_pdu();

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

	pages = (sizeof(data) / MAX_PAGE_LEN)
				+ ((sizeof(data) % MAX_PAGE_LEN) ? 1 : 0);

	DBG("Long data advertisement with:");
	DBG("Time between PDUs: %u ms", ADV_INTERVAL);
	DBG("T_advEvent:        %u ms", ADV_EVENT);
	DBG("Data length:       %u", sizeof(data));
	DBG("Number of pages:   %u", pages);

	format_next_pdu();
	radio_send(channels[idx], ADV_CHANNEL_AA, ADV_CHANNEL_CRC, pdu,
								sizeof(pdu));
	timer_start(adv_interval, ADV_INTERVAL, NULL);
	timer_start(adv_event, ADV_EVENT, NULL);

	while (1);

	return 0;
}
