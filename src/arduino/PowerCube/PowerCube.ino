#include <Wire.h>
#include <Print.h>
#include <map>

#include "TOGoS/PowerCube/CommandRunner.h"
#include "TOGoS/PowerCube/DHTReader.h"
#include "TOGoS/PowerCube/DigitalSwitch.h"
#include "TOGoS/PowerCube/FastLEDController.h"
#include "TOGoS/PowerCube/Echoer.h"
#include "TOGoS/PowerCube/Kernel.h"
#include "TOGoS/PowerCube/PWMSwitch.h"
#include "TOGoS/SSD1306/font8x8.h"
#include "TOGoS/SSD1306/Controller.h"
#include "TOGoS/SSD1306/Printer.h"
#include "TOGoS/StringView.h"
#include "TOGoS/stream_operators.h"





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

namespace TOGoS { namespace PowerCube {
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
    
  /** Reads commands from a stream (e.g. Serial) nd forwards them */
  class FastLEDAnimator : public Component {
  private:
    KernelPtr kernel;
    Path unshiftPath;
    Path fillPath;
    unsigned long lastColorPush = 0;
    
    TriangleOscillator rOsc = TriangleOscillator(128, 13, 32, 255);
    TriangleOscillator gOsc = TriangleOscillator(0, 19, 0, 255);
    TriangleOscillator bOsc = TriangleOscillator(0, 23, 0, 255);
    CRGB solidColor;
    enum {
      OSC, SOLID
    } colorMode;
  public:
    FastLEDAnimator(KernelPtr kernel, const TOGoS::StringView& name) : kernel(kernel) {
      this->unshiftPath << "fastleds" << "pixelcolors" << "unshift";
      this->fillPath << "fastleds" << "pixelcolors" << "unshift";
    }

    CRGB nextColor() {
      if( this->colorMode == OSC ) {
        return CRGB(rOsc.next(), gOsc.next(), bOsc.next());
      } else {
        return this->solidColor;
      }
    }

    virtual void update() override {
      unsigned long currentTime = millis();
      if( lastColorPush == 0 || currentTime - lastColorPush > 50 ) {
        using ComponentMessage = TOGoS::PowerCube::ComponentMessage;
        using Path = TOGoS::PowerCube::Path;
        char formattedColor[8];
        TOGoS::StringView rgbStr = TOGoS::PowerCube::hexEncodeRgb(formattedColor, this->nextColor());
        this->kernel->deliverMessage(ComponentMessage(
          this->unshiftPath,
          rgbStr,
          TOGoS::PowerCube::PubBits::Internal
        ));
        lastColorPush = currentTime;
      }
    }

    void setColor(CRGB color) {
      this->colorMode = SOLID;
      this->solidColor = color;
    }

    virtual void onMessage(const ComponentMessage& m) override {
      if( m.path.length >= 2 && m.path[m.path.length-2] == "color" && m.path[m.path.length-1] == "set" ) {
        if( m.payload == "on" ) {
          this->setColor(CRGB(255,255,255));
        } else if( m.payload == "off" ) {
          this->setColor(CRGB(0,0,0));
        } else if( m.payload == "animated" ) {
          this->colorMode = OSC;
        } else if( m.payload[0] == '#' ) {
          CRGB color = parseRgb(m.payload);
          this->setColor(color);
        } else {        
          this->kernel->getLogStream() << "# Unrecognized color: " << m.payload << "\n";
        }
      } else {
        this->kernel->getLogStream() << "# Unrecognized message: " << m.path << "\n";
        this->kernel->getLogStream() << "# Try .../color/set {on|off|animated|#RRGGBB}\n";
      }
    }
  };
}}

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

int brightnessDirection = 1;
uint8_t brightness = 128;

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
  // Disable these if you don't need them because the reads seem to mess with PWM timing
  // enough that your lights will flicker.
  kernel.components["dht6"].reset(new TOGoS::PowerCube::DHTReader(&kernel, "dht6", D6, DHT22));
  kernel.components["dht7"].reset(new TOGoS::PowerCube::DHTReader(&kernel, "dht7", D7, DHT22));
  kernel.components["builtinled"].reset(new TOGoS::PowerCube::PWMSwitch(&kernel, "builtinled", BUILTIN_LED, true));
  kernel.components["d5"].reset(new TOGoS::PowerCube::DigitalSwitch(&kernel, "d5", D5, false));
  kernel.components["fastleds"].reset(new TOGoS::PowerCube::FastLEDController<WS2812B, D1, GRB>(&kernel, "fastleds", 11));
  kernel.components["whiteledstrip"].reset(new TOGoS::PowerCube::PWMSwitch(&kernel, "whiteledstrip", D4, false));
  kernel.components["fastledanimator"].reset(new TOGoS::PowerCube::FastLEDAnimator(&kernel, "fastledanimator"));
}

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


  delay(1);
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
