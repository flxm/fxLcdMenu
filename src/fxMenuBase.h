#pragma once

#include <arduino.h>

enum ItemType { TEXT, MENU, BACK, TOGGLE, VALUE };

#define COLS 20
#define ROWS 2

/**
 * Menu base class
 */
class fxMenu {
public:
  fxMenu(String t = "");

  void setEnable(bool on);
  bool isEnabled();
  
  void setType(ItemType t);
  ItemType getType();
  
  virtual String getTitle();

  /**
   * @return true if control is back to navigation
   */
  virtual bool select();
  virtual void up();
  virtual void down();

protected:  
  String title;
  bool enabled;
  ItemType type;
};
