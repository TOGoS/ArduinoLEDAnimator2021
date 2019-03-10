#include "Printer.h"

size_t TOGoS::SSD1306::Printer::write(uint8_t ch) {
  int fontWidth = pgm_read_byte(&font[0]);
  int fontOffset = 2; // font 'header' size
  if(ch == '\r') {
    return 1;
  }
  if(ch == '\n') {
    this->controller.clearToEndOfRow();
    return 1;
  }
  if(ch < 32 || ch > 127) {
    ch = ' ';
  }
  for(uint8_t i=0; i<fontWidth; i++) {
    // Font array starts at 0, ASCII starts at 32
    this->controller.sendData(pgm_read_byte(&this->font[(ch-32)*fontWidth+fontOffset+i])); 
  }
  return 1;
}
