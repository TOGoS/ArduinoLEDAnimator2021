#include <Print.h>

#include "TOGoS/ArduinoLEDAnimator2021/FastLEDController.h"
#include "TOGoS/stream_operators.h"
#include "config.h"

// Pins:
// - D1 = WS2812 / FastLED
// - D2 = ?
// - D3 = ?
// - D4 = White LED strip (Mosfet - active high

class PinList {};

Print & operator<<(Print &print, PinList pl) {
  print << "# D1 = " << D1 << " (used by 2wire)\n";
  print << "# D2 = " << D2 << "\n";
  print << "# D3 = " << D3 << "\n";
  print << "# D4 = " << D4 << "\n";
  print << "# D5 = " << D5 << "\n";
  print << "# D6 = " << D6 << "\n";
  print << "# D7 = " << D7 << "\n";
  print << "# D8 = " << D8 << "\n";
  print << "# BUILTIN_LED = " << BUILTIN_LED << "\n";
  return print;
}

namespace TOGoS { namespace ArduinoLEDAnimator2021 {
  class TriangleOscillator {
    int current, delta, min, max;
  public:
    TriangleOscillator( int current, int delta, int min, int max )
      : current(current), delta(delta), min(min), max(max) {}
    int next() {
      current += delta;
      if( current > max ) { current = max + max - current; delta = -delta; }
      if( current < min ) { current = min + min - current; delta = -delta; }
      return current;
    }
  };
}}

TOGoS::ArduinoLEDAnimator2021::FastLEDController<WS2812B, FASTLED_PIN, GRB> fastLedController(72);
TOGoS::ArduinoLEDAnimator2021::TriangleOscillator rOsc(128, 13, 32, 255);
TOGoS::ArduinoLEDAnimator2021::TriangleOscillator gOsc(0, 19, 0, 255);
TOGoS::ArduinoLEDAnimator2021::TriangleOscillator bOsc(0, 23, 0, 255);
TOGoS::ArduinoLEDAnimator2021::TriangleOscillator wOsc(0, 10, 0, 255);

const unsigned long colorPushInterval = 50;
unsigned long lastColorPush;

void setup() {
  Serial.begin(115200);
  pinMode(WHITE_PIN, OUTPUT);
  lastColorPush = millis();

  delay(500);

  Serial << "# Welcome to ArduinoLEDAnimator2021" << "\n";
  Serial << "\n";
  Serial << PinList();
  Serial << "\n";
}

void loop() {
  unsigned long currentTime = millis();
  
  delay(10);

  if( lastColorPush == 0 || currentTime - lastColorPush > colorPushInterval ) {
    fastLedController.unshiftColor(CRGB(rOsc.next(), gOsc.next(), bOsc.next()));
    lastColorPush += colorPushInterval;
    FastLED.show();

    digitalWrite(WHITE_PIN, wOsc.next() < 64 ? HIGH : LOW);
  }
  
  Serial << ".";
}
