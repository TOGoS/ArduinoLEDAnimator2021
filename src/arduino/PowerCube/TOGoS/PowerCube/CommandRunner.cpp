#include "CommandRunner.h"

using Kernel = TOGoS::PowerCube::Kernel;
using CommandRunner = TOGoS::PowerCube::CommandRunner;

CommandRunner::CommandRunner(Kernel *kernel, Stream& stream) : kernel(kernel), stream(stream) {}  

void CommandRunner::update() {
  while( this->stream.available() > 0 ) {
    using CommandBuffer = TOGoS::PowerCube::CommandBuffer;
    CommandBuffer::BufferState bufState = this->commandBuffer.onChar(this->stream.read());
    if( bufState == CommandBuffer::BufferState::READY ) {
      this->kernel->getLogStream() << "# Read command: " << this->commandBuffer.str() << "\n";
    }
  }
}
