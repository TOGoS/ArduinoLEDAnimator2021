#include "Kernel.h"
#include "../stream_operators.h"

using ComponentMessage = TOGoS::PowerCube::ComponentMessage;
using Kernel = TOGoS::PowerCube::Kernel;
using Path = TOGoS::PowerCube::Path;

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
