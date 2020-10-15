#include "fxMenuList.h"

fxMenuList::fxMenuList(String t) : fxMenu(t), length(0), pos(0) { type = MENU; }

fxMenuList* fxMenuList::add(fxMenu* i) { items[length++] = i; return this; }
fxMenuList* fxMenuList::add(fxMenu& i) { items[length++] = &i; return this; }

void fxMenuList::up() { setPos(pos-1); }
void fxMenuList::down() { setPos(pos+1); }

void fxMenuList::setPos(int idx) { pos = constrain(idx, 0, length-1); }
byte fxMenuList::getPos() { return pos; }

fxMenu* fxMenuList::getItem(byte idx) { return items[idx]; }
byte fxMenuList::getLength() { return length; }
