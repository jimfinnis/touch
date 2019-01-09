#include <Keyboard.h>
#include "screen.h"

Screen s;

static bool keyhandlingOn = false;

class ArmButton : public Button {
public:
  ArmButton(int id,const char *txt,uint32_t col,int x=-1,int y=-1,int w=-1,int h=-1) :
  Button(id,txt,col,x,y,w,h){}

  virtual void onPress(){
    keyhandlingOn = isdown;
  }
};

class KeyButton : public Button {
  const char *strokes;
public:
  KeyButton(int id,const char *txt,const char *keystrokes,uint32_t col,int x=-1,int y=-1,int w=-1,int h=-1) : Button(id,txt,col,x,y,w,h) {
    strokes = keystrokes;
  }
  virtual void onPress(){
    if(keyhandlingOn)
    Keyboard.print(strokes);
  }
};

void setup(){
  Serial.begin(57600);
  Keyboard.begin();

  s.init();
  s.registerButton(new ArmButton(0,"ARM",0xff0000,NEWROW))->setLatching();
  s.registerButton(new KeyButton(1,"A","foo",0xffff00,NEWROW,SCALE,3,2));
  s.registerButton(new KeyButton(2,"B","bar",0x00ff00));
  s.registerButton(new KeyButton(3,"C","baz",0xffff00));
  s.fixup();
}

void loop(){
  s.draw();
  delay(5);

  if(Button *b = s.poll()){
    Serial.print("Pressed ");Serial.println(b->id);
  }
}
