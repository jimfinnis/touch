/**
 * @file screen.h
 * @brief  Brief description of file.
 *
 */

#ifndef __SCREEN_H
#define __SCREEN_H

#define MAXBUTTONS 20
#define PRESSLENGTH 100


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
    const char *text;
    int x,y; // topleft
    int w,h;
    uint16_t col; // colour, converted from 888 to 565
    bool dirty; // requires redraw!
    int presscount; // how many frames I have been "pressed down" for, graphically.
public:    
    int id;
    
    Button(int _id,const char *_t,int _x,int _y,int _w,int _h,uint32_t _c){
        id=_id;
        x=_x;
        y=_y;
        w=_w;
        h=_h;
        text=_t;
        dirty=true;
        col = col24to16(_c);
        presscount = 0;
    }
    void updateAndDraw();
    
    virtual void markPressed(){
        presscount=PRESSLENGTH;
        dirty=true;
    }
    bool isPressed(int px,int py){
        if(presscount)return false; // avoid repress.
        px -= x; py-=y;
        return px>=0 && px<w && py>=0 && py<h;
    }
};

class Screen {
    Button *buttons[20];
    int nbuttons;
public:    
    Screen();
    void init();
        
    void registerButton(Button *b){
        if(nbuttons<MAXBUTTONS)
            buttons[nbuttons++]=b;
    }
    
    void draw();
    Button *poll();
};

#endif /* __SCREEN_H */
