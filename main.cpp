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
  //Write byte
  void wb(u16 addr, u8 data)
  {
    if(addr < 0x2000)
    {
      ram[addr] = data;
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
        instructionreg = Memory::rb(pc);
        pc++;
        cycle = 0;
        debug_print("opcode: %02x\n",instructionreg);
        debug_print("a: %02x\n",a);
        debug_print("x: %02x\n",x);
        debug_print("y: %02x\n",y);
        debug_print("s: %02x\n",s);
        debug_print("flags: %02x\n",flags);
        debug_print("pc: %04x\n",pc);
        execing = true;
      }
      switch(instructionreg)
      {
        case 0x10:
        {
          switch(cycle)
          {
            case 0:
            {
              tmp1 = Memory::rb(pc);
              pc++;
              cycle++;
              break;
            }
            case 1:
            {
              tmp2 = Memory::rb(pc);
              if(!(flags & 0x80))
              {
                tmp3 = pc + (s8)tmp1;
                pc = (pc & 0xFF00) | ((pc & 0xFF) + (s8)tmp1);
                cycle++;
              }
              else execing = false;
              break;
            }
            case 2:
            {
              tmp2 = Memory::rb(pc);
              if(tmp3 < pc) pc -= 0x100;
              if(tmp3 > pc) pc += 0x100;
              execing = false;
              break;
            }
          }
          break;
        }
        case 0x20:
        {
          switch(cycle)
          {
            case 0:
            {
              tmp3 = Memory::rb(pc);
              pc++;
              cycle++;
              break;
            }
            case 1:
            {
              Memory::rb(0x100 + s);
              cycle++;
              break;
            }
            case 2:
            {
              Memory::wb(0x100 + s, pc >> 8);
              s--;
              cycle++;
              break;
            }
            case 3:
            {
              Memory::wb(0x100 + s, pc & 0xFF);
              s--;
              cycle++;
              break;
            }
            case 4:
            {
              tmp3 |= Memory::rb(pc) << 8;
              pc = tmp3;
              execing = false;
            }
          }
          break;
        }
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
        case 0x84:
        {
          switch(cycle)
          {
            case 0:
            {
              tmp1 = Memory::rb(pc);
              pc++;
              cycle++;
              break;
            }
            case 1:
            {
              Memory::wb(tmp1,a);
              execing = false;
              break;
            }
          }
          break;
        }
        case 0x85:
        {
          switch(cycle)
          {
            case 0:
            {
              tmp1 = Memory::rb(pc);
              pc++;
              cycle++;
              break;
            }
            case 1:
            {
              Memory::wb(tmp1,x);
              execing = false;
              break;
            }
          }
          break;
        }
        case 0x9A:
        {
          switch(cycle)
          {
            case 0:
            {
              Memory::rb(pc);
              s = x; 
              execing = false;
              break;
            }
          }
          break;
        }
        case 0xA0:
        {
          switch(cycle)
          {
            case 0:
            {
              y = Memory::rb(pc);
              pc++;
              if(y & 0x80) flags |= 0x80;
              else flags &= 0x7F;
              if(y == 0) flags |= 0x02;
              else flags &= 0xFD;
              execing = false;
              break;
            }
          }
          break;
        }
        case 0xA2:
        {
          switch(cycle)
          {
            case 0:
            {
              x = Memory::rb(pc);
              pc++;
              if(x & 0x80) flags |= 0x80;
              else flags &= 0x7F;
              if(x == 0) flags |= 0x02;
              else flags &= 0xFD;
              execing = false;
              break;
            }
          }
          break;
        }
        case 0xA9:
        {
          switch(cycle)
          {
            case 0:
            {
              a = Memory::rb(pc);
              pc++;
              if(a & 0x80) flags |= 0x80;
              else flags &= 0x7F;
              if(a == 0) flags |= 0x02;
              else flags &= 0xFD;
              execing = false;
              break;
            }
          }
          break;
        }
        case 0xAD:
        {
          switch(cycle)
          {
            case 0:
            {
              tmp3 = Memory::rb(pc);
              pc++;
              cycle++;
              break;
            }
            case 1:
            {
              tmp3 |= Memory::rb(pc) << 8;
              pc++;
              cycle++;
              break;
            }
            case 2:
            {
              a = Memory::rb(tmp3);
              if(a & 0x80) flags |= 0x80;
              else flags &= 0x7F;
              if(a == 0) flags |= 0x02;
              else flags &= 0xFD;
              execing = false;
              break;
            }
          }
          break;
        }
        case 0xCE:
        {
          switch(cycle)
          {
            case 0:
            {
              tmp3 = Memory::rb(pc);
              pc++;
              cycle++;
              break;
            }
            case 1:
            {
              tmp3 |= Memory::rb(pc) << 8;
              pc++;
              cycle++;
              break;
            }
            case 2:
            {
              tmp1 = Memory::rb(tmp3);
              cycle++;
              break;
            }
            case 3:
            {
              Memory::wb(tmp3,tmp1);
              tmp1--;
              if(tmp1 & 0x80) flags |= 0x80;
              else flags &= 0x7F;
              if(tmp1 == 0) flags |= 0x02;
              else flags &= 0xFD;
              cycle++;
              break;
            }
            case 4:
            {
              Memory::wb(tmp3,tmp1);
              execing = false;
            }
          }
          break;
        }
        case 0xD0:
        {
          switch(cycle)
          {
            case 0:
            {
              tmp1 = Memory::rb(pc);
              pc++;
              cycle++;
              break;
            }
            case 1:
            {
              tmp2 = Memory::rb(pc);
              if(!(flags & 2))
              {
                tmp3 = pc + (s8)tmp1;
                pc = (pc & 0xFF00) | ((pc & 0xFF) + (s8)tmp1);
                cycle++;
              }
              else execing = false;
              break;
            }
            case 2:
            {
              tmp2 = Memory::rb(pc);
              if(tmp3 < pc) pc -= 0x100;
              if(tmp3 > pc) pc += 0x100;
              execing = false;
              break;
            }
          }
          break;
        }
        case 0xD8:
        {
          switch(cycle)
          {
            case 0:
            {
              Memory::rb(pc);
              flags &= 0xF7;
              execing = false;
              break;
            }
          }
          break;
        }
        case 0xEE:
        {
          switch(cycle)
          {
            case 0:
            {
              tmp3 = Memory::rb(pc);
              pc++;
              cycle++;
              break;
            }
            case 1:
            {
              tmp3 |= Memory::rb(pc) << 8;
              pc++;
              cycle++;
              break;
            }
            case 2:
            {
              tmp1 = Memory::rb(tmp3);
              cycle++;
              break;
            }
            case 3:
            {
              Memory::wb(tmp3,tmp1);
              tmp1++;
              if(tmp1 & 0x80) flags |= 0x80;
              else flags &= 0x7F;
              if(tmp1 == 0) flags |= 0x02;
              else flags &= 0xFD;
              cycle++;
              break;
            }
            case 4:
            {
              Memory::wb(tmp3,tmp1);
              execing = false;
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
  for(int i = 0;i<100;i++)
  {
    CPU::tick();
  }
  return 0;
}