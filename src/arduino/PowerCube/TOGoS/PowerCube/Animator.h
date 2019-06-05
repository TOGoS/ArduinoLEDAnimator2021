#ifndef _TOGOS_POWERCUBE_ANIMATOR_H
#define _TOGOS_POWERCUBE_ANIMATOR_H

#include <Print.h>

#include <DHT.h>

#include "Kernel.h"
#include "../StringView.h"
#include "../stream_operators.h"

namespace TOGoS { namespace PowerCube {
  class Animator : public Component {
  public:
    enum class WaveType {
      Triangle,
      Square,
      Flicker,
      _Count
    };
    struct Channel {
      bool enabled = false;
      PathWithOwnData targetPath;
      uint8_t pubBits = PubBits::Internal;
      float period;
      float phase;
      float minValue;
      float maxValue;
      WaveType waveType;
      void randomize();
      float nextValue(float dt);
    };
  protected:
    KernelPtr kernel;
    std::string name;
    std::vector<Channel> channels;
    static constexpr unsigned int maxChannelCount = 16;
    unsigned long previousUpdate = 0;
    Channel *getChannel(uint8_t channelNumber);
    static WaveType parseWaveType(const StringView& name);
  public:
    Animator(KernelPtr kernel, const StringView& name);
    virtual void update() override;
    virtual void onMessage(const ComponentMessage &cm) override;
  };
}}

#endif
