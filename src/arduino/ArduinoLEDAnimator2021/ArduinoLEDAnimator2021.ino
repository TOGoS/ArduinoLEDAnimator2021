#include <Print.h>

#include "TOGoS/StringView.h"
#include "TOGoS/stream_operators.h"

// Pins:
// - D1 = WS2812 / FastLED
// - D2
// - D3
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

void setup() {
  Serial.begin(115200);

  delay(500);

  Serial << "# Welcome to ArduinoLEDAnimator2021" << "\n";
  Serial << "\n";
  Serial << PinList();
  Serial << "\n";
}

void loop() {
  delay(500);
  Serial << ".";
}
