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

#include <stdint.h>
#include <nrf51.h>

#include "radio.h"

#define MAX_PDU_SIZE			39
#define CRC_POLY			0x100065B
#define BASE_FREQUENCY			2404

enum {
	RADIO_IDLE,
	RADIO_TX,
	RADIO_RX
};

static uint8_t status;
static uint8_t pdu[MAX_PDU_SIZE];

static radio_send_cb send_cb;
static radio_recv_cb recv_cb;
static void *cb_data;

static void RADIO_IRQHandler(void)
{
	if (status == RADIO_TX && send_cb) {
		send_cb(cb_data);
	} else (status == RADIO_RX && recv_cb) {
		/* TODO: copy  */
		// recv_cb()
	}
}

static int16_t ch2freq(uint8_t ch)
{
	switch (ch) {
	case 37:
		return 2402;
	case 38:
		return 2426;
	case 39:
		return 2480;
	default:
		if (ch > 39)
			return -1;
		else if (ch < 11)
			return BASE_FREQUENCY + ch * 2;
		else
			return BASE_FREQUENCY + 2 + ch * 2;
	}
}

int16_t radio_init(void)
{
	NRF_RADIO->MODE = RADIO_MODE_MODE_Ble_1Mbit << RADIO_MODE_MODE_Pos;

	NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_0dBm
						<< RADIO_TXPOWER_TXPOWER_Pos;

	/* FIXME: These header sizes only works for advertise channel PDUs */
	NRF_RADIO->PCNF0 |= (1 << RADIO_PCNF0_S0LEN_Pos)
						| (6 << RADIO_PCNF0_LFLEN_Pos)
						| (2 << RADIO_PCNF0_S1LEN_Pos);

	NRF_RADIO->PCNF1 = (3 << RADIO_PCNF1_BALEN_Pos)
		| (RADIO_PCNF1_WHITEEN_Enabled << RADIO_PCNF1_WHITEEN_Pos)
		| (MAX_PDU_SIZE << RADIO_PCNF1_MAXLEN_Pos);


	NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Three << RADIO_CRCCNF_LEN_Pos) |
		(RADIO_CRCCNF_SKIP_ADDR_Skip << RADIO_CRCCNF_SKIP_ADDR_Pos);

	NRF_RADIO->CRCPOLY = CRC_POLY;

	/* Configure two shortcuts:
	 * Start the TASKS_START after an EVENT_READ
	 * Start the TASKS_DISABLE after an EVENT_END
	 */
	NRF_RADIO->SHORTS = (RADIO_SHORTS_READY_START_Pos
					<< RADIO_SHORTS_READY_START_Enabled)
					| (RADIO_SHORTS_END_DISABLE_Pos
					<< RADIO_SHORTS_END_DISABLE_Enabled);

	NRF_RADIO->INTENSET = RADIO_INTENSET_END_Enabled
						<< RADIO_INTENSET_END_Pos;
    
	NVIC_SetPriority(RADIO_IRQn, 1);
	NVIC_ClearPendingIRQ(RADIO_IRQn);
	NVIC_EnableIRQ(RADIO_IRQn);

	NRF_RADIO->PACKETPTR = (uint32_t) pdu;
	memset(pdu, 0, sizeof(pdu));

	status = RADIO_IDLE;
	send_cb = NULL;
	recv_cb = NULL;
	user_data = NULL;
}

int16_t radio_send(uint8_t channel, uint32_t aa, uint32_t crc,
					const uint8_t *data, uint8_t len,
					radio_send_cb func, void *user_data)
{
	return -1;
}

int16_t radio_recv(uint8_t channel, uint32_t aa, uint32_t crc,
					radio_recv_cb func, void *user_data)
{
	uint16_t frequency;

	/* Configure callback */
	recv_cb = func;
	cb_data = user_data;
	status = RADIO_RX;

	/* Set receive address */
	NRF_RADIO->PREFIX0 = aa >> 24;
	NRF_RADIO->BASE0 = (aa & 0xFFFFFF) << 8;
	NRF_RADIO->RXADDRESSES = 0x00000001;

	/* Set frequency */
	frequency = ch2freq(channel);

	if (frequency < 0)
		return -1;

	NRF_RADIO->FREQUENCY = (uint32_t) frequency;

	/* Set CRC and data whitening init values */
	NRF_RADIO->CRCINIT = crc;
	NRF_RADIO->DATAWHITEIV = channel & 0x3F;

	/* Start RX ramp-up */
	NRF_RADIO->TASKS_RXEN = 1UL;

	return 0;
}

int16_t radio_off(void)
{
	NRF_RADIO->EVENTS_DISABLED = 0UL;
	NRF_RADIO->TASKS_DISABLE = 1UL;
	while (NRF_RADIO->EVENTS_DISABLED == 0UL);

	return 0;
}
