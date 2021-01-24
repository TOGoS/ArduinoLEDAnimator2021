#pragma once

#include <Stream.h>

#include "CommandBuffer.h"
#include "Kernel.h"

namespace TOGoS { namespace PowerCube {
  /** Reads commands from a stream (e.g. Serial) nd forwards them */
  class CommandRunner : public Component {
  private:
    CommandBuffer commandBuffer;
    Kernel *kernel;
    Stream& stream;
  public:
    CommandRunner(Kernel *kernel, Stream& stream);
    virtual void update() override;
  };
}}
