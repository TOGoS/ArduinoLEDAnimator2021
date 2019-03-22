#include <Arduino.h>
#include <DHT.h>
#include <math.h>
#include "DHTReader.h"
#include "../BufferPrint.h"

using Kernel = TOGoS::PowerCube::Kernel;
namespace PubBits = TOGoS::PowerCube::PubBits;
using DHTReader = TOGoS::PowerCube::DHTReader;

DHTReader::DHTReader(KernelPtr kernel, const StringView& name, uint8_t pin, uint8_t type)
  : kernel(kernel), name(name), dht(pin,type)
{
  this->dht.begin();
}

static bool isSame(float a, float b) {
  if( isnan(a) && isnan(b) ) return true;
  return a == b;
}

void DHTReader::update() {
  unsigned long currentTime = millis();
  if( this->lastReadTime == 0 || currentTime - this->lastReadTime > this->readInterval ) {
    char formatted[10];
    BufferPrint formatter(formatted, sizeof(formatted));
    float temp = this->dht.readTemperature();
    float humid = this->dht.readHumidity();

    if( !isSame(temp, this->prevTemperature) || currentTime - this->prevTemperatureReportTime > this->maxReportInterval ) {
      formatter.clear();
      formatter << temp;
      this->kernel->deliverMessage(ComponentMessage(Path() << this->name << "temperature", formatter.str(), PubBits::All));
      this->prevTemperature = temp;
      this->prevTemperatureReportTime = currentTime;
    }

    if( !isSame(humid, this->prevHumidity) || currentTime - this->prevHumidityReportTime > this->maxReportInterval ) {
      formatter.clear();
      formatter << humid;
      this->kernel->deliverMessage(ComponentMessage(Path() << this->name << "humidity", formatter.str(), PubBits::All));
      this->prevHumidity = humid;
      this->prevHumidityReportTime = currentTime;
    }

    this->lastReadTime = currentTime;
  }
}
