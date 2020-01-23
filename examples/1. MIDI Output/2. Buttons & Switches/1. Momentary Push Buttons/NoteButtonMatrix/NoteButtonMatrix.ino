/**
 * This examples shows how to use a button matrix to read many switches.
 * 
 * @boards  AVR, AVR USB, Due, Nano 33, Teensy 3.x, ESP32
 * 
 * Connections
 * -----------
 * 
 * Connect a 4 × 4 matrix of buttons with the rows to pins 2, 3, 4 and 5, 
 * and the columns to pins 6, 7, 8 and 9.
 * 
 * Pull-up resistors are not necessary, because the internal ones will be used. 
 * Required diode zener 1N4148 for multipresser buttons at the same time
 *
 * If you want to be able to press multiple buttons at once, add a diode 
 * in series with each button:  
 * @image html Button-matrix.png
 * 
 * Behavior
 * --------
 * 
 * When you press one of the buttons, a note on event for the corresponding note
 * is sent, when you release it, a note off event is sent.
 * 
 * Mapping
 * -------
 *
 * The note numbers are specified in the `addresses` array.
 * Map accordingly in your DAW or DJ software.
 * Ableton drumrack map added
 *
 * Written by Pieter P, 24/09/2017  
 * https://github.com/tttapa/Control-Surface
 */

#include <Control_Surface.h>

USBMIDI_Interface midi;

// The note numbers corresponding to the buttons in the matrix
const AddressMatrix<4, 4> addresses = {{
  {51, 50, 49, 48}, // Ableton drumrack map
  {47, 46, 45, 44},
  {43, 42, 41, 40},
  {39, 38, 37, 36},
}};

NoteButtonMatrix<4, 4> buttonmatrix = {
  {2, 3, 4, 5}, // row pins
  {6, 7, 8, 9},    // column pins
  addresses,    // address matrix
  CHANNEL_1,    // channel and cable number
};

void setup() {
  Control_Surface.begin();
}

void loop() {
  Control_Surface.loop();
}
