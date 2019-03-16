#include "Kernel.h"

using ComponentMessage = TOGoS::PowerCube::ComponentMessage;
using Kernel = TOGoS::PowerCube::Kernel;

Kernel &TOGoS::PowerCube::operator<<(Kernel &kernel, const ComponentMessage &m) {
  kernel.deliverMessage(m);
  return kernel;
}
