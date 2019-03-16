#include <Wire.h>
#include <Print.h>
#include <map>

#include "TOGoS/PowerCube/CommandRunner.h"
#include "TOGoS/PowerCube/Kernel.h"
#include "TOGoS/SSD1306/font8x8.h"
#include "TOGoS/SSD1306/Controller.h"
#include "TOGoS/SSD1306/Printer.h"
#include "TOGoS/StringView.h"
#include "TOGoS/stream_operators.h"

// TODO (technical): parse messages
// TODO (story): read temperature and humidity, show on OLED screen
// TODO (story): turn on and off a relay based on messages from serial port
// TODO (story): thermostat - turn a switch on and off based on temperature
// TODO (feature): Battery charging
// TODO (technical): message publishing should take scope argument: internal|serial|network
// TODO (technical): save settings

using StringView = TOGoS::StringView;

TOGoS::SSD1306::Controller oledController(Wire, 0x3C);
TOGoS::SSD1306::Printer oledPrinter(oledController, font8x8);

TOGoS::PowerCube::EchoerClass echoerClass;

TOGoS::PowerCube::Kernel kernel;

void setup() {
  Serial.begin(115200);
  kernel.initialize();
  kernel.components["serial-command-runner"] = new TOGoS::PowerCube::CommandRunner(&kernel, Serial);
  //kernel.components["bob"] = echoerClass.createInstance(&kernel, "bob");

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
  kernel.update();
  if( kernel.getCurrentTickNumber() % 75 == 0 ) {
    kernel << TOGoS::PowerCube::ComponentMessage("bob", "hello", "123");
  }

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
