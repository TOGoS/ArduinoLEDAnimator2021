#include <Arduino.h>
#include <DHT.h>
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


void DHTReader::update() {
  unsigned long currentTime = millis();
  if( this->lastReadTime == 0 || currentTime - this->lastReadTime > this->readInterval ) {
    char formatted[10];
    BufferPrint formatter(formatted, sizeof(formatted));
    float temp = this->dht.readTemperature();
    float humid = this->dht.readHumidity();

    formatter.clear();
    formatter << temp;
    this->kernel->deliverMessage(ComponentMessage(Path() << this->name << "temperature", formatter.str(), PubBits::Internal|PubBits::Serial));

    formatter.clear();
    formatter << humid;
    this->kernel->deliverMessage(ComponentMessage(Path() << this->name << "humidity", formatter.str(), PubBits::Internal|PubBits::Serial));

    this->lastReadTime = currentTime;
  }
}
