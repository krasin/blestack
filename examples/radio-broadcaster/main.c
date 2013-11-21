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
#include "timer.h"

#define CRC_POLY		0x0100065BUL
#define CRC_INIT		0x00555555UL
#define WHIT_POLY		0x00000091UL

#define ADV_ACCESS_ADDR		0x8E89BED6UL
#define ADV_INTERVAL_STEP	625
#define ADV_INTERVAL		1600
#define TIME_BETWEEN_ADVS	10000

#define PDU_MAX_LEN		39

static uint8_t frequencies = { 2402, 2426, 2480 };
static uint8_t channels = { 37, 38, 39 };
static uint8_t curr = 0;
static uint32_t features;

static uint8_t advA[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
static uint8_t pdu[PDU_MAX_LEN];
static uint8_t pdu_len;

static uint16_t timer;

static void advertise(void)
{
	/* Is last frequency? */
	if (curr + 1 == sizeof(frequencies))
		timer_start(&timer, ADV_INTERVAL * ADV_INTERVAL_STEP,
				advertise, 0);
	else
		timer_start(&timer, TIME_BETWEEN_ADVS, advertise, 0);

	radio.configure(RADIO_CONF_FREQUENCY, frequencies[curr]);

	if (HAS_FEATURE(features, RADIO_FEAT_AUTO_WHITENING)) {
		radio.configure(RADIO_CONF_WHITENING_INIT,
				channels[curr] & 0x3F);
	}

	radio_send(ADV_ACCESS_ADDR, advA, sizeof(advA), 0, 0);

	curr = (curr + 1) % sizeof(frequencies);
}

int main(void)
{
	radio_init();

	/* Get supported features */
	radio.features(&features);

	/* If the radio support automatic CRC calculation, then sets the
	 * polynomial and initial value.
	 */
	if (HAS_FEATURE(features, RADIO_FEAT_AUTO_CRC)) {
		radio.configure(RADIO_CONF_CRC_POLY, CRC_POLY);
		radio.configure(RADIO_CONF_CRC_INIT, CRC_INIT);
	}

	/* If the radio support automatic data whitening but don't
	 * automatically set the whitening polynomial, sets it.
	 */
	if (HAS_FEATURE(features, RADIO_FEAT_AUTO_WHITENING &&
			!HAS_FEATURE(features, RADIO_FEAT_BLE_WHIT_POLY)) {
		radio.configure(RADIO_CONF_WHITENING_POLY, WHIT_POLY);
	}

	/* Send first packet */
	advertise();

	while (1);

	return 0;
}