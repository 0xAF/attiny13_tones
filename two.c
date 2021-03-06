/**
 * Copyright (c) 2016, Łukasz Marcin Podkalicki <lpodkalicki@gmail.com>
 * ATtiny13/015
 * Two-Tone Alarm.
 * --
 * Settings:
 *  FUSE_L=0x6A
 *  FUSE_H=0xFF
 *  F_CPU=1200000
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define SPEAKER_PIN PB0
// #define PWM_TO_OCR0A

#define N_1 (_BV(CS00))
#define N_8 (_BV(CS01))
#define N_64 (_BV(CS01) | _BV(CS00))
#define N_256 (_BV(CS02))
#define N_1024 (_BV(CS02) | _BV(CS00))

static void twotone_alarm(uint8_t type);
static void tone_loop(uint8_t OCRxn, uint8_t N, uint8_t max, uint8_t delay, uint8_t pause, uint8_t fade);
static void timer_set(uint8_t OCRxn, uint8_t N);
static void sleep(uint8_t ms);

void timer_set(uint8_t OCRxn, uint8_t N)
{
  //        first disable all CS[0:2] bits, then set only the needed (N)
  TCCR0B = (TCCR0B & ~(_BV(CS02) | _BV(CS01) | _BV(CS00))) | N;
  // TCCR0B = (TCCR0B & ~(_BV(CS02) | _BV(CS01) | _BV(CS00)));
  OCR0A = OCRxn - 1;
}

// ISR(TIM0_OVF_vect)
// {
//   TCNT0 = 10;
//   PORTB ^= _BV(SPEAKER_PIN); // set speaker pin as OUTPUT
// }

int main(void)
{

  /* setup */
  DDRB |= _BV(SPEAKER_PIN); // set speaker pin as OUTPUT
// #define PWM_TO_OCR0A
#ifdef PWM_TO_OCR0A
  TCCR0A |= _BV(WGM00) | _BV(WGM01); // set timer mode to Fast PWM
  // TCCR0B |= _BV(WGM02); // set timer mode to Fast PWM
#else
  TCCR0A |= _BV(WGM01); // set timer mode to Fast PWM
  // TCCR0A |= _BV(WGM00); // set timer mode to Fast PWM
  // TCCR0B |= _BV(WGM02); // set timer mode to Fast PWM
#endif

  // TCCR0A |= _BV(COM0A0) | _BV(COM0A1); // connect PWM pin to Channel A of Timer0
#ifdef PWM_TO_OCR0A
  TCCR0A |= _BV(COM0A1); // connect PWM pin to Channel A of Timer0
#else
  TCCR0A |= _BV(COM0A0); // connect PWM pin to Channel A of Timer0
#endif

  // TIMSK0 |= _BV(TOIE0); // enable Timer Overflow interrupt
  // sei(); // enable global interrupts
  // TCCR0B = (TCCR0B & ~(_BV(CS02) | _BV(CS01) | _BV(CS00))) | N_1024;

  /* loop */
  uint8_t tone = 0;
  while (1) {
    uint8_t i = 10 - tone;
    while (i--) {
      twotone_alarm(tone);
      // twotone_alarm(0);
      // timer_set(64, N_8);
      // tone_loop(64, N_1, 10, 10, 0, 0);
      // tone_loop(64, N_8, 10, 10, 0, 0);
    }
    tone = ++tone % 7;
  }
}

void twotone_alarm(uint8_t type)
{

  switch (type) {
  /* Please, put here your own two-tone alarm composition! */
  case 6:
    tone_loop(64, N_8, 128, 10, 10, 1);
    tone_loop(64, N_8, 6, 10, 0, -1);
    break;
  case 5:
    tone_loop(180, N_8, 128, 10, 10, -1);
    tone_loop(52, N_8, 6, 10, 0, -1);
    break;
  case 4:
    tone_loop(180, N_8, 32, 10, 10, -5);
    tone_loop(20, N_8, 6, 10, 0, -1);
    break;
  case 3:
    tone_loop(180, N_8, 16, 10, 10, -10);
    tone_loop(22, N_8, 6, 10, 0, -1);
    break;
  case 2:
    tone_loop(22, N_8, 16, 10, 10, 10);
    tone_loop(22, N_8, 6, 10, 0, -1);
    break;
  case 1:
    tone_loop(123, N_8, 6, 10, 10, 1);
    tone_loop(22, N_8, 6, 10, 0, -1);
    break;
  default:
  case 0:
    tone_loop(32, N_8, 6, 10, 10, 1);
    tone_loop(22, N_8, 6, 10, 0, -1);
    break;
  }
}

/**
 * Single tone loop with fade-in/out effect.
 *
 * Base square wave frequency,
 * F = F_CPU / (2 * N * (1 + OCRnx)), where:
 * - F is a calculated PWM frequency
 * - F_CPU is a clock source (1.2MHz)
 * - the N variable represents the prescaler factor (1, 8, 64, 256, or 1024)
 *
 * @param OCRxn: timer OCRxn value
 * @param N: timer prescaler (N_1, N_8, N_64, N_256, N_1024)
 * @param max: number of iterations (incr/decr of OCRxn)
 * @param delay: little delay after each iteration in miliseconds
 * @param pause: delay after a tone loop, delay between tones
 * @param fade: fade-in (1) or fade-out (-1) factor
 */
void tone_loop(uint8_t OCRxn, uint8_t N, uint8_t max, uint8_t delay, uint8_t pause, uint8_t fade)
{
  uint8_t i;

  for (i = 0; i < max; ++i) {
    timer_set(OCRxn, N);
    OCRxn += fade;
    sleep(delay);
    TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00)); // stop the timer
  }

  // DDRB &= ~(_BV(SPEAKER_PIN)); // set speaker pin as INPUT
  sleep(pause);
  // DDRB |= _BV(SPEAKER_PIN); // set speaker pin as OUTPUT
}

void sleep(uint8_t ms)
{
  uint8_t i;

  for (i = 0; i < ms; ++i) {
    _delay_ms(1);
  }
}
