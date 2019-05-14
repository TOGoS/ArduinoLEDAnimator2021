#ifndef _TOGOS_POWERCUBE_PWMSWITCH_H
#define _TOGOS_POWERCUBE_PWMSWITCH_H

#include <Print.h>

#include "Kernel.h"

namespace TOGoS { namespace PowerCube {
  /** Reads commands from a stream (e.g. Serial) nd forwards them */
  class PWMSwitch : public Component {
  private:
    KernelPtr kernel;
    std::string name;
    uint8_t pin;
    bool isActiveLow;
    int32_t targetLevel;
    int32_t level;
    bool currentInstantaneousState = 0;
  public:
    PWMSwitch(KernelPtr kernel, const StringView &name, uint8_t pin, bool isActiveLow);
    void set(float level);
    virtual void update() override;
    virtual void onMessage(const ComponentMessage& cm) override;
  };
}}

#endif
