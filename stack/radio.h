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

typedef void (*radio_send_cb) (void *user_data);
typedef void (*radio_recv_cb) (const uint8_t *data, uint8_t len,
							void *user_data);

int16_t radio_init(void);

int16_t radio_send(uint8_t channel, uint32_t aa, uint32_t crc,
					const uint8_t *data, uint8_t len,
					radio_send_cb func, void *user_data);

int16_t radio_recv(uint8_t channel, uint32_t aa, uint32_t crc,
					radio_recv_cb func, void *user_data);

int16_t radio_off(void);
