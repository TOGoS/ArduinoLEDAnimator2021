#ifndef _TOGOS_POWERCUBE_PATHWITHOWNDATA_H
#define _TOGOS_POWERCUBE_PATHWITHOWNDATA_H

namespace TOGoS { namespace PowerCube {
  struct PathWithOwnData : public Path {
    std::unique_ptr<char[]> data;
    PathWithOwnData() = default;
    PathWithOwnData &operator=(const Path &path);
  };
}}

#endif
