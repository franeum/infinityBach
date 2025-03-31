#define BUTTON_INIT   2
#define BUTTON_PLAY   3
#define BUTTON_CHANGE 4

#include "MIDIUSB.h"
#include "Button2.h"

Button2 b_init, b_play, b_change; 

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
  MidiUSB.flush();
}

void released(Button2& btn) {
    byte value;
    
    if (btn == b_init) {
      value = 48;  
    } else if (btn == b_play) {
      value = 72;
    } else if (btn == b_change) {
      value = 60;
    }
    noteOn(0, value, 80);
}

void setup() {
  Serial.begin(115200);
  b_init.begin(BUTTON_INIT);
  b_init.setReleasedHandler(released);

  b_play.begin(BUTTON_PLAY);
  b_play.setReleasedHandler(released);

  b_change.begin(BUTTON_CHANGE);
  b_change.setReleasedHandler(released);
}

void loop() {
  b_init.loop();
  b_play.loop();
  b_change.loop();
}
