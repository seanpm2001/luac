/*
** $Id: dump.c,v 1.17 1999/04/09 02:51:31 lhf Exp lhf $
** save bytecodes to file
** See Copyright Notice in lua.h
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "luac.h"

#ifdef OLD_ANSI
#define strerror(e)     "(no error message provided by operating system)"
#endif

#define DumpBlock(b,size,D)	fwrite(b,size,1,D)
#define	DumpInt			DumpLong

static void DumpWord(int i, FILE* D)
{
 int hi= 0x0000FF & (i>>8);
 int lo= 0x0000FF &  i;
 fputc(hi,D);
 fputc(lo,D);
}

static void DumpLong(long i, FILE* D)
{
 int hi= 0x00FFFF & (i>>16);
 int lo= 0x00FFFF & i;
 DumpWord(hi,D);
 DumpWord(lo,D);
}

static void DumpNumber(real x, FILE* D)
{
#if LUAC_NATIVE
 DumpBlock(&x,sizeof(x),D);
#else
 char b[256];
 int n;
 sprintf(b,NUMBER_FMT"%n",x,&n);
 fputc(n,D);
 DumpBlock(b,n,D);
#endif
}

static void DumpCode(TProtoFunc* tf, FILE* D)
{
 int size=luaU_codesize(tf);
 DumpLong(size,D);
 DumpBlock(tf->code,size,D);
}

static void DumpString(char* s, int size, FILE* D)
{
 if (s==NULL)
  DumpLong(0,D);
 else
 {
  DumpLong(size,D);
  DumpBlock(s,size,D);
 }
}

static void DumpTString(TaggedString* s, FILE* D)
{
 if (s==NULL)
  DumpString(NULL,0,D);
 else
  DumpString(s->str,s->u.s.len+1,D);
}

static void DumpLocals(TProtoFunc* tf, FILE* D)
{
 if (tf->locvars==NULL)
  DumpInt(0,D);
 else
 {
  LocVar* v;
  int n=0;
  for (v=tf->locvars; v->line>=0; v++)
   ++n;
  DumpInt(n,D);
  for (v=tf->locvars; v->line>=0; v++)
  {
   DumpInt(v->line,D);
   DumpTString(v->varname,D);
  }
 }
}

static void DumpFunction(TProtoFunc* tf, FILE* D);

static void DumpConstants(TProtoFunc* tf, FILE* D)
{
 int i,n=tf->nconsts;
 DumpInt(n,D);
 for (i=0; i<n; i++)
 {
  TObject* o=tf->consts+i;
  fputc(-ttype(o),D);			/* ttype(o) is negative - ORDER LUA_T */
  switch (ttype(o))
  {
   case LUA_T_NUMBER:
	DumpNumber(nvalue(o),D);
	break;
   case LUA_T_STRING:
	DumpTString(tsvalue(o),D);
	break;
   case LUA_T_PROTO:
	DumpFunction(tfvalue(o),D);
	break;
   case LUA_T_NIL:
	break;
   default:				/* cannot happen */
	luaU_badconstant("dump",i,o,tf);
	break;
  }
 }
}

static void DumpFunction(TProtoFunc* tf, FILE* D)
{
 DumpInt(tf->lineDefined,D);
 DumpTString(tf->source,D);
 DumpCode(tf,D);
 DumpLocals(tf,D);
 DumpConstants(tf,D);
 if (ferror(D))
  luaL_verror("write error" IN ": %s (errno=%d)",INLOC,strerror(errno),errno);
}

static void DumpHeader(TProtoFunc* Main, FILE* D)
{
 fputc(ID_CHUNK,D);
 fputs(SIGNATURE,D);
 fputc(VERSION,D);
#if LUAC_NATIVE
 fputc(sizeof(real),D);
 DumpNumber(TEST_NUMBER,D);
#else
 fputc(0,D);
#endif
}

void luaU_dumpchunk(TProtoFunc* Main, FILE* D)
{
 DumpHeader(Main,D);
 DumpFunction(Main,D);
}
