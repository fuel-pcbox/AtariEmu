#include <SDL/SDL.h>
#include <cstdio>
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
    else if(addr >= 0xD800 && addr <= 0xFFFF)
    {
      return os[addr - 0xD800];
    }
  }
}  

#ifdef DEBUG
#define debug_print(...) printf(__VA_ARGS__)
#else
#define debug_print(...)
#endif

namespace CPU
{
  u8 a,x,y;
  u8 s = 0;
  u8 flags = 0x20;
  u16 pc = 0xFF;;
  
  u8 instructionreg = 0;
  bool reset = true;
  int cycle = 0;
  bool execing = false;
  
  u8 tmp1;
  u8 tmp2;
  u16 tmp3;
  u16 tmp4;
  
  void loadroms()
  {
    FILE* fp = fopen("co12399b.rom","rb");
    fread(Memory::os,1,0x800,fp);
    fclose(fp);
    fp = fopen("co12499b.rom","rb");
    fread(Memory::os+0x800,1,0x1000,fp);
    fclose(fp);
    fp = fopen("co14599b.rom","rb");
    fread(Memory::os+0x1800,1,0x1000,fp);
    fclose(fp);
  }
  void tick()
  {
    if(reset)
    {
      switch(cycle)
      {
        case 0:
        {
          Memory::rb(pc);
          cycle++;
          break;
        }
        case 1:
        {
          Memory::rb(pc);
          cycle++;
          break;
        }
        case 2:
        {
          Memory::rb(pc);
          cycle++;
          break;
        }
        case 3:
        {
          Memory::rb(0x100 + s);
          s--;
          cycle++;
          break;
        }
        case 4:
        {
          Memory::rb(0x100 + s);
          s--;
          cycle++;
          break;
        }
        case 5:
        {
          Memory::rb(0x100 + s);
          s--;
          cycle++;
          break;
        }
        case 6:
        {
          pc = Memory::rb(0xFFFC);
          cycle++;
          break;
        }
        case 7:
        {
          pc |= (Memory::rb(0xFFFD) << 8);
          debug_print("[CPU] Reset finished.\n");
          execing = false;
          reset = false;
          break;
        }
      }
    }
    else
    {
      if(!execing)
      {
        debug_print("opcode: %02x\n",instructionreg);
        debug_print("a: %02x\n",a);
        debug_print("x: %02x\n",x);
        debug_print("y: %02x\n",y);
        debug_print("s: %02x\n",s);
        debug_print("flags: %02x\n",flags);
        debug_print("pc: %04x\n",pc);
        instructionreg = Memory::rb(pc);
        pc++;
        cycle = 0;
        execing = true;
      }
      switch(instructionreg)
      {
        case 0x78:
        {
          switch(cycle)
          {
            case 0:
            {
              Memory::rb(pc);
              flags |= 4;
              execing = false;
              break;
            }
          }
          break;
        }
      }
    }
  }
};

int main()
{
  CPU::loadroms();
  for(int i = 0;i<50;i++)
  {
    CPU::tick();
  }
  return 0;
}