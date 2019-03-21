#ifndef _TOGOS_POWERCUBE_KERNEL_H
#define _TOGOS_POWERCUBE_KERNEL_H

// Standard libraries
#include <map>

// Arduino libraries
#include <Print.h>
#include <HardwareSerial.h>

// My libraries
#include "../StringView.h"

// TODO: Split this up

namespace TOGoS { namespace PowerCube {
  class Kernel;
  using KernelPtr = Kernel*;

  struct Path {
    static const uint8_t maxPartCount = 2;
    uint8_t length = 0;
    StringView parts[maxPartCount];
    Path &operator<<(const StringView& p) {
      if( this->length < maxPartCount ) {
	this->parts[this->length++] = p;
      }
      return *this;
    }
    const StringView *begin() const { return this->parts; }
    const StringView *end() const { return this->parts + this->length; }
    const StringView& operator[](uint8_t index) const { return this->parts[index]; }
    StringView& operator[](uint8_t index) { return this->parts[index]; }
  };
  Print &operator<<(Print& p, const Path& path);

  namespace PubBits {
    enum {
      Internal = 1,
      Serial = 2,
      Network = 4
    };
  };

  struct ComponentMessage {
    Path path;
    StringView payload;
    uint8_t pubBits = 0;
    ComponentMessage() {}
    ComponentMessage(const Path &path, const StringView &p, uint8_t pubBits)
      : path(path), payload(p), pubBits(pubBits) {}
  };
  Print &operator<<(Print& p, const ComponentMessage& cm);

  class Component {
  public:
    virtual void update() {}
    virtual void onMessage(const ComponentMessage& m) {}
    virtual ~Component() = default;
  };
  class ComponentClass {
  public:
    virtual Component *createInstance(KernelPtr kernel, const StringView &name) = 0;
    virtual void deleteInstance(Component *) = 0;
  };

  class Kernel {
  public:
    std::map<std::string,ComponentClass*> componentClasses; // TODO: should probably be shared_ or unique_ptrs
    std::map<std::string,Component*> components; // TODO: should probably be shared_ or unique_ptrs
    unsigned int currentTickNumber = 0;
    unsigned int getCurrentTickNumber() { return this->currentTickNumber; }
    void deliverMessage(const ComponentMessage &m);
    void update();
    Print& getLogStream() { return Serial; }
  };

  Kernel &operator<<(Kernel &kernel, const ComponentMessage &m);
}}

#endif
