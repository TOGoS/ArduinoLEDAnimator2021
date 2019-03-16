#ifndef _TOGOS_STREAM_OPERATORS_H
#define _TOGOS_STREAM_OPERATORS_H

class TOGoS::StringView;
class Print;

Print & operator<<(Print &p, const TOGoS::StringView& sv);

#endif
