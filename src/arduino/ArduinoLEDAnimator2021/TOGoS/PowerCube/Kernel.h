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
#include "Path.h"

// TODO: Split this up

namespace TOGoS { namespace PowerCube {
  class Kernel;
  using KernelPtr = Kernel*;

  // PubBits identify a class of agent,
  // both for identifying where a message originated
  // and for indicating where a message should be delivered
  namespace PubBits {
    enum {
      Internal = 1,
      Serial = 2,
      Network = 4,
      All = 7,
    };
  };

  struct ComponentMessage {
    Path path;
    StringView payload;
    // TODO: Add 'origin'?  Because e.g. only want to reply with 'help text' to Serial commands.
    // Where to deliver?
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

  class Kernel {
  public:
    std::map<std::string,std::unique_ptr<Component>> components; // TODO: should probably be shared_ or unique_ptrs
    unsigned int currentTickNumber = 0;
    unsigned int getCurrentTickNumber() { return this->currentTickNumber; }
    void deliverMessage(const ComponentMessage &m);
    void update();
    Print& getLogStream() { return Serial; }
  };

  Kernel &operator<<(Kernel &kernel, const ComponentMessage &m);
}}

#endif
