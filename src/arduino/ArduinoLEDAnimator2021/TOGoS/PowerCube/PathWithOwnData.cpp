#include "PathWithOwnData.h"

using PathWithOwnData = TOGoS::PowerCube::PathWithOwnData;

PathWithOwnData &PathWithOwnData::operator=(const Path &path) {
  size_t dataLength = 0;
  for( const StringView& c : path ) {
    dataLength += c.size()+1;
  }
  this->length = 0;
  this->data.reset(new char[dataLength]);//std::make_unique<char[]>(dataLength);
  size_t copied = 0;
  for( const StringView& c : path ) {
    char *segDest = this->data.get() + copied;
    memcpy(segDest, c.begin(), c.size());
    *this << StringView(segDest, c.size());
    copied += c.size();
    this->data[copied++] = 0;
  }
}
