#include <Keyboard.h>
#include "screen.h"

Screen scrInit;

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

Screen scrElite;
void setupElite(){
  scrElite.registerButton(new ArmButton(0,"ARM",0xff0000,NEWROW))->setLatching();
  
  scrElite.registerButton(new KeyButton(1,"MODE","m",0x00ff00,NEWROW));
  scrElite.registerButton(new KeyButton(2,"NGHT","\\",0x00ff00));
  scrElite.registerButton(new SpecialButton(3,"CARGO",KEY_HOME,0x00ff00));

  scrElite.registerButton(new KeyButton(4,"JETT","#",0xff0000,NEWROW))->setConfirm();
  scrElite.registerButton(new SpecialButton(5,"SILENT",KEY_DELETE,0x0000ff));

  scrElite.registerButton(new KeyButton(6,"CELL","'",0xffff00,NEWROW));
  scrElite.registerButton(new KeyButton(7,"CHFF","/",0xff00ff));
  scrElite.registerButton(new KeyButton(7,"HEAT","v",0xff0000));

  scrElite.registerButton(new KeyButton(9,"GRP","N",0xff0000,NEWROW));
  scrElite.registerButton(new KeyButton(9,"GEAR","L",0x00ff00));
  scrElite.registerButton(new KeyButton(9,"DPLY","u",0xff0000));
  
  scrElite.fixup();
}

Screen scrNMS;
void setupNMS(){
  scrNMS.registerButton(new ArmButton(0,"ARM",0xff0000,NEWROW))->setLatching();
  scrNMS.registerButton(new KeyButton(1,"PHOTO","9",0x00ff00,NEWROW));

  scrNMS.fixup();

}

void setup(){
  Serial.begin(57600);
//  Keyboard.begin();
  ScreenSystem.init();

  // set up the screens

  setupElite();
  setupNMS();

  scrInit.registerButton(new ScreenChangeButton(0,"ELITE",0xff0000,&scrElite,NEWROW))->setConfirm();
  scrInit.registerButton(new ScreenChangeButton(0,"NMS",0x00ff00,&scrNMS,NEWROW))->setConfirm();
  scrInit.fixup();
  ScreenSystem.go(&scrInit);

  
}

void loop(){
  ScreenSystem.draw();
  delay(5);

  if(Button *b = ScreenSystem.poll()){
    Serial.print("Pressed ");Serial.println(b->id);
  }
}
