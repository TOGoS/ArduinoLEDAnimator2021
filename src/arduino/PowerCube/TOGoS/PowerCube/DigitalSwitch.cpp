#include <Arduino.h>
#include "DigitalSwitch.h"

namespace TOGoS { namespace PowerCube {
  DigitalSwitch::DigitalSwitch(KernelPtr kernel, const StringView &name, uint8_t pin, bool isActiveLow) : kernel(kernel), name(name), pin(pin), isActiveLow(isActiveLow) {
    pinMode(this->pin, OUTPUT);
  }

  void DigitalSwitch::set(bool state, bool force) {
    if( this->desiredStateIsKnown && this->desiredState == state ) return;
    this->desiredState = state;
    this->desiredStateIsKnown = true;
    if( this->currentStateIsKnown ) {
      if( this->currentState == state ) return;
      unsigned long elapsed = millis() - this->lastSetTime;
      if( force == false && elapsed < this->minimumSwitchInterval ) {
	this->kernel->getLogStream() << "# " << this->name << ": Want to switch to " << (state?"on":"off") << " but only " << elapsed << "ms of " << this->minimumSwitchInterval << "ms elapsed since last change\n";
	return;
      }
    }
    uint8_t value = (state ? HIGH : LOW) ^ (this->isActiveLow ? 1 : 0);
    this->kernel->getLogStream() << "# " << this->name << ": Setting pin " << this->pin << " to " << (value == LOW ? "LOW" : "HIGH") << "\n";
    digitalWrite(this->pin, value);
    this->currentState = state;
    this->currentStateIsKnown = true;
    this->everSet = true;
    this->lastSetTime = millis();
    this->kernel->deliverMessage(ComponentMessage(Path() << this->name << "state", state ? "on" : "off", this->pubBits));
  }
  void DigitalSwitch::update() {
    if( !this->desiredStateIsKnown ) return;
    this->set(this->desiredState, false);
  }
  void DigitalSwitch::onMessage(const ComponentMessage& cm) {
    if( cm.path.length == 3 && cm.path[0] == this->name && cm.path[1] == "state" && cm.path[2] == "set" ) {
      if( cm.payload == "on" ) this->set(true);
      else if( cm.payload == "off" ) this->set(false);
      else
	this->kernel->getLogStream() << "# " << this->name << " Unrecognized switch state: " << cm.payload << "\n";
    }
  }
}}
