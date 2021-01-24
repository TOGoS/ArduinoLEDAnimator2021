#ifndef _TOGOS_POWERCUBE_PWMSWITCH_H
#define _TOGOS_POWERCUBE_PWMSWITCH_H

#include <Print.h>

#include "Kernel.h"

#define TOGOS_POWERCUBE_PWMSWITCH_MODE_ANALOGWRITE 1 // Use the platform's built-in PWM capabilities
#define TOGOS_POWERCUBE_PWMSWITCH_MODE_EMULATED 2
#ifndef TOGOS_POWERCUBE_PWMSWITCH_MODE
#define TOGOS_POWERCUBE_PWMSWITCH_MODE TOGOS_POWERCUBE_PWMSWITCH_MODE_ANALOGWRITE
#endif

namespace TOGoS { namespace PowerCube {
  /** Reads commands from a stream (e.g. Serial) nd forwards them */
  class PWMSwitch : public Component {
  private:
    KernelPtr kernel;
    std::string name;
    uint8_t pin;
    bool isActiveLow;
#if TOGOS_POWERCUBE_PWMSWITCH_MODE==TOGOS_POWERCUBE_PWMSWITCH_MODE_EMULATED
    int32_t targetLevel;
    int32_t level;
    bool currentInstantaneousState = 0;
#endif    
  public:
    PWMSwitch(KernelPtr kernel, const StringView &name, uint8_t pin, bool isActiveLow);
    void set(float level);
    virtual void update() override;
    virtual void onMessage(const ComponentMessage& cm) override;
  };
}}

#endif
