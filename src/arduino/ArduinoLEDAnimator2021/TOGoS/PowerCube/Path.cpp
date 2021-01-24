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

Path::Path(const char *parseMe) : Path(StringView(parseMe)) {}

Print &TOGoS::PowerCube::operator<<(Print &p, const Path &path) {
  for( size_t i=0; i<path.length; ++i ) {
    if( i > 0 ) p << "/";
    p << path[i];
  }
  return p;
}
