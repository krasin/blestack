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

#include "log.h"
#include "timer.h"

#define ONE_SECOND			1000
#define HALF_SECOND			500

static uint16_t timer1;
static uint16_t timer2;

static void timeout(void *user_data)
{
	uint16_t *timer = user_data;

	DBG("Timer %u fired!", *timer);

	if (timer == &timer1)
		timer1 = timer_start(ONE_SECOND, timeout, timer);
	else
		timer2 = timer_start(HALF_SECOND, timeout, timer);
}

int main(void)
{
	log_init();
	timer_init();

	timer1 = timer_start(ONE_SECOND, timeout, &timer1);
	timer2 = timer_start(HALF_SECOND, timeout, &timer2);

	while (1);

	return 0;
}

// #include <nrf51.h>
// #include <nrf51_bitfields.h>

// #define TICK		1
// #define OVRFLW		2
// static uint8_t flag;

// void RTC0_IRQHandler(void)
// {
// 	if (NRF_RTC0->EVENTS_TICK) {
// 		NRF_RTC0->EVENTS_TICK = 0;
// 		flag |= TICK;
// 	}

// 	if (NRF_RTC0->EVENTS_OVRFLW) {
// 		NRF_RTC0->EVENTS_OVRFLW = 0;
// 		flag |= OVRFLW;
// 	}
// }

// int main(void)
// {
// 	flag = 0;

// 	log_init();
// 	DBG("");

// 	if (!NRF_CLOCK->EVENTS_HFCLKSTARTED) {
// 		NRF_CLOCK->TASKS_HFCLKSTART = 1UL;
// 		while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0UL);
// 	}

// 	if (!NRF_CLOCK->EVENTS_LFCLKSTARTED) {
// 		NRF_CLOCK->LFCLKSRC = CLOCK_LFCLKSRC_SRC_Xtal
// 						<< CLOCK_LFCLKSRC_SRC_Pos;

// 		NRF_CLOCK->TASKS_LFCLKSTART = 1;
// 		while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);
// 	}

// 	NVIC_SetPriority(RTC0_IRQn, 0);
// 	NVIC_ClearPendingIRQ(RTC0_IRQn);
// 	NVIC_EnableIRQ(RTC0_IRQn);

// 	NRF_RTC0->PRESCALER = 0xFFF;
// 	NRF_RTC0->EVTENSET = 0;
// 	NRF_RTC0->EVTENSET |= RTC_EVTENSET_TICK_Msk;
// 	NRF_RTC0->EVTENSET |= RTC_EVTENSET_OVRFLW_Msk;
// 	NRF_RTC0->INTENSET = RTC_INTENSET_TICK_Msk;

// 	NRF_RTC0->TASKS_TRIGOVRFLW = 1UL;
// 	NRF_RTC0->TASKS_START = 1UL;

// 	while (1) {
// 		if (flag) {
// 			log_print("%c %c\r\n",
// 				(flag & TICK) ? 'T' : ' ',
// 				(flag & OVRFLW) ? 'O' : ' ');
// 		}

// 		flag = 0;
// 	}

// 	return 0;
// }
