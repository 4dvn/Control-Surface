#include <Control_Surface.h>

USBMIDI_Interface midi;

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
  Control_Surface.begin();
}

void loop() {
  Control_Surface.loop();
}
