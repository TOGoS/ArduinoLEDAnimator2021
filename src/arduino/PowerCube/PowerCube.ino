#include <Wire.h>
#include <Print.h>
#include <map>

#include "TOGoS/SSD1306/font8x8.h"
#include "TOGoS/SSD1306/Controller.h"
#include "TOGoS/SSD1306/Printer.h"

/** Intended as a drop-in replacement for std::string_view */
struct string_view {
  const char *_begin;
  size_t _size;
  string_view(const char *str) : _begin(str), _size(strlen(str)) {}
  const char *begin() const { return this->_begin; }
  const char *end() const { return this->_begin + this->_size; }
  size_t size() const { return this->_size; }
  operator std::string() const { return std::string(this->begin(), this->size()); }
};

Print & operator<<(Print &p, const string_view& sv) {
  p.write(sv.begin(), sv.size());
}

namespace TOGoS { namespace PowerCube {
  class Kernel;

  struct ComponentMessage {
    string_view componentName;
    string_view subTopic;
    string_view payload;
    ComponentMessage(const string_view &c, const string_view &t, const string_view &p) :
      componentName(c), subTopic(t), payload(p) {}
  };

  class Component {
  public:
    virtual void update() = 0;
    virtual void onMessage(const ComponentMessage& m) = 0;
    virtual ~Component() = default;
  };
  class ComponentClass {
  public:
    virtual Component *createInstance(Kernel *kernel, const string_view &name) = 0;
    virtual void deleteInstance(Component *) = 0;
  };

  class Kernel {
  public:
    std::map<std::string,ComponentClass*> componentClasses;
    std::map<std::string,Component*> components;
    unsigned int currentTickNumber = 0;
    unsigned int getCurrentTickNumber() { return this->currentTickNumber; }
    void initialize() {
    }
    void deliverMessage(const ComponentMessage &m) {
      if( this->components.count(m.componentName) ) {
        this->components[m.componentName]->onMessage(m);
      }
    }
    void update() {
      for (auto &c : this->components) {
        c.second->update();
      }
      ++currentTickNumber;
    }
  };

  Kernel &operator<<(Kernel &kernel, const ComponentMessage &m) {
    kernel.deliverMessage(m);
    return kernel;
  }

  class Echoer : public Component {
    Kernel *kernel;
    std::string name;
  public:
    Echoer(Kernel *kernel, const string_view &name) : kernel(kernel), name(name) {
    }
    virtual void onMessage(const ComponentMessage& m) override {
      Serial << m.componentName;
      Serial << ": got ";
      Serial << m.subTopic;
      Serial << " message: ";
      Serial << m.payload;
      Serial << "\n";
    }
    virtual void update() override {
      if( this->kernel->getCurrentTickNumber() % 100 == 0 ) {
        Serial.print(this->name.c_str());
        Serial.print(": Bro, it's ");
        Serial.println(this->kernel->getCurrentTickNumber());
      }
    }
  };
  class EchoerClass : public ComponentClass {
  public:
    virtual Component *createInstance(Kernel *kernel, const string_view &name) override {
      return new Echoer(kernel, name);
    }
    virtual void deleteInstance(Component *comp) {
      delete comp;
    }
  };
}}

TOGoS::SSD1306::Controller oledController(Wire, 0x3C);
TOGoS::SSD1306::Printer oledPrinter(oledController, font8x8);

TOGoS::PowerCube::EchoerClass echoerClass;

TOGoS::PowerCube::Kernel kernel;

void setup() {
  Serial.begin(115200);
  kernel.initialize();
  kernel.components["bob"] = echoerClass.createInstance(&kernel, "bob");

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
