#ifndef GTIA_H
#define GTIA_H

#include "../6502Core/m6502.h"

struct gtia
{
  u8 consol;
  u8 consol_ena;
  
  void init();
  u8 rb(u16 addr);
  void wb(u16 addr, u8 val);
};

#endif //GTIA_H