#include "screen.h"

Screen s;

void setup(){
  Serial.begin(57600);

  s.init();
  s.registerButton(new Button(0,"ARM",0xff0000,NEWROW))->setLatching();
  s.registerButton(new Button(1,"A",0xffff00,NEWROW,SCALE,3,2))->setLatching();
  s.registerButton(new Button(2,"B",0x00ff00));
  s.registerButton(new Button(1,"C",0xffff00));
  s.registerButton(new Button(2,"NOT ALERT",0x00ff00,NEWROW));
  s.registerButton(new Button(1,"ALERT",0xffff00));
  s.registerButton(new Button(2,"NOT ALERT",0x00ff00,NEWROW));
  s.registerButton(new Button(1,"ALERT",0xffff00));
  s.registerButton(new Button(2,"NOT ALERT",0x00ff00,NEWROW));
  s.fixup();
}

void loop(){
  s.draw();
  delay(5);

  if(Button *b = s.poll()){
    Serial.print("Pressed ");Serial.println(b->id);
  }
}
