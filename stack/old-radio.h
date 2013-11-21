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

#include <stdint.h>

/* Radio features */
#define RADIO_FEAT_AUTO_CRC			0x01
#define RADIO_FEAT_AUTO_WHITENING		0x02
#define RADIO_FEAT_BLE_WHIT_POLY		0x04

#define HAS_FEATURE(mask, feat)			(mask & feat)

/* Radio configuration */
enum {
	RADIO_CONF_FREQUENCY,
	RADIO_CONF_CRC_POLY,
	RADIO_CONF_CRC_INIT,
	RADIO_CONF_WHITENING_POLY,
	RADIO_CONF_WHITENING_INIT,
};

typedef void (*radio_cb) (void *userdata);
typedef void (*radio_recv_cb) (uint8_t *data, uint8_t len, void *userdata);

struct radio_driver {
	/* Initialize the radio. */
	uint16_t (*init) (void);

	/* Get the supported features. */
	uint16_t (*features) (uint32_t *result);

	/* Configure the radio. */
	uint16_t (*configure) (uint16_t type, uint32_t value);

	/* Send a packet. */
	uint16_t (*send) (uint32_t address, uint8_t *data, uint8_t len,
						radio_cb cb, void *userdata);

	/* Receive a packet. */
	uint16_t (*receive) (uint16_t frequency,
			     uint32_t *address,
			     uint8_t *data,
			     uint8_t len,
			     radio_recv_cb cb,
			     void *userdata);
};

extern struct radio_driver radio;
