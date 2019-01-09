/**
 * @file screen.cpp
 * @brief  Brief description of file.
 *
 */

#include "TouchScreen.h"
#include "TFTv2.h"

#include "screen.h"

#define SCREENWIDTH 240

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

static const int FONTSIZEX=6;
static const int FONTSIZEY=8;
static const int FONTSCALE=2;
static const int BUTTONHEIGHT = 50;
static const int BUTTONMARGIN = 4; // margin around button text in X
static const int BUTTONROWSEP = 10;
static const int BUTTONPADDING = 10; // padding around button in X

//static const int BMINWIDTH = 40; // button min width for old method of width

void panic(int n,const char *t){
    Tft.fillRectangle(0,0,240,320,0);
    Tft.drawString("Fatal error",0,0,3,WHITE);
    Tft.drawNumber(n,0,100,4,RED);
    
    pinMode(13,OUTPUT);
    for(;;){
        for(int i=0;i<n;i++){
            digitalWrite(13,HIGH);
            delay(300);
            digitalWrite(13,LOW);
            delay(300);
        }
        delay(700);
    }
    Serial.println("Fatal error ");Serial.print(n);
    Serial.print(": ");Serial.println(t);
}
    
        

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

void Screen::fixup(){
    
    uint8_t nrows=0;
    uint8_t rowcounts[10];
    float rowscales[10];
    
    // first get row lengths
    for(int i=0;i<nbuttons;i++){
        Button *b = buttons[i];
        if(b->y < 0){
            if(b->x == NEWROW){
                rowcounts[nrows]=0;
                float sc = 1;
                if(b->y == SCALE){
                    sc = b->w;
                    sc = sc / (float)b->h;
                }
                rowscales[nrows]=sc;
                nrows++;
            }
            if(nrows==0){
                panic(2,"must use NEWROW in first row");
            }
                
            rowcounts[nrows-1]++;
        }
    }
    
    // now position on the screen
    int curbut=0;
    int sy = 0;
    for(int i=0;i<nrows;i++){
        int rowct = rowcounts[i];
        int slotwidth = SCREENWIDTH/rowct;
//        Serial.print("Row : ");Serial.println(rowct);
        // consume N buttons
        for(int j=0;j<rowct;j++){
            Button *b = buttons[curbut++];
            // centre of button
            int x = slotwidth*j; x+=slotwidth>>1;
            // actual button width
            /* this code sets the margin narrowly around the text
               int w = strlen(b->text)*FONTSIZEX*FONTSCALE+BUTTONMARGIN;
               if(w<BMINWIDTH)w=BMINWIDTH;
             */
            int w = slotwidth - BUTTONPADDING;
            // left edge of button
            x -= w/2;
            b->x = x;
            b->y = sy;
            b->w = w;
            b->h = BUTTONHEIGHT*rowscales[i];
        }
        sy+=BUTTONHEIGHT*rowscales[i]+BUTTONROWSEP;
        
    }
    
}
/*
void Screen::registerButton(int id,const char *txt, uint32_t col){
    
    
    
}
*/
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

    if(presscount){
        if(!--presscount)
            dirty=true;
    }
    if(confirmcount){
        if(!--confirmcount)
            dirty=true;
    }
    if(dirty && y>=0){ // only redraw dirty, fixed up buttons
        dirty = false;
//        Serial.print("Drawing button ");Serial.print(text);Serial.print(" at ");
//        Serial.print(x);Serial.print(",");Serial.print(y);Serial.print(" with col ");Serial.println(col);
        if(presscount){
            Tft.fillRectangle(x,y,w,h,col);
        } else {
            int xmid = x+w/2;
            int ymid = y+h/2;
            
            if(latching && isdown){
                Tft.fillRectangle(x,y,w,h,col);
                Tft.drawString((char*)(text),
                               xmid-strlen(text)*FONTSIZEX*FONTSCALE/2,
                               ymid-FONTSIZEY*FONTSCALE/2,FONTSCALE,0);
            } else if(confirm && confirmcount) {
                Tft.fillRectangle(x,y,w,h,WHITE);
                Tft.fillRectangle(x+5,y+5,w-10,h-10,BLACK);
                Tft.drawString((char*)(text),
                               xmid-strlen(text)*FONTSIZEX*FONTSCALE/2,
                               ymid-FONTSIZEY*FONTSCALE/2,FONTSCALE,col);
            } else {
                Tft.fillRectangle(x,y,w,h,0);
                Tft.drawRectangle(x,y,w,h,col);
                Tft.drawString((char*)(text),
                               xmid-strlen(text)*FONTSIZEX*FONTSCALE/2,
                               ymid-FONTSIZEY*FONTSCALE/2,FONTSCALE,col);
            }                
        }
    }
}

    
