#include "Path.h"

using Path = TOGoS::PowerCube::Path;

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
