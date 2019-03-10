#pragma once

namespace TOGoS {
  namespace PowerCube {
    class Component {
      virtual void setEnabled(bool enabled);
      virtual bool getEnabled() const;
    };
  }
}
