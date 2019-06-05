#include "../BufferPrint.h"
#include "Animator.h"

namespace TOGoS { namespace PowerCube {
  using WaveType = Animator::WaveType;
  using Channel = Animator::Channel;

  void Channel::randomize() {
    this->waveType = (WaveType)random(0, (int)WaveType::_Count);
    this->period = random(1, 360)/6.0;
    this->phase = random(0, 1000)/1000.0;
    this->minValue = random(0, 32768)/32768.0;
    this->maxValue = this->minValue + random(0, 32768)/32768.0 * (1 - this->minValue);
  }
  float Channel::nextValue(float dt) {
    if( this->waveType == WaveType::Flicker ) {
      if( random(0,1000) / 1000.0 < (dt/this->period) ) {
	this->phase = random(0,1000) / 1000.0;
      }
      return this->minValue + this->phase * (this->maxValue - this->minValue);
    }
    
    this->phase += dt / this->period;
    int pfloor = floor(this->phase);
    this->phase -= pfloor;
    float v;
    switch( this->waveType ) {
    case WaveType::Triangle:
      if( this->phase > 0.5 ) v = 2 * (1 - this->phase);
      else v = 2 * this->phase;
      break;
    case WaveType::Square:
      if( this->phase > 0.5 ) v = 1;
      else v = 0;
    }
    return this->minValue + v * (this->maxValue - this->minValue);
  }
    
  Animator::Animator(KernelPtr kernel, const StringView& name)
    : kernel(kernel), name(name) { }
  Channel *Animator::getChannel(uint8_t channelNumber) {
    if( channelNumber >= this->channels.size() ) {
      this->kernel->getLogStream() << "# " << this->name << ": Channel index out of range: " << channelNumber << " >= " << this->channels.size() << "\n";
      return nullptr;
    }
    return &this->channels[channelNumber];
  }
  void Animator::update() {
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - this->previousUpdate;
    float elapsedTimeSeconds = elapsedTime / 1000.0;
    for( Channel &c : this->channels ) {
      float v = c.nextValue(elapsedTimeSeconds);
      
      char message[10];
      BufferPrint bp(message, sizeof(message));
      bp << v;
      
      this->kernel->deliverMessage(ComponentMessage(c.targetPath, bp.str(), c.pubBits));
    }
    this->previousUpdate = currentTime;
  }
  void Animator::onMessage(const ComponentMessage &cm) {
    if( cm.path.length >= 1 && cm.path[0] == this->name ) {
      if( cm.path.length == 2 && cm.path[1] == "randomize" ) {
	for( Channel &c : this->channels ) c.randomize();
      }
      if( cm.path.length == 2 && cm.path[1] == "channelcount" ) {
	unsigned int count;
	if( !sscanf(cm.payload.begin(), "%u", &count) ) {
	  this->kernel->getLogStream() << "# " << this->name << ": Bad channel count: '" << cm.payload << "'\n";
	  return;
	}
	if( count > maxChannelCount ) {
	  this->kernel->getLogStream() << "# " << this->name << ": Channel count too high: " << count << "\n";
	  return;
	}
	this->channels.resize(count);
	this->kernel->getLogStream() << "# " << this->name << ": set channel count to " << count << "\n";
	return;
      }
      if( cm.path.length >= 5 && cm.path[1] == "channels" && cm.path[4] == "set" ) {
	const StringView &propName = cm.path[3];
	const StringView &channelName = cm.path[2];
	unsigned int channelNumber;
	if( !sscanf(channelName.begin(), "%u", &channelNumber) ) {
	  this->kernel->getLogStream() << "# " << this->name << ": Invalid channel name: '" << channelName << "'\n";
	  return;
	}
	Channel *chan = this->getChannel(channelNumber);
	if( chan == nullptr ) {
	  this->kernel->getLogStream() << "# " << this->name << ": No such channel: " << channelNumber << "\n";
	  return;
	} else if( propName == "targetpath" ) {
	  chan->targetPath = Path(cm.payload);
	  this->kernel->getLogStream() << "# " << this->name << ": set channel " << channelName << "'s target path to '" << cm.payload << "'\n";
	  return;
	}
      }
    }
  }
}}
