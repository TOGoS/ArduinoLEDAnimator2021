#ifndef _TOGOS_POWERCUBE_PATH_H
#define _TOGOS_POWERCUBE_PATH_H

#include <cstdint>
#include <Print.h>
#include "../StringView.h"

namespace TOGoS { namespace PowerCube {
  struct Path {
    Path() = default;
    Path(const StringView& parseMe);
    Path(const char *parseMe);
    static const uint8_t maxPartCount = 5;
    uint8_t length = 0;
    StringView parts[maxPartCount];
    Path &operator<<(const StringView& p) {
      if( this->length < maxPartCount ) {
        this->parts[this->length++] = p;
      }
      return *this;
    }
    const StringView *begin() const { return this->parts; }
    const StringView *end() const { return this->parts + this->length; }
    const StringView& operator[](uint8_t index) const { return this->parts[index]; }
    StringView& operator[](uint8_t index) { return this->parts[index]; }
  };
  Print &operator<<(Print& p, const Path& path);
}}

#endif
