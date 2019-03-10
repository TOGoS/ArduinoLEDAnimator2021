#pragma once
// Make font stuff work
#ifdef __AVR__
  #include <avr/pgmspace.h>
  #define OLEDFONT(name) static const uint8_t __attribute__ ((progmem))_n[]
#elif defined(ESP8266)
  #include <pgmspace.h>
  #define OLEDFONT(name) static const uint8_t name[]
#else
  #define pgm_read_byte(addr) (*(const uint8_t *)(addr))
  #define OLEDFONT(name) static const uint8_t name[]
#endif
