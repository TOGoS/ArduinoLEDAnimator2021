#include "CommandRunner.h"

using Kernel = TOGoS::PowerCube::Kernel;
namespace PubBits = TOGoS::PowerCube::PubBits;
using CommandRunner = TOGoS::PowerCube::CommandRunner;

CommandRunner::CommandRunner(Kernel *kernel, Stream& stream) : kernel(kernel), stream(stream) {}  

static bool isWhitespace(char c) {
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

ComponentMessage parseMessage(const StringView& str) {
  const char *c = str.begin();
  char const * const end = c + str.size();
  while( isWhitespace(*c) ) ++c;

  ComponentMessage mess;
  if( c == end || *c == '#' ) return mess;

  const char *pathSegBegin = c;
  while( c != end && !isWhitespace(*c) ) {
    if( *c == '/' ) {
      mess.path << StringView(pathSegBegin, c-pathSegBegin);
      pathSegBegin = c+1;
    }
    ++c;
  }
  mess.path << StringView(pathSegBegin, c-pathSegBegin);
  while( c != end && isWhitespace(*c) ) ++c;

  mess.payload = StringView(c, end - c);
  return mess;
}

void CommandRunner::update() {
  while( this->stream.available() > 0 ) {
    using CommandBuffer = TOGoS::PowerCube::CommandBuffer;
    CommandBuffer::BufferState bufState = this->commandBuffer.onChar(this->stream.read());
    if( bufState == CommandBuffer::BufferState::READY ) {
      ComponentMessage cm = parseMessage(this->commandBuffer.str());
      cm.pubBits = PubBits::Internal;
      *this->kernel << cm;
      //this->kernel->getLogStream() << "# bits = " << (uint32_t)cm.pubBits << "\n"; // << cm << "\n";
      //for( auto &ps : cm.path ) this->kernel->getLogStream() << "# Path segment: " << ps << "\n";
      //this->kernel->getLogStream() << "# Payload: " << cm.payload << "\n";
      //this->kernel->getLogStream() << "# Read command: " << this->commandBuffer.str() << "\n";
    }
  }
}
