# fxLcdMenu

**Customizable menu library for LC Displays**

![License: MIT](https://img.shields.io/github/license/flxm/fxMenu)


## Purpose


## Simple Example

```c++
#include <LiquidCrystal_I2C.h>
#include <fxMenu.h>

LiquidCrystal_I2C lcd;

fxMenuNav* nav = new fxMenuNav();
fxMenu* mainmenu = new fxMenu();
fxMenuItem first("First");

void setup() {
  lcd.begin();
  mainmenu->add(&first);
  nav->setMenu(mainmenu);
}
```


## Features

- programmatic menu navigation
- ideal for use with rotary controllers
- horizontal scroll transition effects


## Screenshots


## Dependencies

This library depends on the following libraries:

- LiquidCrystal_I2C https://github.com/johnrickman/LiquidCrystal_I2C


## History

### 0.1.0
  - initial github commit
