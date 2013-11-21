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

#include "timer.h"

#define NUM_MAX_TIMERS			8

#define RTC_PERIOD_MS			1
#define RTC_PRESCALER			(((32768 * RTC_PERIOD_MS) / 1000) - 1)
#define RTC_FREQUENCY			((32768 / RTC_PRESCALER) - 1)
#define RTC_OVRFLW			0x1FFF

#define CURRENT_TICKS()			(NRF_RTC0->COUNTER)
#define MS_TO_TICKS(ms)			(ms / RTC_FREQUENCY)

struct timer {
	uint32_t ovrflw;
	uint32_t ticks;
	timer_cb func;
	void *data;
};

static struct timer timers[NUM_MAX_TIMERS];
static uint8_t next_timer;


static uint8_t find_next_timer(void)
{

}

static __inline void remove_timer(uint8_t id)
{

}

static void RTC0_IRQHandler(void)
{
	

	/* TODO: handle EVENT_OVRFLW, updating all active timers' delta */
}

int16_t timer_init(void)
{
	/* Initialize low frequency clock */
	if (!NRF_CLOCK->EVENTS_LFCLKSTARTED) {
		NRF_CLOCK->LFCLKSRC = CLOCK_LFCLKSRC_SRC_Xtal
						<< CLOCK_LFCLKSRC_SRC_Pos;

		NRF_CLOCK->TASKS_LFCLKSTART = 1;
		while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);
	}

	/* Configure and start RTC0 */
	NRF_RTC0->PRESCALER = RTC_PRESCALER;
	NRF_RTC0->TASKS_START = 1UL;

	memset(timers, 0, sizeof(timers));
	next_timer = NUM_MAX_TIMERS;

	return 0;
}

int16_t timer_start(uint8_t *id, uint32_t ms, timer_cb cb, void *user_data)
{
	uint32_t ticks;
	struct timer *t;
	uint8_t i;

	if (cb == NULL)
		return -1;

	for (i = 0; i < NUM_MAX_TIMERS; i++) {
		t = &timers[i];

		if (t->func == NULL) {
			ticks = MS_TO_TICKS(ms) + CURRENT_TICKS();

			t->ovrflw = ticks / RTC_OVRFLW;
			t->ticks = ticks % RTC_OVRFLW;
			t->func = cb;
			t->data = user_data;

			*id = i;

			if (next_timer == NUM_MAX_TIMERS || )
				next_timer = i;

			return 0;
		}
	}

	return -1;
}

int16_t timer_stop(uint8_t id)
{
	struct timer *t;

	if (id > NUM_MAX_TIMERS)
		return -1;

	t = &timers[i];

	if (t->func == NULL)
		return -1;

	memset(t, 0, sizeof(struct timer));

	return 0;
}
