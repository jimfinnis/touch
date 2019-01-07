#include "screen.h"

Screen s;

void setup(){
  Serial.begin(57600);

  s.init();
  s.registerButton(new Button(1,"ALERT",0,0,100,50,0xffff00));
  s.registerButton(new Button(2,"NOT ALERT",105,0,100,50,0x00ff00));
}

void loop(){
  s.draw();
  delay(5);

  if(Button *b = s.poll()){
    Serial.print("Pressed ");Serial.println(b->id);
  }
}
