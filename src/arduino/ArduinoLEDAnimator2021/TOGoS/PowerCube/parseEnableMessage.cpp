#include "parseBoolean.h"

namespace TOGoS { namespace PowerCube {
  bool parseEnableMessage(const ComponentMessage &cm, bool &enabled) {
    if( cm.path.length >= 2 && cm.path[1] == "enabled" && cm.path[2] == "set" ) {
      return parseBoolean(cm.payload, enabled);
    }
    return false;
  }
}}
