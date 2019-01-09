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

class SpecialButton: public Button {
  char key;
public:
  SpecialButton(int id,const char *txt,char _key,uint32_t col,int x=-1,int y=-1,int w=-1,int h=-1) :
  Button(id,txt,col,x,y,w,h){key=_key;}
  virtual void onPress(){
    if(keyhandlingOn)
      Keyboard.write(key);
  }
};

void setup(){
  Serial.begin(57600);
  Keyboard.begin();

  s.init();
  s.registerButton(new ArmButton(0,"ARM",0xff0000,NEWROW))->setLatching();
  
  s.registerButton(new KeyButton(1,"MODE","m",0x00ff00,NEWROW));
  s.registerButton(new KeyButton(2,"NGHT","\\",0x00ff00));
  s.registerButton(new KeyButton(3,"CRSC","`",0x00ff00));

  s.registerButton(new KeyButton(4,"JETT","#",0xff0000,NEWROW))->setConfirm();
  s.registerButton(new SpecialButton(5,"SLNT",KEY_DELETE,0x0000ff));

  s.registerButton(new KeyButton(6,"CELL","'",0xffff00,NEWROW));
  s.registerButton(new KeyButton(7,"CHFF","/",0xff00ff));
  s.registerButton(new KeyButton(7,"HEAT","/",0xff0000));

  s.registerButton(new KeyButton(9,"GRP","N",0xff0000,NEWROW));
  s.registerButton(new KeyButton(9,"GEAR","L",0x00ff00));
  s.registerButton(new KeyButton(9,"DPLY","L",0xff0000));
  
  s.fixup();
}

void loop(){
  s.draw();
  delay(5);

  if(Button *b = s.poll()){
    Serial.print("Pressed ");Serial.println(b->id);
  }
}
