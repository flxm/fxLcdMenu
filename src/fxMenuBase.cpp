#include "fxMenuBase.h"

fxMenu::fxMenu(String t) : title(t), enabled(true) { }

void fxMenu::setEnable(bool on) { enabled = on; }
bool fxMenu::isEnabled() { return enabled; }

void fxMenu::setType(ItemType t) { type = t; }
ItemType fxMenu::getType() { return type; }

String fxMenu::getTitle() { return title; }

/**
 * @return true if control is back to navigation
 */
bool fxMenu::select() { return true; }
void fxMenu::up() { }
void fxMenu::down() { }
