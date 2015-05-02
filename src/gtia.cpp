#include "gtia.h"

void gtia::init()
{
  consol = 0x0f;
  consol_ena = 0x00;
}

u8 gtia::rb(u16 addr)
{
  u8 res = 0;
  switch(addr & 0x1f)
  {
    case 0x1f:
    {
      res = consol & 7;
      if(consol_ena & 1) res &= 6;
      if(consol_ena & 2) res &= 5;
      if(consol_ena & 4) res &= 3;
      break;
    }
  }
  return res;
}

void gtia::wb(u16 addr, u8 val)
{
  switch(addr & 0x1f)
  {
    case 0x1f:
    {
      consol_ena = val & 0xf;
      break;
    }
  }
}