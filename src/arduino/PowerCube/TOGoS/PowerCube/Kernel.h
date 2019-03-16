#ifndef _TOGOS_POWERCUBE_KERNEL_H
#define _TOGOS_POWERCUBE_KERNEL_H

// Standard libraries
#include <map>
#include <memory>

// Arduino libraries
#include <Print.h>
#include <HardwareSerial.h>

// My libraries
#include "../StringView.h"
#include "../stream_operators.h"

// TODO: Split this up

namespace TOGoS { namespace PowerCube {
  class Kernel;

  struct Path {
    static const uint8_t maxPartCount = 2;
    uint8_t length;
    StringView parts[maxPartCount];
    Path &operator<<(const StringView& p) {
      if( this->length < maxPartCount ) {
	this->parts[this->length++] = p;
      }
    }
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
    uint8_t pubBits;
    ComponentMessage(const Path &path, const StringView &p, uint8_t pubBits)
      : path(path), payload(p), pubBits(pubBits) {}
  };

  class Component {
  public:
    virtual void update() {}
    virtual void onMessage(const ComponentMessage& m) {}
    virtual ~Component() = default;
  };

  class Kernel {
  public:
    std::map<std::string,std::unique_ptr<Component>> components; // TODO: should probably be shared_ or unique_ptrs
    unsigned int currentTickNumber = 0;
    unsigned int getCurrentTickNumber() { return this->currentTickNumber; }
    void initialize() {
    }
    void deliverMessage(const ComponentMessage &m) {
      if( (m.pubBits & PubBits::Serial) != 0 ) {
	this->getLogStream() << m.path << " " << m.payload << "\n";
      }
      if( (m.pubBits & PubBits::Internal) != 0 &&
	  m.path.length >= 1 && this->components.count(m.path[0]) ) {
        this->components[m.path[0]]->onMessage(m);
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
}}

#endif
