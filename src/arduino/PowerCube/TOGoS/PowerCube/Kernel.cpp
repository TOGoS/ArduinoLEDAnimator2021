#include <cstring>

#include "Kernel.h"
#include "../stream_operators.h"

using ComponentMessage = TOGoS::PowerCube::ComponentMessage;
using Kernel = TOGoS::PowerCube::Kernel;
using Path = TOGoS::PowerCube::Path;
using PathWithOwnData = TOGoS::PowerCube::PathWithOwnData;

Path::Path(const StringView& parseMe) {
  const char *c = parseMe.begin();
  char const * const end = c + parseMe.size();
  const char *pathSegBegin = c;
  while( c != end ) {
    if( *c == '/' ) {
      *this << StringView(pathSegBegin, c-pathSegBegin);
      pathSegBegin = c+1;
    }
    ++c;
  }
}


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

void Kernel::deliverMessage(const ComponentMessage &m) {
  //this->getLogStream() << "# Kernel: Delivering message: " << m << " (bits=" << m.pubBits << ")\n";
  if( (m.pubBits & PubBits::Serial) != 0 ) {
    this->getLogStream() << m.path << " " << m.payload << "\n";
  }
  if( (m.pubBits & PubBits::Internal) != 0 &&
      m.path.length >= 1 && this->components.count(m.path[0]) ) {
    this->components[m.path[0]]->onMessage(m);
  }
}
void Kernel::update() {
  for (auto &c : this->components) {
    c.second->update();
  }
  ++currentTickNumber;
}

Print &TOGoS::PowerCube::operator<<(Print& p, const Path& path) {
  for( uint32_t i=0; i<path.length; ++i ) {
    if( i != 0 ) p << "/";
    p << path.parts[i];
  }
  return p;
}

Kernel &TOGoS::PowerCube::operator<<(Kernel &kernel, const ComponentMessage &m) {
  kernel.deliverMessage(m);
  return kernel;
}

Print &TOGoS::PowerCube::operator<<(Print &p, const ComponentMessage &cm) {
  return p << cm.path << " " << cm.payload;
}
