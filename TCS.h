//Head file of TLL's CPU Simulator

#define TC0 0
//Machine TC0's version is 0 0

//define pause_keyname "<Space>"//Space key to pause
//define pause_keyid 32

//Code's length define(bytes)
#define CodeI 1
//4-bit Opcode
#define CodeII 1
//4-bit Opcode+Reg
#define CodeIII 2
//4-bit Opcode+4-bit zero+8-bit IMM(int)
#define CodeIV 2
//4-bit Opcode+4-bit Reg1+4-bit Reg2+4-bit zero
#define CodeV 2
//4-bit Opcode+4-bit Reg1+4-bit Reg2+4-bit Reg3

//Opcode define
#define ADD 0
#define ADD_L CodeII

#define AND 1
#define AND_L CodeII

#define OR 2
#define OR_L CodeII

#define NOT 3
#define NOT_L CodeI

#define GETR 4
#define GETR_L CodeII

#define SETR 5
#define SETR_L CodeII

#define SETI 6
#define SETI_L CodeIII

#define GETM 7
#define GETM_L CodeII

#define SETM 8
#define SETM_L CodeIV

#define JE 9
#define JE_L CodeIV

#define JS 0xA
#define JS_L CodeV

#define JMP 0xB
#define JMP_L CodeII

#define RMV 0xC
#define RMV_L CodeI

#define GETF 0xD
#define GETF_L CodeIV

#define JL 0xE
#define JL_L CodeIV

#define NOP 0xF
#define NOP_L CodeI
