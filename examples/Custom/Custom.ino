#include <Encoder.h>
#include <Control_Surface.h>

USBMIDI_Interface midi;

CCPotentiometer potentiometer = {
  A0,                                   // Analog pin connected to potentiometer
  {MIDI_CC::Channel_Volume, CHANNEL_1}, // Channel volume of channel 1
};

CCRotaryEncoder enc = {
  {6, 7},       // pins
  MCU::V_POT_1, // MIDI address (CC number + optional channel)
  1,            // optional multiplier if the control isn't fast enough
};

// The note numbers corresponding to the buttons in the matrix
const AddressMatrix<2, 2> addresses = {{
  {39, 38}, // Ableton drumrack map
  {37, 36},
}};

NoteButtonMatrix<2, 2> buttonmatrix = {
  {2, 3}, // row pins
  {4, 5},    // column pins
  addresses,    // address matrix
  CHANNEL_1,    // channel and cable number
};

void setup() {
  relativeccsender::setMode(relativeCCmode::MACKIE_CONTROL_RELATIVE);
  Control_Surface.begin();
}

void loop() {
  Control_Surface.loop();
}
