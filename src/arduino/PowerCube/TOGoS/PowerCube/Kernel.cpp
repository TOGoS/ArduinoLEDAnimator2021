#include "Kernel.h"
#include "../stream_operators.h"

using ComponentMessage = TOGoS::PowerCube::ComponentMessage;
using Kernel = TOGoS::PowerCube::Kernel;
using Path = TOGoS::PowerCube::Path;

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
