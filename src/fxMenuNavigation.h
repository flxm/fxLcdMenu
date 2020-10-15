#pragma once

#include <LiquidCrystal_I2C.h>

#include "fxMenuBase.h"
#include "fxMenuList.h"

/**
 * Menu Navigation Controller
 */
class fxMenuNav {
public:
  fxMenuNav(LiquidCrystal_I2C& lcd);
  void setAnim(bool on);
  void setAnimDelay(byte ms);
  void setSkipDisabled(bool on);

  void up();
  void down();
    
  void setPos(int idx);
  void select();
  void enter(fxMenuList* x);
  void back();

  void setMenu(fxMenuList* m, byte pos = 0);
  void setMenu(fxMenuList& m, byte pos = 0);
  void render();
  
protected:
  LiquidCrystal_I2C& lcd;
  fxMenuList* menu;
  byte pos;
  byte offset;

  bool anim;
  byte animdelay;
  bool skipDisabled;
  
  fxMenuList* prev;
  byte prevpos;

  byte focusrow;
  bool focus;
  fxMenu* xxx; // TODO
};
