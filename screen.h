/**
 * @file screen.h
 * @brief  Brief description of file.
 *
 */

#ifndef __SCREEN_H
#define __SCREEN_H

#define MAXBUTTONS 20
#define PRESSLENGTH 70
#define CONFIRMLENGTH 500

#define NEWROW 0
#define SCALE -2

// convert from 888 to 565
inline uint16_t col24to16(uint32_t rgb){
    uint16_t b = rgb & 255;
    uint16_t g = (rgb >> 8)&255;
    uint16_t p = (rgb >> 16)&255;
    p >>= 3;p <<= 6;g >>= 2;
    p |= g;p <<= 5;b >>= 3;
    p |= b;
    return p;
}

// a basic button that does onPress when pressed. Use setLatching to make it latching rather than
// momentary, and setConfirm to make it require two presses.

class Button {
    friend class Screen;
    const char *text;
    int x,y; // topleft
    int w,h;
    uint16_t col; // colour, converted from 888 to 565
    bool dirty; // requires redraw!
    int presscount; // how many frames I have been "pressed down" for, graphically.
    int confirmcount; // how many frames a confirm has left
    bool latching; // button is latching rather than momentary
    bool confirm; // button is a momentary with confirm
    
    // used to check button pressage
    bool isPressed(int px,int py){
        if(presscount)return false; // avoid repress.
        px -= x; py-=y;
        return px>=0 && px<w && py>=0 && py<h;
    }
    virtual void markPressed(){
        presscount=PRESSLENGTH;
        dirty=true;
        if(latching)isdown=!isdown;
        if(confirm){
            if(confirmcount){
                onPress();
                confirmcount=0;
            } else  {
                confirmcount=CONFIRMLENGTH;
            }
        } else {
            onPress();
        }
    }

public:    
    int id;
    bool isdown; // applies only to latching buttons
    
    // Can be used in these ways:
    // (id,txt,c,  x,y,w,h) - manual positioning
    // (id,txt,c,  NEWROW) - auto positioning, start new row - first button must do this
    // (id,txt,c)       - auto positioning, add to current row
    // (id,txt,c,  NEWROW,SCALE,3,2) - auto positioning, start new row of given height scale  (3/2
    
    
    Button(int _id,const char *_t,uint32_t _c,int _x=-1,int _y=-1,int _w=-1,int _h=-1){
        id=_id;
        x=_x;
        y=_y;
        w=_w;
        h=_h;
        text=_t;
        col = col24to16(_c);
        presscount = 0;
        latching = false;
        isdown = false;
        dirty=true;
        confirm=false;
        confirmcount=0;
    }
    
    Button *setLatching(){
        latching=true;
        confirm=false;
        return this;
    }
    Button *setConfirm(){
        latching=false;
        confirm=true;
        return this;
    }
    
    void updateAndDraw();
    
    // override if you want to. You might not want to for latching buttons,
    // just read their isdown member.
    virtual void onPress(){}
};

// a button which, when pressed, changes to another Screen.

class ScreenChangeButton : public Button {
private:
    class Screen *screen;
public:  
    ScreenChangeButton(int _id,const char *_t,uint32_t _c,class Screen *s,int _x=-1,int _y=-1,int _w=-1,int _h=-1) : 
      Button(_id,_t,_c,_x,_y,_w,_h){
      screen = s;
  }

  virtual void onPress();
};

// a screen within the system; create some of these and set the 
// primary screen using ScreenSystem.go().

class Screen {
    friend class ScreenSystemClass;
    Button *buttons[20];
    int nbuttons;
    bool screenDirty;
public:    
    Screen();
    
    Button *registerButton(Button *b){
        if(nbuttons<MAXBUTTONS){
            buttons[nbuttons++]=b;
        }
        return b;
    }

    void forceRedraw(){
      screenDirty=true;
    }
    
    // fix up buttons with unassigned positions
    void fixup();
    
    void draw();
    Button *poll();
};

// a set of screens; the instance is ScreenSystem.

class ScreenSystemClass {
private:  
  Screen *curscreen;
public:
  ScreenSystem(Screen *initial){
    curscreen = initial;
  }

  // you must call this!
  void init();


  void go(Screen *s){
    Serial.print("going to screen ");
    Serial.println(s->nbuttons);
    curscreen = s;
  }

  void draw(){
    curscreen->draw();
  }

  Button *poll(){
    return curscreen->poll();
  }
};

extern ScreenSystemClass ScreenSystem; // the singleton

#endif /* __SCREEN_H */
