#include <SDL/SDL.h>
#include "../6502Core/m6502.h"
#include "gtia.h"
#include <cstdio>
#include <cstdlib>
#include <cstdint>

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;

namespace Memory
{
  u8 ram[0x2000];
  u8 romcart[0x4000];
  u8 os[0x2800];

  gtia gfx;
  
  void loadroms()
  {
    FILE* fp = fopen("co12399b.rom","rb");
    fread(os,1,0x800,fp);
    fclose(fp);
    fp = fopen("co12499b.rom","rb");
    fread(os+0x800,1,0x1000,fp);
    fclose(fp);
    fp = fopen("co14599b.rom","rb");
    fread(os+0x1800,1,0x1000,fp);
    fclose(fp);
    memset(romcart,0xff,0x4000);
  }

  //Read byte
  u8 rb(u16 addr)
  {
    if(addr < 0x2000)
    {
      return ram[addr];
    }
    else if(addr >= 0x8000 && addr < 0xC000)
    {
      return romcart[addr - 0x8000];
    }
    else if(addr >= 0xD000 && addr <= 0xD0FF)
    {
      return gfx.rb(addr);
    }
    else if(addr >= 0xD800 && addr <= 0xFFFF)
    {
      return os[addr - 0xD800];
    }
    else printf("[Memory] Unhandled read from address %04x\n", addr);
  }
  //Write byte
  void wb(u16 addr, u8 data)
  {
    if(addr < 0x2000)
    {
      ram[addr] = data;
    }
    else if(addr >= 0xD000 && addr <= 0xD0FF)
    {
      gfx.wb(addr,data);
    }
    else printf("[Memory] Unhandled write to address %04x with data %02x!\n", addr, data);
  }
}

int main()
{
  m6502 cpu;

  cpu.rb = Memory::rb;
  cpu.wb = Memory::wb;

  cpu.init(true);

  Memory::gfx.init();

  Memory::loadroms();
  for(int i = 0;i<500000;i++)
  {
    cpu.tick();
    printf("op=%02X\n",cpu.op);
    printf("cycle=%d\n",i);
    printf("a=%02X\n",cpu.a);
    printf("x=%02X\n",cpu.x);
    printf("y=%02X\n",cpu.y);
    printf("s=%02X\n",cpu.s);
    printf("p=%02X\n",cpu.flags);
    printf("pc=%04X\n",cpu.pc);
  }
  return 0;
}
