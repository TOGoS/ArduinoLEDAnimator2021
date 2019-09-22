// The Arduino compiler doesn't seem to know about cpp
// files that aren't in the root sketch directory,
// so we'll include them here.

// Core stuff
#include "TOGoS/PowerCube/Kernel.cpp"
#include "TOGoS/PowerCube/Path.cpp"
#include "TOGoS/PowerCube/PathWithOwnData.cpp"
#include "TOGoS/stream_operators.cpp"

// Components
#include "TOGoS/PowerCube/CommandBuffer.cpp"
#include "TOGoS/PowerCube/CommandRunner.cpp"
#include "TOGoS/PowerCube/DigitalSwitch.cpp"
#include "TOGoS/PowerCube/DHTReader.cpp"
#include "TOGoS/PowerCube/FastLEDController.cpp"
#include "TOGoS/PowerCube/PWMSwitch.cpp"
#include "TOGoS/SSD1306/Controller.cpp"
#include "TOGoS/SSD1306/Printer.cpp"
#include "TOGoS/PowerCube/parseEnableMessage.cpp"
#include "TOGoS/PowerCube/parseBoolean.cpp"
