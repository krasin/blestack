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
#include <string.h>

#include <nrf51.h>
#include <nrf51_bitfields.h>

// #include <app_util.h>
// #include <nrf_gpio.h>
// #include <boards.h>

#include "nrf51822.h"
#include "timer.h"
#include "log.h"

#define INTERNAL_DBG			1

#define NUM_MAX_TIMERS			4
#define TIMER_EMPTY			NUM_MAX_TIMERS

#define RTC_PRESCALER			0
#define RTC_FREQUENCY			(32768 / (RTC_PRESCALER + 1))

/* Maximum number of ticks before an overflow. */
#define NUM_MAX_TICKS			0xFFFFFFUL

/* Maximum number of milisseconds. */
#define NUM_MAX_MS							\
	((NUM_MAX_TICKS / RTC_FREQUENCY) * 1000UL)

/* Convert milisseconds to RTC ticks. */
#define MS_TO_TICKS(ms)							\
	(((uint64_t) ms * (uint64_t) RTC_FREQUENCY) / 1000ULL)

/* Get the current tick counter. */
#define CURRENT_TICK()			NRF_RTC0->COUNTER

#define LAST_TICK(t)			(timers[t].start + timers[t].interval)

struct timer {
	uint32_t start;
	uint32_t interval;
	timer_cb func;
	void *data;
};

static struct timer timers[NUM_MAX_TIMERS];
static int16_t ntimer;
static uint32_t ticks;
static uint8_t started;

static __inline void start_rtc(void)
{
	if (started) {
#if INTERNAL_DBG
		DBG("already started");
#endif
		return;
	}

#if INTERNAL_DBG
	DBG("");
#endif

	NRF_RTC0->EVTENSET = RTC_EVTENSET_COMPARE0_Msk;
	NRF_RTC0->INTENSET = RTC_INTENSET_COMPARE0_Msk;

	NVIC_ClearPendingIRQ(RTC0_IRQn);
	NVIC_EnableIRQ(RTC0_IRQn);

	NRF_RTC0->TASKS_START = 1UL;

	started = 1;
}

static __inline void stop_rtc(void)
{
#if INTERNAL_DBG
	DBG("");
#endif

	NRF_RTC0->EVTENSET = RTC_EVTENCLR_COMPARE0_Msk;
	NRF_RTC0->INTENSET = RTC_INTENCLR_COMPARE0_Msk;

	NVIC_DisableIRQ(RTC0_IRQn);

	NRF_RTC0->TASKS_STOP = 1UL;

	started = 0;
}

static __inline void timer_remove(int16_t id)
{
#if INTERNAL_DBG
	DBG("id: %u", id);
#endif

	memset(timers + id, 0, sizeof(struct timer));
}

static __inline uint8_t timer_timeout(int16_t id)
{
	uint32_t diff;

	if (ticks < timers[id].start)
		diff = ticks + NUM_MAX_TICKS - timers[id].start;
	else
		diff = ticks - timers[id].start;

	DBG("%u %u %u %u", ticks, timers[id].start, timers[id].interval, diff);

	return diff >= timers[id].interval;
}

static __inline void update_timers(void)
{
	timer_cb func;
	uint32_t cc;
	void *data;
	uint16_t i;

	ntimer = -1;

	for (i = 0; i < NUM_MAX_TIMERS; i++) {
		if (timers[i].func) {
#if INTERNAL_DBG
			DBG("id: %u start: %u interval: %u", i,
					timers[i].start, timers[i].interval);
#endif
			if (timer_timeout(i)) {
#if INTERNAL_DBG
				DBG("timeout id: %u", i);
#endif
				func = timers[i].func;
				data = timers[i].data;
				timer_remove(i);
				func(data);
			} else if (ntimer < 0 || LAST_TICK(i) < LAST_TICK(ntimer))
				ntimer = i;
		}
	}

	if (ntimer == -1)
		stop_rtc();
	else {
		cc = LAST_TICK(ntimer) % NUM_MAX_TICKS;
		NRF_RTC0->CC[0] = cc;

#if INTERNAL_DBG
		DBG("ntimer: %u timeout: %u", ntimer, cc);
#endif
	}
}

#if INTERNAL_DBG
void SWI0_IRQHandler(void)
{
	update_timers();
}
#endif

void RTC0_IRQHandler(void)
{
	ticks = CURRENT_TICK();

	NRF_RTC0->EVENTS_COMPARE[0] = 0;

#if INTERNAL_DBG
	DBG("ticks: %u", ticks);
	NVIC_SetPendingIRQ(SWI0_IRQn);
#else
	update_timers();
#endif
}

int16_t timer_start(uint32_t ms, timer_cb cb, void *user_data)
{
	struct timer *t;
	uint32_t cc;
	int16_t i;

	if (cb == NULL)
		return -1;

	if (ms > NUM_MAX_MS)
		return -1;

	ticks = CURRENT_TICK();

	for (i = 0; i < NUM_MAX_TIMERS; i++) {
		if (timers[i].func)
			continue;

		t = timers + i;
		t->start = ticks;
		t->interval = MS_TO_TICKS(ms);
		t->func = cb;
		t->data = user_data;

		cc = LAST_TICK(i) % NUM_MAX_TICKS;

#if INTERNAL_DBG
		DBG("id: %u start: %lu interval: %lu timeout: %lu", i,
						t->start, t->interval, cc);
#endif
		if (ntimer < 0 || LAST_TICK(i) < LAST_TICK(ntimer)) {
// 			ntimer = i;
// 			NRF_RTC0->CC[0] = cc;

// #if INTERNAL_DBG
// 			DBG("new ntimer: %u", ntimer);
// #endif

			NVIC_SetPendingIRQ(SWI0_IRQn);
			if (!started)
				start_rtc();
		}

		return i;
	}

	return -1;
}

int16_t timer_stop(int16_t id)
{
	if (id >= NUM_MAX_TIMERS || id < 0)
		return -1;

	if (timers[id].func == NULL)
		return -1;

	timer_remove(id);

	if (id == ntimer)
		update_timers();

#if INTERNAL_DBG
	DBG("id %u", id);
#endif

	return 0;
}

int16_t timer_init(void)
{
	if (!NRF_CLOCK->EVENTS_LFCLKSTARTED) {
		NRF_CLOCK->LFCLKSRC = CLOCK_LFCLKSRC_SRC_Xtal
						<< CLOCK_LFCLKSRC_SRC_Pos;

		NRF_CLOCK->TASKS_LFCLKSTART = 1;
		while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);
	}

	NRF_RTC0->PRESCALER = RTC_PRESCALER;
	NVIC_SetPriority(RTC0_IRQn, IRQ_PRIORITY_HIGH);

	memset(timers, 0, sizeof(timers));
	ntimer = -1;

	ticks = 0;
	started = 0;

#if INTERNAL_DBG
	NVIC_ClearPendingIRQ(SWI0_IRQn);
	NVIC_SetPriority(SWI0_IRQn, IRQ_PRIORITY_LOW);
	NVIC_EnableIRQ(SWI0_IRQn);

	DBG("Timer initialized");
	DBG("RTC freq (Hz):	%lu", RTC_FREQUENCY);
	DBG("Max allowed ms:	%lu", NUM_MAX_MS);
	DBG("Ticks/ms:		%lu", MS_TO_TICKS(1));
	DBG("Max allowed ticks:	%lu", MS_TO_TICKS(NUM_MAX_MS));
#endif

	// GPIO_LED_CONFIG(LED0);
	// GPIO_LED_CONFIG(LED1);

	return 0;
}
