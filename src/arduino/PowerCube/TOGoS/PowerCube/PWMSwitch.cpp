#include <Arduino.h>
#include "PWMSwitch.h"

namespace TOGoS { namespace PowerCube {
  PWMSwitch::PWMSwitch(KernelPtr kernel, const StringView &name, uint8_t pin, bool isActiveLow) : kernel(kernel), name(name), pin(pin), isActiveLow(isActiveLow) {
    pinMode(this->pin, OUTPUT);
  }
  void PWMSwitch::set(float level) {
    if( level < 0 ) level = 0;
    if( level > 1 ) level = 1;
    if( this->isActiveLow ) level = 1 - level;
#if TOGOS_POWERCUBE_PWMSWITCH_MODE==TOGOS_POWERCUBE_PWMSWITCH_MODE_EMULATED
    this->level = this->targetLevel = level * 65536;
#else
    analogWrite(this->pin, level * PWMRANGE);
#endif
  }
  void PWMSwitch::update() {
#if TOGOS_POWERCUBE_PWMSWITCH_MODE==TOGOS_POWERCUBE_PWMSWITCH_MODE_EMULATED
    bool targetSubstate = this->level > 32768;
    if( targetSubstate != this->currentInstantaneousState ) {
      digitalWrite(this->pin, (targetSubstate != this->isActiveLow) ? HIGH : LOW);
      this->currentInstantaneousState = targetSubstate;
    }
    int instLevel = this->currentInstantaneousState ? 65536 : 0;
    int quantError = this->level - instLevel;
    this->level = this->targetLevel + quantError;
#endif
  }
  void PWMSwitch::onMessage(const ComponentMessage& cm) {
    if( cm.path.length == 3 && cm.path[0] == this->name && (cm.path[1] == "state" || cm.path[1] == "level") && cm.path[2] == "set" ) {
      float level;
      if( cm.payload == "on" ) this->set(1.0);
      else if( cm.payload == "off" ) this->set(0.0);
      else if( sscanf(cm.payload.begin(), "%f", &level) ) this->set(level);
      else
	this->kernel->getLogStream() << "# " << this->name << " Unrecognized PWM switch state: " << cm.payload << "\n";
    }
  }
}}
