#include <Print.h>
#include "StringView.h"
#include "stream_operators.h"

Print & operator<<(Print &p, const TOGoS::StringView& sv)
{
  p.write(sv.begin(), sv.size());
  return p;
}
