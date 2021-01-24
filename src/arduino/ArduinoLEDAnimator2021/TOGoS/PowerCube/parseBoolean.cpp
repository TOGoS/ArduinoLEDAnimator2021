#include "parseBoolean.h"

namespace TOGoS { namespace PowerCube {
  bool parseBoolean(const StringView &text, bool &result) {
    if( text == "yes" || text == "true" || text == "1" || text == "on" || text == "enabled" ) {
      result = true;
      return true;
    }
    if( text == "no" || text == "false" || text == "0" || text == "off" || text == "disabled" ) {
      result = false;
      return true;
    }
    return false;
  }
}}
