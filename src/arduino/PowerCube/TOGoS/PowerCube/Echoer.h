#ifndef _TOGOS_POWERCUBE_ECHOER_H
#define _TOGOS_POWERCUBE_ECHOER_H

#include <Print.h>

#include "Kernel.h"
#include "../stream_operators.h"

namespace TOGoS { namespace PowerCube {
  /** Reads commands from a stream (e.g. Serial) nd forwards them */
  class Echoer : public Component {
  private:
    Print& stream;
  public:
    Echoer(Print& stream) : stream(stream) { }
    virtual void onMessage(const ComponentMessage& m) {
      this->stream << "# Echoer received message: " << m << "\n";
    }
  };
}}

#endif
