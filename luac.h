/*
** $Id: luac.h,v 1.12 1999/09/09 13:24:52 lhf Exp lhf $
** definitions for luac
** See Copyright Notice in lua.h
*/

#define LUA_REENTRANT

#include "lauxlib.h"
#include "lfunc.h"
#include "lmem.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstring.h"
#include "lundump.h"

extern lua_State *lua_state;
#define	L	lua_state

typedef struct
{
 const char* name;			/* name of opcode */
 int op;				/* value of opcode */
 int class;				/* class of opcode (byte variant) */
 int args;				/* types of arguments (operands) */
 int arg;				/* arg #1 */
 int arg2;				/* arg #2 */
} Opcode;

/* from dump.c */
void luaU_dumpchunk(const TProtoFunc* Main, FILE* D, int native);

/* from opcode.c */
int luaU_opcodeinfo(const TProtoFunc* tf, const Byte* p, Opcode* I,
	const char* xFILE, int xLINE);
int luaU_codesize(const TProtoFunc* tf);

/* from opt.c */
void luaU_optchunk(TProtoFunc* Main);

/* from print.c */
void luaU_printchunk(const TProtoFunc* Main);

/* from test.c */
void luaU_testchunk(const TProtoFunc* Main);
const TObject* luaU_getconstant(const TProtoFunc* tf, int i, int at);

#define INFO(tf,p,I)	luaU_opcodeinfo(tf,p,I,__FILE__,__LINE__)

/* fake (but convenient) opcodes */
#define NOP	255
#define STACK	(-1)
#define ARGS	(-2)
#define VARARGS	(-3)
