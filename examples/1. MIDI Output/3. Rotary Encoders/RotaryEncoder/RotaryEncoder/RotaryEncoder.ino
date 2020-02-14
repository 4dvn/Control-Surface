#include <Encoder.h> // Include the Encoder library.
// This must be done before the Control Surface library.
#include <Control_Surface.h> // Include the Control Surface library

// Instantiate a MIDI over USB interface.
USBMIDI_Interface midi;

// Instantiate a CCRotaryEncoder object
CCRotaryEncoder rotaryencoder = {{2, 3}, MIDI_CC::Channel_Volume, 1,};

void setup() {
  RelativeCCSender::setMode(relativeCCmode::MACKIE_CONTROL_RELATIVE);
  Control_Surface.begin(); // Initialize Control Surface
}

void loop() {
  Control_Surface.loop(); // Update the Control Surface
}
