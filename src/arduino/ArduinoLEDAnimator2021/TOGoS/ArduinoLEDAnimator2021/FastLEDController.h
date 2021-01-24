#ifndef TOGOS_POWERCUBE_FASTLEDCONTROLLER_H
#define TOGOS_POWERCUBE_FASTLEDCONTROLLER_H

#include <FastLED.h>
#include <vector>

#include "../StringView.h"

namespace TOGoS { namespace ArduinoLEDAnimator2021 {
  uint8_t parseHexDigit(char digit);
  uint8_t parseHexPair(const char *digits);
  CRGB parseRgb(const TOGoS::StringView &rgbStr);
  TOGoS::StringView hexEncodeRgb(char *buffer, const CRGB &rgb);
  Print &operator<<(Print &printer, const CRGB& rgb);

  template<template<uint8_t DATA_PIN, EOrder RGB_ORDER> class Controller, uint8_t DATA_PIN, EOrder RGB_ORDER>
  class FastLEDController {
  protected:
    std::string name;
  public:
    std::vector<CRGB> colorData;
    FastLEDController(size_t ledCount)
      : colorData(ledCount)
    {
      FastLED.addLeds<Controller, DATA_PIN, RGB_ORDER>(this->colorData.data(), this->colorData.size());
    }
    void unshiftColor( CRGB color ) {
      for( size_t i=colorData.size(); i > 0; --i ) {
        colorData[i] = colorData[i-1];
      }
      colorData[0] = color;
    }
    void setColor( CRGB color ) {
      for( size_t i=colorData.size(); i-- > 0; ) {
        colorData[i] = color;
      }
    }
  };
}}

#endif
