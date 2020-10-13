#include <LiquidCrystal_I2C.h>

enum ItemType { TEXT, MENU, BACK, TOGGLE, VALUE };

#define SCRL 30
#define MAXITEMS 5
#define COLS 20
#define ROWS 2

class fxMenuNav;
typedef void (*CallbackType)(void);


class fxMenu {
public:
  fxMenu(String t = "") : title(t), enabled(true) { }

  void enable(bool on) { enabled = on; }
  bool isEnabled() { return enabled; }
  void setType(ItemType t) { type = t; }
  ItemType getType() { return type; }
  virtual String getTitle() { return title; }
  virtual void select(fxMenuNav* nav) { }

protected:  
  String title;
  bool enabled;
  ItemType type;
};


class fxMenuItem : public fxMenu {
public:
  fxMenuItem(String t) : fxMenu(t) { type = TEXT; }
};


class fxMenuItemBool : public fxMenuItem {
public:
  fxMenuItemBool(String t, bool* v)  : fxMenuItem(t), vv(v) { type = TOGGLE; cb = NULL; }
  virtual void select(fxMenuNav* nav) {
    *vv = !*vv;
    if (cb) cb();
  }
  virtual String getTitle() { return title + " " + ( *vv ? "ON" : "OFF") ; }

  void subscribe(const CallbackType& func) {
    cb = func;
  }
    
protected:
  bool* vv;
  CallbackType cb;
};


class fxMenuItemInt : public fxMenuItem {
public:
  fxMenuItemInt(String t, int* v) : fxMenuItem(t), vv(v) { type = VALUE; cb = NULL; }
  virtual void select(fxMenuNav* nav) {
    *vv += 10;
    if (cb) cb();
  }
  virtual String getTitle() { return title + " " + String(*vv) ; }

  void subscribe(const CallbackType& func) {
    cb = func;
  }
    
protected:
  int* vv;
  CallbackType cb;
};


class fxMenuItemBack : public fxMenuItem {
public:
  fxMenuItemBack(String t = "Back")  : fxMenuItem(t) { type = BACK; }
};


class fxMenuList : public fxMenu {
public:
  fxMenuList(String t = "") : fxMenu(t), length(0), pos(0) { type = MENU; }

  fxMenuList* add(fxMenuItem* i) { items[length++] = i; return this; }
  fxMenuList* add(fxMenuList* i) { items[length++] = i; return this; }
  fxMenuList* add(fxMenuItemBack* i) { items[length++] = i; return this; }
  
  void up() { setPos(pos-1); }
  void down() { setPos(pos+1); }
  void setPos(int idx) { pos = constrain(idx, 0, length-1); }
  byte getPos() { return pos; }
  
  fxMenu* getItem(byte idx) { return items[idx]; }
  byte getLength() { return length; }

protected:
  fxMenu* items[MAXITEMS];
  byte length;
  byte pos;
};


class fxMenuNav {
public:
  fxMenuNav(LiquidCrystal_I2C& lcd) : lcd(lcd), anim(false) { }

  void setAnim(bool on) { anim = on; }

  void up() { setPos(pos-1);  }
  void down() { setPos(pos+1); }
  
  void setPos(int idx) {
    byte prevpos = pos;
    byte prevoff = offset;
    
    pos = constrain(idx, 0, menu->getLength()-1);
    while (pos >= offset + ROWS) { offset++; }
    while (pos < offset) { offset--; }

    if (prevpos != pos || prevoff != offset)
      render();
  }
  
  void select() {
    ItemType t = menu->getItem(pos)->getType();
    
    if (t == MENU) {
      prev = menu;
      prevpos = pos;
      fxMenuList* x = (fxMenuList*) (menu->getItem(pos));

      if (anim) {
        for(byte s=0; s<16; s++) { lcd.scrollDisplayLeft(); delay(SCRL); }
        for(byte s=0; s<32; s++) { lcd.scrollDisplayRight(); }
      }
      setMenu(x);

      if (anim) {
        for(byte s=0; s<16; s++) { lcd.scrollDisplayLeft(); delay(SCRL); }
      }
    }

    if (t == BACK) {
      if (anim) {
        for(byte s=0; s<16; s++) { lcd.scrollDisplayRight(); delay(SCRL); }
        for(byte s=0; s<32; s++) { lcd.scrollDisplayLeft(); }
      }

      setMenu(prev, prevpos);

      if (anim) {
        for(byte s=0; s<16; s++) { lcd.scrollDisplayRight(); delay(SCRL); }
      }
    }

    if (t == TOGGLE || t == VALUE) {
       menu->getItem(pos)->select(this);
       render();
    }
}

  void setMenu(fxMenuList* m, byte pos = 0) { 
    menu = m;
    setPos(pos);
    render();
  }
  
  void render() {
    for (byte i=offset; i<min(menu->getLength()-offset, ROWS)+offset; i++) {
      if (i == pos) Serial.print(">");
      Serial.println(menu->getItem(i)->getTitle());

      Serial.println(i-offset);
      lcd.setCursor(0, i-offset);
      char leadchar = ' ';
      if (i == pos) {
        if (!menu->getItem(i)->isEnabled()) { leadchar = 1; }
        else if (menu->getItem(i)->getType() == BACK) { leadchar = 2; }
        else leadchar = 0;
      }
      lcd.printByte(leadchar);
      
      lcd.print(menu->getItem(i)->getTitle());
      byte l = menu->getItem(i)->getTitle().length();
      for (byte ll=l+1; ll<COLS; ll++) { lcd.print(" "); } // clear rest of line
    }

 /*
    // Clear last line of display if needed
    if (menu->getLength() < ROWS-1) {
      lcd.setCursor(0, ROWS-1);
      for (byte ll=0; ll<COLS; ll++) { lcd.print(" "); } // clear rest of line
    }
 */   
    Serial.println();
  }
  
protected:
  LiquidCrystal_I2C& lcd;
  fxMenuList* menu;
  bool anim;
  byte pos;
  byte offset;
  fxMenuList* prev;
  byte prevpos;
};
