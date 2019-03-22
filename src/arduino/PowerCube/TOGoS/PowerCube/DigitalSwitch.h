#ifndef _TOGOS_POWERCUBE_DIGITALSWITCH_H
#define _TOGOS_POWERCUBE_DIGITALSWITCH_H

#include <Print.h>

#include <DHT.h>

#include "Kernel.h"
#include "../StringView.h"
#include "../stream_operators.h"

namespace TOGoS { namespace PowerCube {
  /** Reads commands from a stream (e.g. Serial) nd forwards them */
  class DigitalSwitch : public Component {
  private:
    KernelPtr kernel;
    std::string name;
    uint8_t pin;
    bool isActiveLow;
    bool currentStateIsKnown = false;
    bool currentState;
    bool desiredStateIsKnown = false;
    bool desiredState;
    bool everSet = false;
    unsigned long lastSetTime = 0;
  public:
    unsigned long minimumSwitchInterval = 0;
    uint8_t pubBits = PubBits::All;
    DigitalSwitch(KernelPtr kernel, const StringView &name, uint8_t pin, bool isActiveLow=false);
    void set(bool state, bool force=false);
    virtual void update() override;
    virtual void onMessage(const ComponentMessage& cm) override;
  };
}}

#endif
