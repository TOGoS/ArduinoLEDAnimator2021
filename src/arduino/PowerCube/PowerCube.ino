#include <Wire.h>
#include <Print.h>
#include <map>

#include "TOGoS/PowerCube/CommandRunner.h"
#include "TOGoS/PowerCube/DHTReader.h"
#include "TOGoS/PowerCube/DigitalSwitch.h"
#include "TOGoS/PowerCube/Echoer.h"
#include "TOGoS/PowerCube/Kernel.h"
#include "TOGoS/SSD1306/font8x8.h"
#include "TOGoS/SSD1306/Controller.h"
#include "TOGoS/SSD1306/Printer.h"
#include "TOGoS/StringView.h"
#include "TOGoS/stream_operators.h"

#include <FastLED.h>
#include <vector>

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

namespace TOGoS { namespace PowerCube {
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
    virtual void update() override {
      //this->controller.showLeds();   // Don't do this it makes you have seizures.
      //this->kernel->getLogStream() << "# FastLED.show()\n";
      FastLED.show(); // This actually only needs to be done once for all controllers
    }
    virtual void onMessage(const ComponentMessage& m) override {
      if( m.path.length >= 2 && m.path[m.path.length-2] == "pixelcolors" && m.path[m.path.length-1] == "unshift" ) {
        CRGB color = parseRgb(m.payload);
        // this->kernel->getLogStream() << "# " << this->name << "Unshifting color " << color << " lol\n";
        this->unshiftColor(color);
      }
      if( m.path.length >= 1 && m.path[m.path.length-1] == "debugloop" ) {
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




// TODO (story): read temperature and humidity, show on OLED screen
// TODO (story): turn on and off a relay based on messages from serial port
// TODO (story): thermostat - turn a switch on and off based on temperature
// TODO (feature): Battery charging
// TODO (technical): message publishing should take scope argument: internal|serial|network
// TODO (technical): save settings

using StringView = TOGoS::StringView;

TOGoS::SSD1306::Controller oledController(Wire, 0x3C);
TOGoS::SSD1306::Printer oledPrinter(oledController, font8x8);

TOGoS::PowerCube::Kernel kernel;

void dumpPinList() {
  Serial << "# D1 = " << D1 << " (used by 2wire)\n";
  Serial << "# D2 = " << D2 << "\n";
  Serial << "# D3 = " << D3 << "\n";
  Serial << "# D4 = " << D4 << "\n";
  Serial << "# D5 = " << D5 << "\n";
  Serial << "# D6 = " << D6 << "\n";
  Serial << "# D7 = " << D7 << "\n";
  Serial << "# D8 = " << D8 << "\n";
  Serial << "# BUILTIN_LED = " << BUILTIN_LED << "\n";
}

void setup() {
  Serial.begin(115200);

  delay(500);

  // Wire uses D1, which I want to use for controlling a WS2812B strip.
  // Therefore commenting out the OLED screen stuff.
  /*
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
  */

  dumpPinList();

  kernel.components["serial-command-runner"].reset(new TOGoS::PowerCube::CommandRunner(&kernel, Serial));
  kernel.components["echoer"].reset(new TOGoS::PowerCube::Echoer(Serial));
  kernel.components["dht6"].reset(new TOGoS::PowerCube::DHTReader(&kernel, "dht6", D6, DHT22));
  kernel.components["dht7"].reset(new TOGoS::PowerCube::DHTReader(&kernel, "dht7", D7, DHT22));
  kernel.components["builtinled"].reset(new TOGoS::PowerCube::DigitalSwitch(&kernel, "builtinled", BUILTIN_LED, true));
  kernel.components["d5"].reset(new TOGoS::PowerCube::DigitalSwitch(&kernel, "d5", D5, false));
  kernel.components["fastleds"].reset(new TOGoS::PowerCube::FastLEDController<WS2812B, D1, GRB>(&kernel, "fastleds", 11));
  kernel.components["whiteledstrip"].reset(new TOGoS::PowerCube::DigitalSwitch(&kernel, "whiteledstrip", D4, false));
}

int brightnessDirection = 1;
uint8_t brightness = 128;

unsigned long lastColorPush = 0;

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

TriangleOscillator rOsc(128, 13, 32, 255);
TriangleOscillator gOsc(0, 19, 0, 255);
TriangleOscillator bOsc(0, 23, 0, 255);

void loop() {
  kernel.update();

  /*
  if( kernel.getCurrentTickNumber() % 100 == 0 ) {
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
  }
  */

  unsigned long currentTime = millis();
  if( lastColorPush == 0 || currentTime - lastColorPush > 50 ) {
    using ComponentMessage = TOGoS::PowerCube::ComponentMessage;
    using Path = TOGoS::PowerCube::Path;
    char formattedColor[8];
    TOGoS::StringView rgbStr = hexEncodeRgb(formattedColor, CRGB(rOsc.next(), gOsc.next(), bOsc.next()));
    kernel.deliverMessage(ComponentMessage(
      Path() << "fastleds" << "pixelcolors" << "unshift",
      rgbStr,
      TOGoS::PowerCube::PubBits::Internal
    ));
    lastColorPush = currentTime;
  }
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
