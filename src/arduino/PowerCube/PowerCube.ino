
// #include "TOGSSD1306.h"

#include <Wire.h>

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
#include "TOGSSD1306/font8x8.h"

// Data sheet for the SSD1306: https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf

class TOGoS_SSD1306Controller {
  TwoWire &twi;
  uint8_t address;
 public:
  TOGoS_SSD1306Controller(TwoWire &twi, uint8_t address) : twi(twi), address(address) { }
  void initialize();
  void sendCommand(uint8_t command);
  void sendData(uint8_t data);

  void displayOn();
  void clear(uint8_t data=0x00);
  void clearToEndOfRow();
  void setBrightness(uint8_t brightness);
  void gotoRowCol(uint8_t row, uint8_t col);

  uint8_t atRow = 0, atColumn = 0;

  static const uint8_t rowCount = 8;
  static const uint8_t columnCount = 128;
  
  static const uint8_t SSD1306_Command_Mode         = 0x80;
  static const uint8_t SSD1306_Data_Mode            = 0x40;
  static const uint8_t SSD1306_Display_Off_Cmd      = 0xAE;
  static const uint8_t SSD1306_Display_On_Cmd       = 0xAF;
  static const uint8_t SSD1306_Normal_Display_Cmd   = 0xA6;
  static const uint8_t SSD1306_Inverse_Display_Cmd  = 0xA7;
  static const uint8_t SSD1306_Activate_Scroll_Cmd  = 0x2F;
  static const uint8_t SSD1306_Dectivate_Scroll_Cmd = 0x2E;
  static const uint8_t SSD1306_Set_Brightness_Cmd   = 0x81;
};

void TOGoS_SSD1306Controller::initialize() {
  sendCommand(0xAE);            //display off
  sendCommand(0xA6);            //Set Normal Display (default)
  sendCommand(0xAE);            //DISPLAYOFF
  sendCommand(0xD5);            //SETDISPLAYCLOCKDIV
  sendCommand(0x80);            // the suggested ratio 0x80
  sendCommand(0xA8);            //SSD1306_SETMULTIPLEX
  sendCommand(0x3F);
  sendCommand(0xD3);            //SETDISPLAYOFFSET
  sendCommand(0x0);             //no offset
  sendCommand(0x40|0x0);        //SETSTARTLINE
  sendCommand(0x8D);            //CHARGEPUMP
  sendCommand(0x14);
  sendCommand(0x20);            //MEMORYMODE
  sendCommand(0x00);            //0x0 act like ks0108
  sendCommand(0xA1);            //SEGREMAP   Mirror screen horizontally (A0)
  sendCommand(0xC8);            //COMSCANDEC Rotate screen vertically (C0)
  sendCommand(0xDA);            //0xDA
  sendCommand(0x12);            //COMSCANDEC
  sendCommand(0x81);            //SETCONTRAST
  sendCommand(0xCF);            //
  sendCommand(0xd9);            //SETPRECHARGE 
  sendCommand(0xF1); 
  sendCommand(0xDB);            //SETVCOMDETECT                
  sendCommand(0x40);
  sendCommand(0xA4);            //DISPLAYALLON_RESUME        
  sendCommand(0xA6);            //NORMALDISPLAY             
  sendCommand(0x2E);            //Stop scroll
  sendCommand(0x20);            //Set Memory Addressing Mode
  sendCommand(0x00);            //Set Memory Addressing Mode ab Horizontal addressing mode
}

void TOGoS_SSD1306Controller::sendCommand(uint8_t command) {
  this->twi.beginTransmission(this->address);    // begin I2C communication
  this->twi.write(SSD1306_Command_Mode);   // Set OLED Command mode
  this->twi.write(command);
  this->twi.endTransmission();             // End I2C communication
}

void TOGoS_SSD1306Controller::sendData(uint8_t data) {
  this->twi.beginTransmission(this->address); // begin I2C transmission
  this->twi.write(SSD1306_Data_Mode);            // data mode
  this->twi.write(data);
  this->twi.endTransmission();                    // stop I2C transmission

  // Increment where we think we are, assuming the usual addressing mode:
  ++this->atColumn;
  if( this->atColumn == this->columnCount ) {
    this->atColumn = 0;
    ++this->atRow;
    if( this->atRow == this->rowCount ) this->atRow = 0;
  }
}

void TOGoS_SSD1306Controller::displayOn() {
  this->sendCommand(SSD1306_Display_On_Cmd);     //display on
}

void TOGoS_SSD1306Controller::setBrightness(uint8_t brightness) {
   sendCommand(SSD1306_Set_Brightness_Cmd);
   sendCommand(brightness);
}

void TOGoS_SSD1306Controller::gotoRowCol(uint8_t row, uint8_t col) {
  this->sendCommand(0xB0 + row);                          //set page address
  this->sendCommand(0x00 + (col & 0x0F));    //set column lower addr
  this->sendCommand(0x10 + ((col>>4)&0x0F)); //set column higher addr
  this->atColumn = col;
  this->atRow = row;
}

void TOGoS_SSD1306Controller::clear(uint8_t data) {
  this->gotoRowCol(0,0);
  do {
    this->sendData(data);
  } while( this->atColumn != 0 || this->atRow != 0 );
}

void TOGoS_SSD1306Controller::clearToEndOfRow() {
  do {
    this->sendData(0x00);
  } while( this->atColumn != 0 );
}


class TOGoS_SSD1306Printer : public Print {
  TOGoS_SSD1306Controller &controller;
  const uint8_t* font;
public:
  TOGoS_SSD1306Printer(TOGoS_SSD1306Controller& controller,const uint8_t* font) : controller(controller), font(font) {}
  virtual size_t write(uint8_t ch) override {
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
  virtual int availableForWrite() const {
    return true;
  }
};

/*
class DisplayMode {
  virtual void setup() = 0;
  virtual void update() = 0;
}
*/

TOGoS_SSD1306Controller oledController(Wire, 0x3C);
TOGoS_SSD1306Printer oledPrinter(oledController, font8x8);

void setup() {
  Wire.begin();
  oledController.initialize();
  oledController.displayOn();
  oledController.setBrightness(255);
  oledController.clear(0xAA);
  oledController.gotoRowCol(0, 0);
  oledPrinter.println("Initializing!");
  for( int i=5; i>0; --i ) {
    oledController.gotoRowCol(1, 0);
    oledPrinter.print("Starting in ");
    oledPrinter.println(i);
    delay(1000);
  }
}

int brightnessDirection = 1;
uint8_t brightness = 128;

void loop() {
  if( brightness == 0 && brightnessDirection < 0 ) {
    brightnessDirection = 1;
  }
  if( brightness == uint8_t(0xFF) && brightnessDirection > 0 ) {
    brightnessDirection = -1;
  }
  brightness += brightnessDirection;
  oledController.setBrightness(brightness);
  oledController.gotoRowCol(7, 0);
  oledPrinter.print("Brightness: ");
  oledPrinter.println(brightness);
  delay(100);
}

int hiCount = 0;

void horse_around_loop_1() {
  oledController.sendCommand(0xA3);
  oledController.sendCommand(0x02); // Number of rows in fixed area
  oledController.sendCommand(0x06); // Number of rows in scroll area

  // oled.sendCommand(0xC8); // Flip horizontally?  No that didn't do anything.
  // oled.sendCommand(0xA1); // Flip horizontally? // Nope.

  //oled.sendCommand(0x40 | (hiCount&0x3F)); // This only scrolls the first line?!??
  //oled.sendCommand(0xDA);


  ++hiCount;
  delay(200);
}

void do_scroll() {
  // Set vertical scroll area
  oledController.sendCommand(0xA3);
  oledController.sendCommand(0x02); // Number of rows in fixed area
  oledController.sendCommand(0x06); // Number of rows in scroll area

  // This causes the lower part of the screen to scroll horizontally
  oledController.sendCommand(0x29);
  oledController.sendCommand(0x00); // Dummy byte
  oledController.sendCommand(0x02); // Start page address
  oledController.sendCommand(0x00); // Interval between scroll steps
  oledController.sendCommand(0x07); // End page address
  oledController.sendCommand(0x01);
  // Activate scroll
  oledController.sendCommand(0x2F);
  delay(100);
  //oled.sendCommand(0x2E); // Stop scroll
}
