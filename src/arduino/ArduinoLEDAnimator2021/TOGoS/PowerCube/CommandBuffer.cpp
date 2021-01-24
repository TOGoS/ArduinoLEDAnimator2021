#include "CommandBuffer.h"

using CommandBuffer = TOGoS::PowerCube::CommandBuffer;
using StringView = TOGoS::StringView;

CommandBuffer::CommandBuffer() {
  this->reset();
}
void CommandBuffer::reset() {
  this->buffer[bufferSize-1] = 0;
  this->pos = 0;
  this->state = BufferState::READING;
}
bool CommandBuffer::addChar(char c) {
  if( pos >= bufferSize - 1 ) return false;
  buffer[pos++] = c;
  return true;
}
CommandBuffer::BufferState CommandBuffer::onChar(char c) {
  if( this->state == BufferState::READY ) {
    // Clear it out!
    pos = 0;
  }
  if( c == '\n' || c == '\r' ) {
    buffer[pos] = 0;
    return this->state = BufferState::READY;
  }
  if( addChar(c) ) return this->state = BufferState::READING;
  return this->state = BufferState::OVERFLOWED;
}
StringView CommandBuffer::str() const {
  return StringView(buffer, pos);
}
