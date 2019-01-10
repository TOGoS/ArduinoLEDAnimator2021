
#include "TOGSSD1306.h"

class SSD1306Printer : public Print {
  TOGSSD1306 &ssd1306;
 public:
  SSD1306Printer(TOGSSD1306& ssd1306) : ssd1306(ssd1306) {}
  virtual size_t write(uint8_t c) override {
    this->ssd1306.putChar(c);
  }
  virtual int availableForWrite() const {
    return true;
  }
};

SSD1306Printer oledPrinter(oled);

void setup() {
  Wire.begin();
  oled.init();
  oled.clearDisplay();
  oled.setOctetXY(0,0);
  oledPrinter.print("Initializing!");
  delay(5000);
  oled.clearDisplay();
  oled.setOctetXY(0,0);
  oledPrinter.print("Header 1!");
  oled.setOctetXY(1,0);
  oledPrinter.print("Header 2!");
  oled.setOctetXY(2,0);
  oledPrinter.print("Body 1!");

  //oled.activateScroll();

  /*
  oled.sendCommand(0xA3);
  oled.sendData(2);
  oled.sendCommand(0xB6);
  oled.sendData(6);
  */
}

int hiCount = 0;

void loop() {
  // This causes the lower part of the screen to scroll horizontally
  oled.sendCommand(0x29);
  oled.sendCommand(0x00);
  oled.sendCommand(0x02);
  oled.sendCommand(0x00);
  oled.sendCommand(0x07);
  oled.sendCommand(hiCount);
  oled.sendCommand(0x2F);
  delay(100);
}

void xloop() {
  oled.sendData(0x00);
  oled.sendData(0x00);
  oledPrinter.print("Hi #");
  oledPrinter.print(hiCount++);
  oled.sendData(0x00);
  oled.sendData(0x00);
  oled.sendData(0x01);
  //delay(100);
  oled.sendData(0x03);
  //delay(100);
  oled.sendData(0x07);
  //delay(100);
  oled.sendData(0x0F);
  //delay(100);
  oled.sendData(0x1F);
  //delay(100);
  oled.sendData(0x3F);
  //delay(100);
  oled.sendData(0x7F);
  //delay(100);
  oled.sendData(0xFF);
  //delay(100);
}
