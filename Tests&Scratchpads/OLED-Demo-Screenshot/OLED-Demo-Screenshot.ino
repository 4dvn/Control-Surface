/**
   @example OLED-Demo.ino
   @brief An extensive example demonstrating the use of DisplayElement%s
          to display information from the DAW on a small OLED display.

   The example displays the cursor time location, play and record status,
   and for each of the 8 first tracks, it displays:
   mute, solo, record ready, pan, and VU level meter with peak indicator.

   Using a BankSelector, it displays two channels at once, and you can
   cycle through four banks to display all 8 tracks.

   There are two rotary encoders that control the pan of the selected
   tracks, two mute, solo, record, and select buttons for the tracks,
   and a play and record button.
*/

#include <Encoder.h> // Include the Encoder library.
// This must be done before the Control Surface library.

#include <Control_Surface.h> // Include the Control Surface library

#include <Adafruit_SSD1306.h> // Include the library for your specific display
#include <Wire.h>             // Include the I²C library for the display

// ----------------------------- MIDI Interface ----------------------------- //
// ========================================================================== //

/*
   Instantiate a MIDI interface to use for the Control Surface.
*/

USBMIDI_Interface midi;
// USBDebugMIDI_Interface midi(115200);

// ----------------------------- Display setup ------------------------------ //
// ========================================================================== //

/*
   Instantiate and initialize the SSD1306 OLED display
*/

Adafruit_SSD1306 ssd1306Display; // Instantiate a display

void initializeDisplay() {
  // Initialize with the display with I²C address 0x3C
  ssd1306Display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Wire.setClock(1800000); // Set the I²C clock to 1.8 MHz for faster refresh
  ssd1306Display.setRotation(2); // Flip the display upside down
}

// --------------------------- Display interface ---------------------------- //
// ========================================================================== //

/*
   This creates a mapping between your specific display driver and the
   general display interface used by the Control Surface library.
   It is roughly based on the Adafruit_GFX API.
*/

class SSD1306_DisplayInterface : public DisplayInterface {
  public:
    SSD1306_DisplayInterface(Adafruit_SSD1306 &display) : disp(display) {}

    void clear() override {
      disp.clearDisplay();
    }
    void drawBackground() override {
      disp.drawLine(1, 8, 126, 8, WHITE);
    }
    void display() override {
      disp.display();
    }

    void drawPixel(int16_t x, int16_t y, uint16_t color) override {
      disp.drawPixel(x, y, color);
    }

    void setTextColor(uint16_t color) override {
      disp.setTextColor(color);
    }
    void setTextSize(uint8_t size) override {
      disp.setTextSize(size);
    }
    void setCursor(int16_t x, int16_t y) override {
      disp.setCursor(x, y);
    }

    size_t write(uint8_t c) override {
      return disp.write(c);
    }

    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                  uint16_t color) override {
      disp.drawLine(x0, y0, x1, y1, color);
    }
    void drawFastVLine(int16_t x, int16_t y, int16_t h,
                       uint16_t color) override {
      disp.drawFastVLine(x, y, h, color);
    }
    void drawFastHLine(int16_t x, int16_t y, int16_t w,
                       uint16_t color) override {
      disp.drawFastHLine(x, y, w, color);
    }

    void drawXBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w,
                     int16_t h, uint16_t color) override {
      disp.drawXBitmap(x, y, bitmap, w, h, color);
    }

  private:
    Adafruit_SSD1306 &disp;
};

// Instantiate a display interface
SSD1306_DisplayInterface display = ssd1306Display;

// ------------------------------- Bank setup ------------------------------- //
// ========================================================================== //

/*
   Create a bank and a bank selector to change its setting.
*/

Bank bank(2); // Create a new bank with two tracks per bank

// Create a new bank selector with four bank settings and an increment button
// on pin A2
IncrementDecrementSelector bankselector(bank, 4, {8, 9});

// -------------------------- MIDI Output Elements -------------------------- //
// ========================================================================== //

/*
   Define all hardware that send out MIDI messages when changed.
*/

constexpr Channel channel = 1;

PBPotentiometer masterVolume = { A1, 9 };

// Instantiate the buttons
Bankable::NoteButton channelButtons[] = {
  {bank, 4, MCU::V_POT_SELECT_1, channel},
  {bank, 5, MCU::MUTE_1, channel},
  {bank, 6, MCU::SOLO_1, channel},
  
  // {bank, 7, MCU::REC_RDY_1, channel},
  // { bank, 12, MCU::V_POT_SELECT_2, channel },
  // { bank, 10, MCU::MUTE_2,         channel },
  // { bank, 11, MCU::SOLO_2,         channel },
};

NoteButton playButton = {7, MCU::PLAY, channel};
// NoteButton recordButton = {15, MCU::RECORD, channel };

// Bankable::CCRotaryEncoder encoder_A = { bank, {0, 1}, MCU::V_POT_1, channel, 1, 4 };
Bankable::CCRotaryEncoder encoder_B = {bank,    {2, 3}, MCU::V_POT_1,
                                       channel, 1,      4
                                      };

// -------------------------- MIDI Input Elements --------------------------- //
// ========================================================================== //

/*
   Define all elements that listen for MIDI messages.
*/

// Time display keeps track of the bar counter
MCU::TimeDisplay timedisplay = {channel};

// Play / Record
MIDINote play = {MCU::PLAY, channel};
MIDINote record = {MCU::RECORD, channel};

// Mute
Bankable::MIDINote<4> mute_A = {bank, MCU::MUTE_1, channel};
Bankable::MIDINote<4> mute_B = {bank, MCU::MUTE_2, channel};

// Solo
Bankable::MIDINote<4> solo_A = {bank, MCU::SOLO_1, channel};
Bankable::MIDINote<4> solo_B = {bank, MCU::SOLO_2, channel};

MIDINote rudeSolo = {MCU::RUDE_SOLO, channel};

// Record arm / ready
Bankable::MIDINote<4> recrdy_A = {bank, MCU::REC_RDY_1, channel};
Bankable::MIDINote<4> recrdy_B = {bank, MCU::REC_RDY_2, channel};

// VU meters
MCU::Bankable::VU<4> vu_A = {bank, 1, channel, 0};
MCU::Bankable::VU<4> vu_B = {bank, 2, channel, 0};

// VPot rings
MCU::Bankable::VPotRing<4> vpot_A = {bank, 1, channel};
MCU::Bankable::VPotRing<4> vpot_B = {bank, 2, channel};

// ---------------------------- Display Elements ---------------------------- //
// ========================================================================== //

/*
   Define all display elements that display the state of the input elements.
*/

// Time display
MCU::TimeDisplayDisplay timedisplaydisplay = {
  display, timedisplay, {0, 0}, 1, WHITE
};
// position (0, 0), font size (1)

// Play / Record
NoteBitmapDisplay playDisp = {display, play, XBM::play_7, {16 + 64, 0}, WHITE};
NoteBitmapDisplay recordDisp = {
  display, record, XBM::record_7, {26 + 64, 0}, WHITE
};

// Mute
NoteBitmapDisplay muteDisp_A = {
  display, mute_A, XBM::mute_10B, {14, 50}, WHITE
};
NoteBitmapDisplay muteDisp_B = {
  display, mute_B, XBM::mute_10B, {14 + 64, 50}, WHITE
};

// Solo
NoteBitmapDisplay soloDisp_A = {
  display, solo_A, XBM::solo_10B, {14, 50}, WHITE
};
NoteBitmapDisplay soloDisp_B = {
  display, solo_B, XBM::solo_10B, {14 + 64, 50}, WHITE
};

NoteBitmapDisplay rudeSoloDisp = {
  display, rudeSolo, XBM::solo_7, {36 + 64, 0}, WHITE
};

// Record arm / ready
NoteBitmapDisplay recrdyDisp_A = {
  display, recrdy_A, XBM::rec_rdy_10B, {14 + 14, 50}, WHITE
};
NoteBitmapDisplay recrdyDisp_B = {
  display, recrdy_B, XBM::rec_rdy_10B, {14 + 14 + 64, 50}, WHITE
};

// VU meters
MCU::VUDisplay vuDisp_A = {display, vu_A, {32 + 11, 60}, 16, 3, 1, WHITE};
// position (32+11, 60), width (16), bar height (3) px, bar spacing (1) px
MCU::VUDisplay vuDisp_B = {display, vu_B, {32 + 11 + 64, 60}, 16, 3, 1, WHITE};

// VPot rings
MCU::VPotDisplay vpotDisp_A = {display, vpot_A, {0, 10}, 16, 13, WHITE};
// position (0, 10), outer radius (16) px, inner radius (13) px
MCU::VPotDisplay vpotDisp_B = {display, vpot_B, {64, 10}, 16, 13, WHITE};

// Bank selector
SelectorDisplay bsDisp_A = {display, bankselector, 1, 2, {0, 50}, 2, WHITE};
// first track of the bank (1), two tracks per bank (2), position (0, 50), font size (2)
SelectorDisplay bsDisp_B = {display, bankselector, 2, 2, {64, 50}, 2, WHITE};

// --------------------------------- Setup ---------------------------------- //
// ========================================================================== //

void setup() {
  RelativeCCSender::setMode(MACKIE_CONTROL_RELATIVE); // Correct mode for
  // MCU rotary encoders
  initializeDisplay();     // Start the OLED display
  Serial.begin(115200);
  // while (!Serial);
  // delay(250);
  Control_Surface.begin(); // Initialize Control Surface
}

// ---------------------------------- Loop ---------------------------------- //
// ========================================================================== //

void loop() {
  Control_Surface.loop(); // Refresh all elements
}
