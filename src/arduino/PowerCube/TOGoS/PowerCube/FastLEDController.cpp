#include "FastLEDController.h"

namespace TOGoS { namespace PowerCube {

uint8_t parseHexDigit(char digit) {
  if( digit >= '0' && digit <= '9' ) return digit - '0';
  if( digit >= 'A' && digit <= 'F' ) return 10 + digit - 'A';
  if( digit >= 'a' && digit <= 'f' ) return 10 + digit - 'a';
  return 0;
}

uint8_t parseHexPair(const char *digits) {
  return (parseHexDigit(digits[0]) << 4) | parseHexDigit(digits[1]);
}

CRGB parseRgb(const TOGoS::StringView &rgbStr) {
  if( rgbStr.size() == 7 && rgbStr[0] == '#' ) {
    return CRGB(
      parseHexPair(rgbStr.begin()+1),
      parseHexPair(rgbStr.begin()+3),
      parseHexPair(rgbStr.begin()+5)
    );
  }
  return CRGB(255,0,255);
}

TOGoS::StringView hexEncodeRgb(char *buffer, const CRGB &rgb)
{
  sprintf(buffer, "#%02X%02X%02X", rgb.red, rgb.green, rgb.blue);
  return TOGoS::StringView(buffer, 7);
}

Print &operator<<(Print &printer, const CRGB& rgb) {
  char buffer[8];
  hexEncodeRgb(buffer, rgb);
  return printer << buffer;
}

}}

