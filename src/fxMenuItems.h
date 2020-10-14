#include "fxMenuBase.h"

typedef void (*CallbackType)(void);

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
  bool select() override {
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
  bool select() override {
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
