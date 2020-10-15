#pragma once

#include <arduino.h>

#define COLS 20
#define ROWS 2


/**
 * Menu base class
 */
class fxMenu {
public:
  enum ItemType { TEXT, MENU, BACK, TOGGLE, VALUE };

  fxMenu(String t = "");

  void setEnable(bool on);
  bool isEnabled();
  
  void setType(fxMenu::ItemType t);
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
