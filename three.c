/**
 * THE OUTPUT PIN IN THIS CODE IS PB1
 *
 * Settings:
 *  FUSE_L=0x6A
 *  FUSE_H=0xFF
 *  F_CPU=1200000
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define SPEAKER_PIN PB1
#define BUTTON_PIN PB3

#define _NOP()                   \
  do {                           \
    __asm__ __volatile__("nop"); \
  } while (0)

#define my_delay(x)                  \
  for (uint16_t i = x; i > 0; i--) { \
    _NOP();                          \
    _NOP();                          \
    _NOP();                          \
    _NOP();                          \
    _NOP();                          \
    _NOP();                          \
    _NOP();                          \
    _NOP();                          \
    _NOP();                          \
    _NOP();                          \
  }

void tone(uint16_t spd, uint16_t time)
{
  uint16_t c;
  if (time < spd)
    time = spd;
  c = time / spd * 10;
  for (uint16_t i = c; i > 0; i--) {
    PORTB |= _BV(SPEAKER_PIN);
    my_delay(spd * 1);
    PORTB &= ~(_BV(SPEAKER_PIN));
    my_delay(spd * 4);
  }
}

int main(void)
{
  /* setup */
  DDRB |= _BV(SPEAKER_PIN); // set speaker pin as OUTPUT
  DDRB &= ~_BV(BUTTON_PIN); // set button pin as INPUT
  // PORTB &= ~_BV(BUTTON_PIN); // set button pin as INPUT
  PORTB |= _BV(BUTTON_PIN); // pull line high to prevent floating

  while (1) {
    if (PINB & (_BV(BUTTON_PIN))) {
      for (uint8_t i = 10; i < 20; i++) {
        tone(i, 80);
      }
      tone(20, 200);
      // _delay_ms(500);
      break;
    } else {
      for (uint8_t i = 20; i > 10; i--) {
        tone(i, 80);
      }
      // tone(20, 250);
      // tone(15, 250);
      // tone(12, 250);
      tone(10, 200);
      // tone(8, 30); tone(12, 30);
      // tone(8, 30); tone(12, 30);
      // _delay_ms(500);
      break;
    }
  }
}
