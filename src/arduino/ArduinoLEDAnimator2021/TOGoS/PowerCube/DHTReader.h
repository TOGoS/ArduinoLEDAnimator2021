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
    float prevHumidity = NAN;
    unsigned long prevHumidityReportTime = 0;
    float prevTemperature = NAN;
    unsigned long prevTemperatureReportTime = 0;
    bool enabled = true;
  public:
    unsigned long readInterval = 1000;
    unsigned long maxReportInterval = 60000;
    DHTReader(KernelPtr kernel, const StringView& name, uint8_t pin, uint8_t type);
    virtual void update() override;
    virtual void onMessage(const ComponentMessage &cm) override;
  };
}}

#endif
