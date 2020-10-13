#include <LiquidCrystal_I2C.h>

enum ItemType { TEXT, MENU, BACK, TOGGLE, VALUE };

#define SCRL 30
#define MAXITEMS 5
#define COLS 20
#define ROWS 2

class fxMenuNav;
typedef void (*CallbackType)(void);

/**
 * Menu base class
 */
class fxMenu {
public:
  fxMenu(String t = "") : title(t), enabled(true) { }

  void enable(bool on) { enabled = on; }
  bool isEnabled() { return enabled; }
  
  void setType(ItemType t) { type = t; }
  ItemType getType() { return type; }
  
  virtual String getTitle() { return title; }

  /**
   * @return true if control is back to navigation
   */
  virtual bool select(fxMenuNav* nav) { return true; }
  virtual void up() { }
  virtual void down() { }

protected:  
  String title;
  bool enabled;
  ItemType type;
};

/**
 * Base Menu Item
 */
class fxMenuItem : public fxMenu {
public:
  fxMenuItem(String t) : fxMenu(t) { type = TEXT; }
};

/**
 * Bool Menu Toggle Item
 */
class fxMenuItemBool : public fxMenuItem {
public:
  fxMenuItemBool(String t, bool* v)  : fxMenuItem(t), vv(v), cb(NULL) { type = TOGGLE; }
  bool select(fxMenuNav* nav) override {
    *vv = !*vv;
    if (cb) cb();
    return true;
  }
  String getTitle() override { return title + " " + ( *vv ? "ON" : "OFF") ; }

  void subscribe(const CallbackType& func) {
    cb = func;
  }
    
protected:
  bool* vv;
  CallbackType cb;
};

/**
 * Value Menu Item
 */
class fxMenuItemInt : public fxMenuItem {
public:
  fxMenuItemInt(String t, int* v, int mn=0, int mx=100) : fxMenuItem(t), vv(v), cb(NULL), editing(false), _min(mn), _max(mx) { type = VALUE; }
  bool select(fxMenuNav* nav) override {
    if (editing) {
      editing = false;
      return true;
    }
    editing = true;
    return false;
  }
  virtual String getTitle() override {
    if (editing) return title + " <" + String(*vv) + ">";
    return title + " " + String(*vv);
    }

  void up() override {
    *vv = constrain(*vv-10, _min, _max);
    if (cb) cb();
  }

  void down() override {
    *vv = constrain(*vv+10, _min, _max);
    if (cb) cb();
  }
  
  void subscribe(const CallbackType& func) {
    cb = func;
  }
    
protected:
  int* vv;
  CallbackType cb;
  bool editing;
  int _min;
  int _max;
};

/**
 * Back Navigation Menu Item
 */
class fxMenuItemBack : public fxMenuItem {
public:
  fxMenuItemBack(String t = "Back")  : fxMenuItem(t) { type = BACK; }
};


class fxMenuList : public fxMenu {
public:
  fxMenuList(String t = "") : fxMenu(t), length(0), pos(0) { type = MENU; }

  fxMenuList* add(fxMenu* i) { items[length++] = i; return this; }
//  fxMenuList* add(fxMenuList* i) { items[length++] = i; return this; }
//  fxMenuList* add(fxMenuItemBack* i) { items[length++] = i; return this; }
  
  void up() override { setPos(pos-1); }
  void down() override { setPos(pos+1); }
  
  void setPos(int idx) { pos = constrain(idx, 0, length-1); }
  byte getPos() { return pos; }
  
  fxMenu* getItem(byte idx) { return items[idx]; }
  byte getLength() { return length; }

protected:
  fxMenu* items[MAXITEMS];
  byte length;
  byte pos;
};

/**
 * Menu Navigation Controller
 */
class fxMenuNav {
public:
  fxMenuNav(LiquidCrystal_I2C& lcd) : lcd(lcd), anim(false), focus(true) { }

  void setAnim(bool on) { anim = on; }

  void up() { 
    if (focus) { setPos(pos-1); }
    else { xxx->up(); render(); }
  }
  void down() {
    if (focus) { setPos(pos+1); }
    else { xxx->down(); render(); }
  }
    
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
      fxMenuList* x = (fxMenuList*) (menu->getItem(pos));
      enter(x);
    }

    if (t == BACK) {
      back();
    }

    if (t == TOGGLE || t == VALUE) {
      bool ret = menu->getItem(pos)->select(this);
      focus = ret;
      if (!focus) focusrow = pos-offset;
      render();
      xxx = (fxMenuItemInt*)menu->getItem(pos);
    }
  }

  void enter(fxMenuList* x) {
    prev = menu;
    prevpos = pos;

    if (anim) {
      for(byte s=0; s<16; s++) { lcd.scrollDisplayLeft(); delay(SCRL); }
      for(byte s=0; s<32; s++) { lcd.scrollDisplayRight(); }
    }
    setMenu(x);

    if (anim) {
      for(byte s=0; s<16; s++) { lcd.scrollDisplayLeft(); delay(SCRL); }
    }    
  }

  void back() {
    if (anim) {
      for(byte s=0; s<COLS; s++) { lcd.scrollDisplayRight(); delay(SCRL); }
      for(byte s=0; s<2*COLS; s++) { lcd.scrollDisplayLeft(); }
    }
    setMenu(prev, prevpos);

    if (anim) {
      for(byte s=0; s<COLS; s++) { lcd.scrollDisplayRight(); delay(SCRL); }
    }    
  }

  void setMenu(fxMenuList* m, byte pos = 0) { 
    menu = m;
    setPos(pos);
    render();
  }
  
  void render() {
    Serial.println();

    for (byte i=offset; i<min(menu->getLength()-offset, ROWS)+offset; i++) {
  
      if (i == pos) Serial.print(">");
      Serial.println(menu->getItem(i)->getTitle());

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
    if (!focus) {
      lcd.setCursor(0, focusrow);
      //lcd.cursor();
      lcd.blink();
    }
 /*
    // Clear last line of display if needed
    if (menu->getLength() < ROWS-1) {
      lcd.setCursor(0, ROWS-1);
      for (byte ll=0; ll<COLS; ll++) { lcd.print(" "); } // clear rest of line
    }
 */   
  }
  
protected:
  LiquidCrystal_I2C& lcd;
  fxMenuList* menu;
  bool anim;
  byte pos;
  byte offset;
  fxMenuList* prev;
  byte prevpos;

  bool focus;
  fxMenuItemInt* xxx;
  byte focusrow;
};
