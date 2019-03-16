#ifndef _TOGOS_POWERCUBE_KERNEL_H
#define _TOGOS_POWERCUBE_KERNEL_H

// Standard libraries
#include <map>

// Arduino libraries
#include <Print.h>
#include <HardwareSerial.h>

// My libraries
#include "../StringView.h"
#include "../stream_operators.h"

// TODO: Split this up

namespace TOGoS { namespace PowerCube {
  class Kernel;

  struct ComponentMessage {
    StringView componentName;
    StringView subTopic;
    StringView payload;
    ComponentMessage(const StringView &c, const StringView &t, const StringView &p) :
      componentName(c), subTopic(t), payload(p) {}
  };

  class Component {
  public:
    virtual void update() {}
    virtual void onMessage(const ComponentMessage& m) {}
    virtual ~Component() = default;
  };
  class ComponentClass {
  public:
    virtual Component *createInstance(Kernel *kernel, const StringView &name) = 0;
    virtual void deleteInstance(Component *) = 0;
  };

  class Kernel {
  public:
    std::map<std::string,ComponentClass*> componentClasses; // TODO: should probably be shared_ or unique_ptrs
    std::map<std::string,Component*> components; // TODO: should probably be shared_ or unique_ptrs
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
    Print& getLogStream() { return Serial; }
  };

  Kernel &operator<<(Kernel &kernel, const ComponentMessage &m);

  class Echoer : public Component {
    Kernel *kernel;
    std::string name;
  public:
    Echoer(Kernel *kernel, const StringView &name) : kernel(kernel), name(name) {
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
    virtual Component *createInstance(Kernel *kernel, const StringView &name) override {
      return new Echoer(kernel, name);
    }
    virtual void deleteInstance(Component *comp) {
      delete comp;
    }
  };
}}

#endif
