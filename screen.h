/**
 * @file screen.h
 * @brief  Brief description of file.
 *
 */

#ifndef __SCREEN_H
#define __SCREEN_H

#define MAXBUTTONS 20
#define PRESSLENGTH 100

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


class Button {
    friend class Screen;
    const char *text;
    int x,y; // topleft
    int w,h;
    uint16_t col; // colour, converted from 888 to 565
    bool dirty; // requires redraw!
    int presscount; // how many frames I have been "pressed down" for, graphically.
    bool latching; // button is latching rather than momentary
    
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
        onPress();
    }

public:    
    int id;
    bool isdown; // applies only to latching buttons
    
    // Can be used in these ways:
    // (id,txt,c,  x,y,w,h) - manual positioning
    // (id,txt,c,  NEWROW) - auto positioning, start new row - first button must do this
    // (id,txt,c,  NEWROW,SCALE,3,2) - auto positioning, start new row of given height scale  (3/2
    
    // (id,txt,c)       - auto positioning, add to current row
    
    Button(int _id,const char *_t,uint32_t _c,int _x=-1,int _y=-1,int _w=-1,int _h=-1){
        id=_id;
        x=_x;
        y=_y;
        w=_w;
        h=_h;
        text=_t;
        dirty=true;
        col = col24to16(_c);
        presscount = 0;
        latching = false;
        isdown = false;
    }
    
    Button *setLatching(){
        latching=true;
        return this;
    }
    
    void updateAndDraw();
    
    // override if you want to.
    virtual void onPress(){}
    
};

class Screen {
    Button *buttons[20];
    int nbuttons;
public:    
    Screen();
    void init();
    
    Button *registerButton(Button *b){
        if(nbuttons<MAXBUTTONS){
            buttons[nbuttons++]=b;
        }
        return b;
    }
    
    // fix up buttons with unassigned positions
    void fixup();
    
    void draw();
    Button *poll();
};

#endif /* __SCREEN_H */
