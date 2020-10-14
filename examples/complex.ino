#include <arduino.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <Encoder.h>
Encoder myEnc(0, 1);

#include <Debouncer.h>
Debouncer debouncer(4, 50);

#include "fxLcdMenu.h"

fxMenuNav* nav = new fxMenuNav(lcd);


fxMenuList* submenu = new fxMenuList("Demo Submenu");
fxMenuItem s1("S1");
bool booltest = false;
fxMenuItemBool s2("S2", &booltest);
fxMenuItemBack back;

fxMenuList* mainmenu = new fxMenuList();

fxMenuItem second("Disabled item");
fxMenuItem third ("Item #3");
fxMenuItem fourth("Item #4");

fxMenuList* options = new fxMenuList("Options");
bool bglight = true;
fxMenuItemBool light("Light", &bglight);
int brightness = 60;
fxMenuItemInt bright("Value", &brightness);
bool anim = false;
fxMenuItemBool mAnim("Anims", &anim);


void setup() {
  Serial.begin(57600);
  delay(2000);
  lcd.init();
  lcd.backlight();

  pinMode(4, INPUT_PULLUP);
  pinMode(10, OUTPUT);
  analogWrite(10, brightness);

  myEnc.write(2); // move transition border betweem two clicks

  byte arrow1[8] = { 0b10000, 0b11000, 0b11100, 0b11110, 0b11100, 0b11000, 0b10000 };
  lcd.createChar(0, arrow1);
  byte arrow2[8] = { 0b10000, 0b11000, 0b10100, 0b10010, 0b10100, 0b11000, 0b10000 };
  lcd.createChar(1, arrow2);
  byte arrow3[8] = { 0b00001, 0b00011, 0b00111, 0b01111, 0b00111, 0b00011, 0b00001 };
  lcd.createChar(2, arrow3);
  
  second.setEnable(false);
  
  mainmenu->add(submenu)->add(&second)->add(&third)->add(&fourth)->add(options);
//  submenu->add(&s1)->add(&s2)->add(&back);
  submenu->add(&back);
  options->add(&light)->add(&bright)->add(&mAnim)->add(&back);
  
  nav->setMenu(mainmenu);

  debouncer.subscribe(Debouncer::Edge::FALL, [](){
    nav->select();
  });

  light.subscribe([](){
     bglight ? lcd.backlight() : lcd.noBacklight();
     bright.setEnable(bglight);
  });

  mAnim.subscribe([](){
    nav->setAnim(anim);
  });
  
  bright.subscribe([](){
    analogWrite(10, brightness);
  });
}



void loop() {
  static int prevval = 0;

  debouncer.update();
  
  int val = myEnc.read() / 4;
  if (val < prevval) nav->up();
  if (val > prevval) nav->down();
  
  prevval = val;
}
