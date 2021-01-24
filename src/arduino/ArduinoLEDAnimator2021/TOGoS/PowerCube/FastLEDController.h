#ifndef TOGOS_POWERCUBE_FASTLEDCONTROLLER_H
#define TOGOS_POWERCUBE_FASTLEDCONTROLLER_H

#include <FastLED.h>
#include <vector>

namespace TOGoS { namespace PowerCube {
  uint8_t parseHexDigit(char digit);
  uint8_t parseHexPair(const char *digits);
  CRGB parseRgb(const TOGoS::StringView &rgbStr);
  TOGoS::StringView hexEncodeRgb(char *buffer, const CRGB &rgb);
  Print &operator<<(Print &printer, const CRGB& rgb);

  template<template<uint8_t DATA_PIN, EOrder RGB_ORDER> class Controller, uint8_t DATA_PIN, EOrder RGB_ORDER>
  class FastLEDController : public TOGoS::PowerCube::Component {
  protected:
    KernelPtr kernel;
    std::string name;
  public:
    std::vector<CRGB> colorData;
    FastLEDController(KernelPtr kernel, const TOGoS::StringView& name, size_t ledCount)
      : kernel(kernel), name(name), colorData(ledCount)
    {
      this->kernel->getLogStream() << "# Setting up FastLEDController " << this->name << " on data pin " << DATA_PIN << " with " << ledCount << " LEDs\n";
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
    virtual void update() override {
      //this->controller.showLeds();   // Don't do this it makes you have seizures.
      //this->kernel->getLogStream() << "# FastLED.show()\n";
      FastLED.show(); // This actually only needs to be done once for all controllers
    }
    virtual void onMessage(const ComponentMessage& m) override {
      if( m.path.length >= 2 && m.path[m.path.length-2] == "pixelcolors" && m.path[m.path.length-1] == "set" ) {
        CRGB color = parseRgb(m.payload);
        this->setColor(color);
      } else if( m.path.length >= 2 && m.path[m.path.length-2] == "pixelcolors" && m.path[m.path.length-1] == "unshift" ) {
        CRGB color = parseRgb(m.payload);
        // this->kernel->getLogStream() << "# " << this->name << "Unshifting color " << color << " lol\n";
        this->unshiftColor(color);
      } else if( m.path.length >= 1 && m.path[m.path.length-1] == "debugloop" ) {
        this->debugLoop();
      }
    }

    uint8_t debugPhase = 0;
    void debugLoop() {
      this->kernel->getLogStream() << "# " << this->name << " entering debug loop\n";
      while( true ) {
        ++debugPhase;
        if( debugPhase >= 8 ) debugPhase = 0;
        this->unshiftColor( CRGB(debugPhase << 5, debugPhase << 4, debugPhase << 3));
        FastLED.show();
        delay(500);
      }
    }
  };
}}

#endif
