#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <map>
#include <vector>
// kosherniy variant
using namespace std;
enum Instr_t { RM, RR, B };
enum code_e {
  HALT = 0,
  SVC = 1,
  ADD = 2,
  SUB = 3,
  MUL = 4,
  DIV = 5,
  MOD = 6,
  AND = 7,
  OR = 8,
  XOR = 9,
  SHL = 11,
  SHR = 12,
  NAND = 10,
  ADDD = 13,
  SUBD = 14,
  MULD = 15,
  DIVD = 16,
  ITOD = 17,
  DTOI = 18,
  BL = 19,
  CMP = 20,
  CMPD = 21,
  CNE = 22,
  CEQ = 23,
  CLE = 24,
  CLT = 25,
  CGE = 26,
  CGT = 27,
  LD = 28,
  ST = 29,
};
class Instruct {
 public:
  Instruct(string n, Instr_t t, int c) {
    name = n;
    type = t;
    code = c;
  }
  Instruct() {}
  string name;
  Instr_t type;
  unsigned int code;
};
void CodeToMem(int *m, unsigned int &c) {
  static int pos = 0;
  m[pos] = c;
  pos++;
}
string ReadInstructFrommem(unsigned int c, map<unsigned int, Instruct> &I) {
  string s;
  unsigned int codeInstr = 0;
  codeInstr = c >> 24;
  return I[codeInstr].name;
}
int reg_to_code(string const &s) {
  if (s == "rz") return 27;
  if (s == "fp") return 28;
  if (s == "sp") return 29;
  if (s == "lr") return 30;
  if (s == "pc") return 31;
  string c = s.substr(1, s.length() - 1);
  return stoi(c);
}
void fill_with_instructions(map<string, Instruct> &m) {
  m["halt"] = Instruct("halt", Instr_t::RR, 0);
  m["svc"] = Instruct("svc", Instr_t::RR, 1);
  m["add"] = Instruct("add", Instr_t::RR, 2);
  m["sub"] = Instruct("sub", Instr_t::RR, 3);
  m["mul"] = Instruct("mul", Instr_t::RR, 4);
  m["div"] = Instruct("div", Instr_t::RR, 5);
  m["mod"] = Instruct("mod", Instr_t::RR, 6);
  m["and"] = Instruct("and", Instr_t::RR, 7);
  m["or"] = Instruct("or", Instr_t::RR, 8);
  m["xor"] = Instruct("xor", Instr_t::RR, 9);
  m["nand"] = Instruct("nand", Instr_t::RR, 10);
  m["shl"] = Instruct("shl", Instr_t::RR, 11);
  m["shr"] = Instruct("shr", Instr_t::RR, 12);
  m["addd"] = Instruct("addd", Instr_t::RR, 13);
  m["subd"] = Instruct("subd", Instr_t::RR, 14);
  m["muld"] = Instruct("muld", Instr_t::RR, 15);
  m["divd"] = Instruct("divd", Instr_t::RR, 16);
  m["itod"] = Instruct("itod", Instr_t::RR, 17);
  m["dtoi"] = Instruct("dtoi", Instr_t::RR, 18);
  m["bl"] = Instruct("bl", Instr_t::B, 19);
  m["cmp"] = Instruct("cmp", Instr_t::RR, 20);
  m["cmpd"] = Instruct("cmpd", Instr_t::RR, 21);
  m["cne"] = Instruct("cne", Instr_t::RR, 22);
  m["ceq"] = Instruct("ceq", Instr_t::RR, 23);
  m["cle"] = Instruct("cle", Instr_t::RR, 24);
  m["clt"] = Instruct("clt", Instr_t::RR, 25);
  m["cge"] = Instruct("cge", Instr_t::RR, 26);
  m["cgt"] = Instruct("cgt", Instr_t::RR, 27);
  m["ld"] = Instruct("ld", Instr_t::RM, 28);
  m["st"] = Instruct("st", Instr_t::RM, 29);
  return;
}
void FillReversMap(map<string, Instruct> &base,
                   map<unsigned int, Instruct> &m) {
  for (auto c : base) {
    m[c.second.code] = c.second;
  }
}
void changepartofcode(unsigned int &code, int pos, int length, unsigned int i) {
  unsigned int mask;
  mask = ~(((1 << length) - 1) << pos);
  i &= mask;
  code |= (i << pos);
}
void RegToMem(long long int c, int pos, char *mem);
long long int MemToReg(int pos, char *mem);
string outcode(int &c) {
  string s = "";
  for (int i = 31; i >= 0; i--) {
    int bit = (c >> i) & 1;
    s += to_string(bit);
  }
  return s;
}
long long int int_from_20(int inp) {
  int a = 0;
  int d = (inp >> 19) & 1;
  if (d) {
    a = -1;
    a = a << 20;
    a |= (inp & 0xFFFF);
  } else {
    a |= inp & 0xFFFF;
  }
  return a;
}
long long int int_from_16(int inp) {
  long long int a = 0;
  int d = (inp >> 15) & 1;
  if (d) {
    a = -1;
    a = a << 16;
    a |= (inp & 0xFFFF);
  } else {
    a |= inp & 0xFFFF;
  }
  return a;
}
long long int int_from_8(int inp) {
  int a = 0;
  int d = (inp >> 7) & 1;
  if (d) {
    a = -1;
    a = a << 8;
    a |= inp & 0xFF;
  } else {
    a |= inp & 0xFF;
  }
  return a;
}
long long int int_from_11(int inp) {
  int a = 0;
  int d = (inp >> 10) & 1;
  if (d) {
    a = -1;
    a = a << 11;
    a |= inp & 0x7FF;
  } else {
    a |= inp & 0x7FF;
  }
  return a;
}
long long int int_from_21(int inp) {
  int a = 0;
  int d = (inp >> 20) & 1;
  if (d) {
    a = -1;
    a = a << 21;
    a |= inp & 0x1FFFFF;
  } else {
    a |= inp & 0x1FFFFF;
  }
  return a;
}
long long int GetIntOpForRR(int &c, long long int *regs) {
  unsigned int reg1;
  unsigned int reg2;
  unsigned int reg3;
  long long int op;
  reg1 = (c >> 21) & (0x1F);
  reg2 = (c >> 16) & (0x1F);
  if (reg2 == 27) {
    op = int_from_16(c);
  } else {
    reg3 = (c >> 11) & (0x1F);
    int offset = (c >> 8) & 0x7;
    op = (regs[reg3] << (offset)) + int_from_8(c);
  }
  return op;
}
double GetDoubleOpForRRD(int &c, long long int *regs) {
  unsigned int reg1;
  unsigned int reg2;
  unsigned int reg3;
  double op;
  union DUI {
    double d;
    long long int i;
  };
  DUI d1;

  reg1 = (c >> 21) & (0x1F);
  reg2 = (c >> 16) & (0x1F);
  if (reg2 == 27) {
    op = (double)int_from_16(c);
  } else {
    reg3 = (c >> 11) & (0x1F);
    // cout<<" now with long op first reg = "<<regs[reg3];
    d1.i = regs[reg3];
    double offset = pow(2, (c >> 8) & 0x7);
    op = (d1.d * (offset)) + (double)int_from_8(c);
  }
  //  cout<<" OP now "<<op<<endl;
  return op;
}
void DoInstruct(int c, long long int *regs, char *mem) {
  unsigned int code;
  code = c >> 26;
  unsigned int reg1;
  unsigned int reg2;
  unsigned int reg3;
  union DUI {
    double d;
    long long int i;
  };
  DUI d1, d2;
  long long int op;
  // cout<<"CODE "<<code<<endl;
  switch (code) {
    case HALT:
      exit(0);
      break;
    case SVC: {
      regs[31] += 4;
      reg1 = (c >> 21) & (0x1F);
      op = GetIntOpForRR(c, regs);
      switch (op) {
        case 0:
          exit(0);
          break;
        case 100:
          cin >> regs[reg1];
          break;
        case 101:
          cin >> d1.d;
          regs[reg1] = d1.i;
          break;
        case 102:
          cout << regs[reg1];
          break;
        case 104:
          cin >> regs[reg1];
          break;
        case 103:
          d1.i = regs[reg1];
          cout << d1.d;
          break;
        case 105:
          cout << (char)regs[reg1];
          break;
      }
    } break;
    case ADD:
      regs[31] += 4;
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      regs[reg1] = regs[reg2] + GetIntOpForRR(c, regs);
      break;
    case SUB:
      regs[31] += 4;
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      regs[reg1] = regs[reg2] - GetIntOpForRR(c, regs);
      break;
    case MUL:
      regs[31] += 4;
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      regs[reg1] = regs[reg2] * GetIntOpForRR(c, regs);
      break;
    case DIV:
      regs[31] += 4;
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      regs[reg1] = regs[reg2] / GetIntOpForRR(c, regs);
      break;
    case MOD:
      regs[31] += 4;
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      regs[reg1] = regs[reg2] % GetIntOpForRR(c, regs);
      break;
    case AND:
      regs[31] += 4;
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      regs[reg1] = regs[reg2] & GetIntOpForRR(c, regs);
      break;
    case OR:
      regs[31] += 4;
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      regs[reg1] = regs[reg2] | GetIntOpForRR(c, regs);
      break;
    case XOR:
      regs[31] += 4;
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      regs[reg1] = regs[reg2] ^ GetIntOpForRR(c, regs);
      break;
    case NAND:
      regs[31] += 4;
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      regs[reg1] = regs[reg2] & (~(GetIntOpForRR(c, regs)));
      break;
    case SHL:
      regs[31] += 4;
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      regs[reg1] = regs[reg2] << GetIntOpForRR(c, regs);
      break;
    case SHR:
      regs[31] += 4;
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      regs[reg1] = regs[reg2] >> GetIntOpForRR(c, regs);
      break;
    case ADDD:
      regs[31] += 4;
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      d2.i = regs[reg2];
      d1.d = d2.d + GetDoubleOpForRRD(c, regs);
      regs[reg1] = d1.i;
      break;
    case SUBD:
      regs[31] += 4;
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      d2.i = regs[reg2];
      d1.d = d2.d - GetDoubleOpForRRD(c, regs);
      regs[reg1] = d1.i;
      break;
    case MULD:
      regs[31] += 4;
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      d2.i = regs[reg2];
      d1.d = d2.d * GetDoubleOpForRRD(c, regs);
      regs[reg1] = d1.i;
      break;
    case DIVD:
      regs[31] += 4;
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      d2.i = regs[reg2];
      // s cout<<"DELENITE "<<d2.d<<endl;
      d1.d = d2.d / GetDoubleOpForRRD(c, regs);
      regs[reg1] = d1.i;
      break;
    case DTOI:
      regs[31] += 4;
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      d2.i = regs[reg2];
      regs[reg1] = (long long int)(d2.d + GetDoubleOpForRRD(c, regs));
      break;
    case ITOD:
      regs[31] += 4;
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      d1.d = (double)(regs[reg2] + GetIntOpForRR(c, regs));
      regs[reg1] = d1.i;
      break;
    case BL:
      regs[31] += 4;
      reg1 = (c >> 21) & (0x1F);
      op = (c) & (0x1FFFFF);
      if (reg1 == 30) op = (c) & (0x1FFFFF);
      if (reg1 == 31) op = int_from_21(c);
      if (reg1 == 0) {
        op = (c) & (0x1FFFFF);
      }
      regs[30] = regs[31];
      regs[31] = op;
      break;
    case ST:
      regs[31] += 4;
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      if (reg2 == 29) {
        op = c & 0xFFFF;
        regs[29] -= op;
        RegToMem(regs[reg1], regs[29], mem);
      } else {
        if (reg2 == 27) {
          op = c & 0xFFFF;
          RegToMem(regs[reg1], op, mem);
        } else {
          if (reg2 == 31) {
            op = int_from_16(c);
            RegToMem(regs[reg1], regs[31] + op, mem);
          } else {
            reg3 = (c >> 11) & (0x1F);
            if (reg3 == 27) {
              op = int_from_11(c);
              RegToMem(regs[reg1], regs[reg2] + op, mem);
            } else {
              int offset = (c >> 8) & 0x7;
              op = (regs[reg3] << offset) + int_from_8(c);
              RegToMem(regs[reg1], regs[reg2] + op, mem);
            }
          }
        }
      }
      break;
    case LD:
      regs[31] += 4;
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      if (reg2 == 29) {
        op = c & 0xFFFF;
        regs[reg1] = MemToReg(regs[29], mem);
        regs[29] += op;
      } else {
        if (reg2 == 27) {
          op = c & 0xFFFF;
          regs[reg1] = MemToReg(op, mem);
        } else {
          if (reg2 == 31) {
            op = int_from_16(c);
            regs[reg1] = MemToReg(regs[31] + op, mem);
          } else {
            reg3 = (c >> 11) & (0x1F);
            if (reg3 == 27) {
              op = int_from_11(c);
              regs[reg1] = MemToReg(regs[reg2] + op, mem);
            } else {
              int offset = (c >> 8) & 0x7;
              op = (regs[reg3] << offset) + int_from_8(c);
              regs[reg1] = MemToReg(regs[reg2] + op, mem);
            }
          }
        }
      }
      break;
    case CMP:
      regs[31] += 4;  //> = 1  < = 0 = = 2
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      if (regs[reg1] > regs[reg2] + GetIntOpForRR(c, regs)) regs[32] = 1;
      if (regs[reg1] < regs[reg2] + GetIntOpForRR(c, regs)) regs[32] = 0;
      if (regs[reg1] == regs[reg2] + GetIntOpForRR(c, regs)) regs[32] = 2;
      break;
    case CMPD:
      regs[31] += 4;  //> = 1  < = 0 = = 2
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      d1.i = regs[reg1];
      d2.i = regs[reg2];
      // cout<<"chisla "<<d1.d<<" "<<d2.d;
      if (d1.d > d2.d + GetDoubleOpForRRD(c, regs)) regs[32] = 1;
      if (d1.d < d2.d + GetDoubleOpForRRD(c, regs)) regs[32] = 0;
      if (d1.d == d2.d + GetDoubleOpForRRD(c, regs)) regs[32] = 2;
      break;
    case CEQ:

      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      regs[31] += 4;
      if (regs[32] == 2) regs[reg1] = regs[reg2] + GetIntOpForRR(c, regs);
      break;
    case CNE:
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      regs[31] += 4;
      if (regs[32] == 1 || regs[32] == 0)
        regs[reg1] = regs[reg2] + GetIntOpForRR(c, regs);
      break;
    case CLE:
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      regs[31] += 4;
      if (regs[32] == 2 || regs[32] == 0)
        regs[reg1] = regs[reg2] + GetIntOpForRR(c, regs);
      break;
    case CLT:
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      regs[31] += 4;
      if (regs[32] == 0) regs[reg1] = regs[reg2] + GetIntOpForRR(c, regs);
      break;
    case CGE:
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      regs[31] += 4;
      if (regs[32] == 2 || regs[32] == 1)
        regs[reg1] = regs[reg2] + GetIntOpForRR(c, regs);
      break;
    case CGT:
      reg1 = (c >> 21) & (0x1F);
      reg2 = (c >> 16) & (0x1F);
      regs[31] += 4;
      if (regs[32] == 1) regs[reg1] = regs[reg2] + GetIntOpForRR(c, regs);
      break;
  }
}

vector<string> Split(
    string s, map<string, unsigned int> &marks,
    vector<string> &nonstopinst)  // the original idea belongs to Babichev S.
                                  // patent issued 04.31.2020 #24234312397296
{
  static int pos = 0;
  // cout<<s<<endl;
  string cur = "";
  vector<string> res;
  for (auto c : s) {
    if (c == ';') break;
    if (c == ':') {
      marks[cur] = pos;
      cur.clear();
    } else {
      bool approp_c = true;
      if (c >= '0' && c <= '9') approp_c = false;
      if (c >= 'a' && c <= 'z') approp_c = false;
      if (c >= 'A' && c <= 'Z') approp_c = false;
      if (c == '_' || c == '-') approp_c = false;
      if (approp_c) {
        if (cur.length() > 0) {
          nonstopinst.push_back(cur);
          res.push_back(cur);
          cur.clear();
        }
      } else
        cur += c;
    }
  }
  if (cur.length() > 0) {
    nonstopinst.push_back(cur);
    res.push_back(cur);
  }
  // for(auto c: res) cout<<" "<<c; cout<<endl;
  if (res.size() > 0) {
    if (res[0] == "bytes") {
      pos += stoi(res[1]);
      return res;
    }
    if (res[0] != "word" && res[0] != "double") {
      pos += 4;
    } else {
      pos += 8;
    }
  }
  return res;
}
void Instruct_to_mem(int c, char *mem) {
  static unsigned int pos = 0;
  union u {
    char c[4];
    int i;
  };
  u u1;
  u1.i = c;
  for (int i = 0; i < 4; i++) {
    mem[pos + i] = u1.c[3 - i];
  }
  pos += 4;
}
void RegToMem(long long int c, int pos, char *mem) {
  union u {
    char c[8];
    long long int i;
  };
  u u1;
  u1.i = c;
  for (int i = 0; i < 8; i++) {
    mem[pos + i] = u1.c[7 - i];
  }
}
long long int MemToReg(int pos, char *mem) {
  union u {
    char c[8];
    long long int i;
  };
  u u1;
  for (int i = 0; i < 8; i++) {
    u1.c[7 - i] = mem[pos + i];
  }
  return u1.i;
}
int mem_to_instr(int pos, char *mem) {
  union u {
    char c[4];
    int i;
  };
  u u1;
  for (int i = 0; i < 4; i++) {
    u1.c[3 - i] = mem[pos + i];
  }
  return u1.i;
}
void showcell(char *mem, int pos) {
  char c = mem[pos];
  int d = 0;
  for (int i = 7; i >= 0; i--) {
    d = (c >> i) & 1;
    if (d)
      cout << "1";
    else
      cout << "0";
  }
  return;
}
int main() {
  map<string, Instruct> Allinstrcts;
  map<unsigned int, Instruct> AllinstrctsRev;
  map<string, unsigned int> marks;
  ifstream in("input.fasm");
  vector<string> nonstopinstr;
  long long int registres[33];
  for (int i = 0; i < 33; i++) registres[i] = 0;
  char *memor = new char[2097152];
  fill_with_instructions(Allinstrcts);
  FillReversMap(Allinstrcts, AllinstrctsRev);
  unsigned int length_of_program, start_point;
  // FILE *f;
  // f = fopen("output.txt","w"); fprintf(f,"%s",""); fclose(f);
  string s;
  while (getline(in, s)) {
    Split(s, marks, nonstopinstr);
  }
  //  for(auto c:marks) cout<<c.first<<" "<<c.second<<endl;
  // for(auto c:nonstopinstr) cout<<c<<endl;
  for (int i = 0; i < nonstopinstr.size();) {
    int code = 0;
    string cur = nonstopinstr[i];
    if (cur == "end") break;
    if (cur == "word") {
      i++;
      code = stoi(nonstopinstr[i]);
      Instruct_to_mem(code, memor);
      i++;
      continue;
    }
    if (cur == "bytes") {
      i++;
      i++;
      continue;
    }
    Instruct instr = Allinstrcts[cur];
    code |= instr.code << 26;
    i++;
    cur = nonstopinstr[i];
    if (instr.code >= 22 && instr.code <= 27) {
      code |= reg_to_code(cur) << 21;
      i++;
      cur = nonstopinstr[i];
      if (marks.find(cur) != marks.end()) {
        code |= (marks[cur] & 0xFFFF);
        code |= 27 << 16;
      } else {
        if (cur != "rz") {
          cur = nonstopinstr[i];
          code |= reg_to_code(cur) << 11;
          i++;
          cur = nonstopinstr[i];
          code |= stoi(cur) << 8;
          i++;
          cur = nonstopinstr[i];
          code |= stoi(cur) & 0xFF;
        } else {
          cur = nonstopinstr[i];
          code |= stoi(cur) & 0xFFFF;
        }
      }
      i++;
      Instruct_to_mem(code, memor);
      continue;
    }
    if (instr.type == Instr_t::RR) {
      code |= reg_to_code(cur) << 21;
      i++;
      cur = nonstopinstr[i];

      if (cur == "pc") {
        code |= reg_to_code("rz") << 16;
        i++;
        cur = nonstopinstr[i];
        if (marks.find(cur) != marks.end()) {
          code |= (marks[cur] & 0xFFFF);
          i++;
          Instruct_to_mem(code, memor);
          continue;
        }
      }
      code |= reg_to_code(cur) << 16;
      i++;
      if (cur != "rz") {
        cur = nonstopinstr[i];
        if (marks.find(cur) != marks.end()) {
          code |= (marks[cur] & 0xFFFF);
          i++;
          Instruct_to_mem(code, memor);
          continue;
        }
        code |= reg_to_code(cur) << 11;
        i++;
        cur = nonstopinstr[i];
        code |= stoi(cur) << 8;
        i++;
        cur = nonstopinstr[i];
        if (marks.find(cur) != marks.end()) {
          code |= marks[cur] & 0xFF;
          i++;
          Instruct_to_mem(code, memor);
          continue;
        }
        code |= stoi(cur) & 0xFF;
      } else {
        cur = nonstopinstr[i];
        if (marks.find(cur) != marks.end()) {
          code |= (marks[cur] & 0xFFFF);
        } else
          code |= stoi(cur) & 0xFFFF;
      }
    }
    if (instr.type == Instr_t::RM) {
      code |= reg_to_code(cur) << 21;
      i++;
      cur = nonstopinstr[i];
      code |= reg_to_code(cur) << 16;
      i++;
      if (cur == "pc" || cur == "rc" || cur == "sp") {
        if (cur == "pc") {
          cur = nonstopinstr[i];
          code |= stoi(cur) & 0xFFFF;
        }
        if (cur == "pz") {
          cur = nonstopinstr[i];
          if (marks.find(cur) != marks.end())
            code |= (marks[cur] & 0xFFFF);
          else
            code |= stoi(cur);
        }
        if (cur == "sp") {
          cur = nonstopinstr[i];
          code |= stoi(cur);
        }
      } else {
        cur = nonstopinstr[i];
        code |= reg_to_code(cur) << 11;
        i++;
        if (cur != "rz") {
          cur = nonstopinstr[i];
          code |= stoi(cur) << 8;
          i++;
          cur = nonstopinstr[i];
          code |= stoi(cur) & 0xFF;
        } else {
          cur = nonstopinstr[i];
          code |= stoi(cur) & 0x7FF;
        }
      }
    }
    if (instr.type == Instr_t::B) {
      if (marks.find(cur) != marks.end()) {
        code |= marks[cur] & 0xFFFF;
      } else {
        code |= reg_to_code(cur) << 21;
        i++;
        if (cur == "pc" || cur == "rz") {
          if (cur == "rz") {
            cur = nonstopinstr[i];
            code |= stoi(cur);
          }
          if (cur == "pc") {
            cur = nonstopinstr[i];
            code |= stoi(cur) & 0x1FFFFF;
          }
        } else {
        }
      }
    }
    i++;
    Instruct_to_mem(code, memor);
  }
  registres[31] = marks[nonstopinstr[nonstopinstr.size() - 1]];
  registres[29] = 2097144;
  in.close();
  while (true) {
    // s cout<<registres[31]<<endl;
    DoInstruct(mem_to_instr(registres[31], memor), registres, memor);
  }
  delete[] memor;
  return 0;
}
