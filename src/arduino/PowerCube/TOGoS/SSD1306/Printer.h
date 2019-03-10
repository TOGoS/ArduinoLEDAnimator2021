#pragma once
#include "Controller.h"

namespace TOGoS { namespace SSD1306 {
  class Printer : public Print {
    TOGoS::SSD1306::Controller &controller;
    const uint8_t* font;
  public:
    Printer(TOGoS::SSD1306::Controller& controller,const uint8_t* font) : controller(controller), font(font) {}
    virtual size_t write(uint8_t ch) override;
    virtual int availableForWrite() const {
      return true;
    }
  };
}}
