#pragma once

#include "../StringView.h"

namespace TOGoS { namespace PowerCube {
  /** Buffers characters from serial until a full command is read */
  class CommandBuffer {
  public:
    enum class BufferState {
      READING,
      READY, // I read a command.  Now take it before feeding me more characters!
      OVERFLOWED
    };

    static const size_t bufferSize = 1024;
  private:
    char buffer[bufferSize];
    size_t pos;
    BufferState state;
  private:
    void reset();
    bool addChar(char c);
  public:
    CommandBuffer();
    BufferState onChar(char c);
    StringView str() const;
  };
}}
