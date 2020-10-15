#include "fxMenuNavigation.h"

/**
 * Menu Navigation Controller
 */
fxMenuNav::fxMenuNav(LiquidCrystal_I2C& lcd) : lcd(lcd), anim(false), animdelay(30), focus(true) { }

void fxMenuNav::setAnim(bool on) { anim = on; }

void fxMenuNav::setAnimDelay(byte ms) { animdelay = ms; }

void fxMenuNav::up() { 
  if (focus) { setPos(pos-1); }
  else { xxx->up(); render(); }
}
void fxMenuNav::down() {
  if (focus) { setPos(pos+1); }
  else { xxx->down(); render(); }
}
  
void fxMenuNav::setPos(int idx) {
  byte prevpos = pos;
  byte prevoff = offset;
  
  pos = constrain(idx, 0, menu->getLength()-1);
  while (pos >= offset + ROWS) { offset++; }
  while (pos < offset) { offset--; }

  if (prevpos != pos || prevoff != offset)
    render();
}

void fxMenuNav::select() {
  fxMenu::ItemType t = menu->getItem(pos)->getType();
  
  if (t == fxMenu::MENU) {
    fxMenuList* x = (fxMenuList*) (menu->getItem(pos));
    enter(x);
  }

  if (t == fxMenu::BACK) {
    back();
  }

  if (t == fxMenu::TOGGLE || t == fxMenu::VALUE) {
    if (menu->getItem(pos)->isEnabled()) {
      bool ret = menu->getItem(pos)->select();
      focus = ret;
      if (!focus) focusrow = pos-offset;
      render();
      xxx = menu->getItem(pos);
    }
  }
}

void fxMenuNav::enter(fxMenuList* x) {
  prev = menu;
  prevpos = pos;

  if (anim) { // move out to left
    for(byte s=0; s<16; s++) { lcd.scrollDisplayLeft(); delay(animdelay); }
    for(byte s=0; s<32; s++) { lcd.scrollDisplayRight(); }
  }
  setMenu(x);

  if (anim) { // move in from right
    for(byte s=0; s<16; s++) { lcd.scrollDisplayLeft(); delay(animdelay); }
  }    
}

void fxMenuNav::back() {
  if (anim) { // move out to right
    for(byte s=0; s<COLS; s++) { lcd.scrollDisplayRight(); delay(animdelay); }
    for(byte s=0; s<2*COLS; s++) { lcd.scrollDisplayLeft(); }
  }
  setMenu(prev, prevpos);

  if (anim) { // move in from left
    for(byte s=0; s<COLS; s++) { lcd.scrollDisplayRight(); delay(animdelay); }
  }    
}

void fxMenuNav::setMenu(fxMenuList* m, byte pos) { 
  menu = m;
  setPos(pos);
  render();
}

void fxMenuNav::setMenu(fxMenuList& m, byte pos) { 
  setMenu(&m, pos);
}

void fxMenuNav::render() {
  Serial.println();

  for (byte i=offset; i<min(menu->getLength()-offset, ROWS)+offset; i++) {

    if (i == pos) Serial.print(">");
    Serial.println(menu->getItem(i)->getTitle());

    lcd.setCursor(0, i-offset);
    char leadchar = ' ';
    if (i == pos) {
      if (!menu->getItem(i)->isEnabled()) { leadchar = 1; }
      else if (menu->getItem(i)->getType() == fxMenu::BACK) { leadchar = 2; }
      else leadchar = 0;
    }
    lcd.write(leadchar);
    lcd.print(menu->getItem(i)->getTitle());
    byte l = menu->getItem(i)->getTitle().length();
    for (byte ll=l+1; ll<COLS; ll++) { lcd.write(' '); } // clear rest of line
  }
  if (!focus) {
    lcd.setCursor(0, focusrow);
    lcd.blink();
  }
 
  // Clear last line of display if needed
  if (menu->getLength() < ROWS) {
    lcd.setCursor(0, ROWS-1); // TODO fix for more than 2 rows
    for (byte ll=0; ll<COLS; ll++) { lcd.write(' '); } // clear rest of line
  }
}
