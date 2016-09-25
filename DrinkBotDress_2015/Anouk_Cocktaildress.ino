#include <Adafruit_NeoPixel.h>
#include "SimpleTimer.h"

#define DISP_BTN 1
#define DBG_LED 13
#define NUM_LEDS 12
#define NUM_ANIM_PHASES 35
#define TIME_ANIM_FAST 20
#define TIME_ANIM_SLOW 80

bool dbg_led_tg = false;
bool p_btn_state = false;
uint8_t anim_pid = 0;

//animations
uint8_t phase_n = 0;
//const PROGMEM uint8_t r_n[NUM_ANIM_PHASES] = {9,9,9,9,9,9,19,28,38,47,57,66,76,85,95,104,114,123,133,142,152,161,171,180,190,190,171,152,133,114,95,76,57,38,19};
//const PROGMEM uint8_t g_n[NUM_ANIM_PHASES] = {1,1,1,1,1,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,20,18,16,14,12,10,8,6,4,2};
//const PROGMEM uint8_t b_n[NUM_ANIM_PHASES] = {3,3,3,3,3,3,6,9,12,15,18,21,24,27,30,33,36,39,42,45,48,51,54,57,60,60,54,48,42,36,30,24,18,12,6};

const PROGMEM uint8_t r_n[NUM_ANIM_PHASES] = {12,12,12,12,12,12,25,38,51,63,76,89,102,114,127,140,153,165,178,191,204,216,229,242,255,255,229,204,178,153,127,102,76,51,25};
const PROGMEM uint8_t g_n[NUM_ANIM_PHASES] = {12,12,12,12,12,12,25,38,51,63,76,89,102,114,127,140,153,165,178,191,204,216,229,242,255,255,229,204,178,153,127,102,76,51,25};
const PROGMEM uint8_t b_n[NUM_ANIM_PHASES] = {12,12,12,12,12,12,25,38,51,63,76,89,102,114,127,140,153,165,178,191,204,216,229,242,255,255,229,204,178,153,127,102,76,51,25};

/** 
TO PINK, LOG
9,19,28,38,47,57,66,76,85,95,104,114,123,133,142,152,161,171,180,190
1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
3,6,9,12,15,18,21,24,27,30,33,36,39,42,45,48,51,54,57,60
FROM PINK, LOG
190,171,152,133,114,95,76,57,38,19
20,18,16,14,12,10,8,6,4,2
60,54,48,42,36,30,24,18,12,6

TO WHITE, LOG
12,25,38,51,63,76,89,102,114,127,140,153,165,178,191,204,216,229,242,255
FROM WHITE, LOG
255,229,204,178,153,127,102,76,51,25
*/



SimpleTimer timer;
Adafruit_NeoPixel led_ring = Adafruit_NeoPixel(NUM_LEDS, 20, NEO_GRB + NEO_KHZ800);

void dispense(uint16_t ml){
  //uint16_t ticks = int(ml*2.78F);
  //Serial.print("Dispensing: ");
  //Serial.print(ticks);
  //Serial.println(" ticks");
  
  timer.change_delay(anim_pid, TIME_ANIM_FAST);
  timer.setTimer(3000, anim_change_time, SimpleTimer::RUN_ONCE);
  
  //header
  Serial2.write("tickdisp 24 250"); 
  Serial2.write(0x0D);
}

void anim_change_time(uint8_t pid){
  timer.change_delay(anim_pid, TIME_ANIM_SLOW);
}

void anim_led(uint8_t pid){
  led_ring_color(r_n[phase_n], g_n[phase_n], b_n[phase_n]);
  
  phase_n++;
  phase_n %= NUM_ANIM_PHASES;
}

void setup() {
  pinMode(DISP_BTN, INPUT_PULLUP);
  pinMode(DBG_LED, OUTPUT);
  digitalWrite(DBG_LED, HIGH);
  
  led_ring.begin();
  led_ring_color(0,0,0);
  
  delay(2000);
  Serial.begin(9600);
  Serial2.begin(9600);
  
  //textmode...
  Serial2.write("!!!"); 
  Serial2.write(0x0D);

  digitalWrite(DBG_LED, LOW);
  
  anim_pid = timer.setTimer(TIME_ANIM_SLOW, anim_led, SimpleTimer::RUN_FOREVER);
}

void led_ring_color(uint8_t r, uint8_t g, uint8_t b){
 for(int i=0; i < NUM_LEDS; i++){
    led_ring.setPixelColor(i, led_ring.Color(r,g,b));
  }
  led_ring.show();
}

void loop() {
  timer.run();
  
  bool button_state = digitalRead(DISP_BTN);
  if(digitalRead(DISP_BTN) != p_btn_state){
    //button change
    p_btn_state = button_state;
    
    if(button_state){
      //button down
      digitalWrite(DBG_LED, LOW);
    } else {
      //button up
      Serial.println("dispense...");
      digitalWrite(DBG_LED, HIGH);
      dispense(30);
    }
  }

}
