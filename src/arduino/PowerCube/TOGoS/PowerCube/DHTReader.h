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
    uint32_t lastReadTime = 0;
  public:
    uint32_t readInterval = 1000;
    DHTReader(KernelPtr kernel, const StringView& name, uint8_t pin, uint8_t type) : kernel(kernel), name(name), dht(pin,type) { }
    virtual void update() override;
  };
}}

#endif
