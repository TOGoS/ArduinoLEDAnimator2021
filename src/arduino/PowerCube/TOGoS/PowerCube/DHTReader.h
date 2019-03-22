#ifndef _TOGOS_POWERCUBE_DHT22_H
#define _TOGOS_POWERCUBE_DHT22_H

#include <Print.h>

#include <DHT.h>

#include "Kernel.h"
#include "../StringView.h"
#include "../stream_operators.h"

namespace TOGoS { namespace PowerCube {
  /** Reads commands from a stream (e.g. Serial) nd forwards them */
  class DHTReader : public Component {
  private:
    KernelPtr kernel;
    std::string name;
    DHT dht;
    unsigned long lastReadTime = 0;
  public:
    unsigned long readInterval = 1000;
    DHTReader(KernelPtr kernel, const StringView& name, uint8_t pin, uint8_t type) : kernel(kernel), name(name), dht(pin,type) { }
    virtual void update() override;
  };
}}

#endif
