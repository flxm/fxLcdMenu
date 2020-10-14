#pragma once

#include "fxMenuBase.h"

#define MAXITEMS 5


class fxMenuList : public fxMenu {
public:
  fxMenuList(String t = "");

  fxMenuList* add(fxMenu* i);

  void up() override;
  void down() override;
  
  void setPos(int idx);
  byte getPos();
  
  fxMenu* getItem(byte idx);
  byte getLength();

protected:
  fxMenu* items[MAXITEMS];
  byte length;
  byte pos;
};
