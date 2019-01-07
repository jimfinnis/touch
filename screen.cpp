/**
 * @file screen.cpp
 * @brief  Brief description of file.
 *
 */

#include <TouchScreen.h>
#include <TFTv2.h>

#include "screen.h"

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // mega
#define YP A2   // must be an analog pin, use "An" notation!
#define XM A1   // must be an analog pin, use "An" notation!
#define YM 54   // can be a digital pin, this is A0
#define XP 57   // can be a digital pin, this is A3 

#elif defined(__AVR_ATmega32U4__) // leonardo
#define YP A2   // must be an analog pin, use "An" notation!
#define XM A1   // must be an analog pin, use "An" notation!
#define YM 18   // can be a digital pin, this is A0
#define XP 21   // can be a digital pin, this is A3 

#else //168, 328, something else
#define YP A2   // must be an analog pin, use "An" notation!
#define XM A1   // must be an analog pin, use "An" notation!
#define YM 14   // can be a digital pin, this is A0
#define XP 17   // can be a digital pin, this is A3 

#endif

//Measured ADC values for (0,0) and (210-1,320-1)
//TS_MINX corresponds to ADC value when X = 0
//TS_MINY corresponds to ADC value when Y = 0
//TS_MAXX corresponds to ADC value when X = 240 -1
//TS_MAXY corresponds to ADC value when Y = 320 -1

#define TS_MINX 116*2
#define TS_MAXX 890*2
#define TS_MINY 83*2
#define TS_MAXY 913*2

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// The 2.8" TFT Touch shield has 300 ohms across the X plate
static TouchScreen ts = TouchScreen(XP, YP, XM, YM);

#define PRESSURETHRESHOLD 1



Screen::Screen(){
    nbuttons = 0;
}

void Screen::init(){
    TFT_BL_OFF;
    Tft.TFTinit();  
}

void Screen::draw(){
    for(int i=0;i<nbuttons;i++){
        buttons[i]->updateAndDraw();
    }
}

Button *Screen::poll(){
    // is there a button press?
    
    Point p = ts.getPoint();

    if (p.z > PRESSURETHRESHOLD) {
//        Serial.print("Raw X = "); Serial.print(p.x);
//        Serial.print("\tRaw Y = "); Serial.print(p.y);
//        Serial.print("\tPressure = "); Serial.println(p.z);
        int x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
        int y = map(p.y, TS_MINY, TS_MAXY, 0, 320);
        for(int i=0;i<nbuttons;i++){
            if(buttons[i]->isPressed(x,y)){
                buttons[i]->markPressed();
                return buttons[i];
            }
        }
    }
    return NULL;
}

    
void Button::updateAndDraw(){
    static const int FONTSIZEX=6;
    static const int FONTSIZEY=8;
    static const int FONTSCALE=2;
    

    if(presscount){
        if(!--presscount)dirty=true;
    }
    if(dirty){
        dirty = false;
        Serial.print("Drawing button ");Serial.print(text);Serial.print(" at ");
        Serial.print(x);Serial.print(",");Serial.print(y);Serial.print(" with col ");Serial.println(col);
        if(presscount){
            Tft.fillRectangle(x,y,w,h,col);
        } else {
            Tft.fillRectangle(x,y,w,h,0);
            Tft.drawRectangle(x,y,w,h,col);
            int ymid = y+h/2;
            int xmid = x+w/2;
            Tft.drawString((char*)(text),
                           xmid-strlen(text)*FONTSIZEX*FONTSCALE/2,
                           ymid-FONTSIZEY*FONTSCALE/2,FONTSCALE,col);
        }
    }
}

    
