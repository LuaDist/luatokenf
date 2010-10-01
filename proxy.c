/*
* proxy.c
* lexer proxy for Lua parser -- implements token filtering
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 12 Sep 2006 11:30:46
* This code is hereby placed in the public domain.
* based on a version by Daniel Silverstone <dsilvers@digital-scurf.org>
*/

#define FILTER	"FILTER"

static int gettoken(lua_State *L)
{
 LexState *ls=lua_touserdata(L,lua_upvalueindex(1));
 SemInfo seminfo;
 int c;
 if (ls!=NULL && ls->fs==NULL) return 0;
 if (ls==NULL)
 {
  c=TK_EOS;
  lua_pushinteger(L,0);
 }
 else
 {
  c=llex(ls,&seminfo);
  lua_pushinteger(L,ls->linenumber);
 }
 if (c<FIRST_RESERVED)
 {
  char s=c;
  lua_pushlstring(L,&s,1);
 }
 else
  lua_pushstring(L,luaX_tokens[c-FIRST_RESERVED]);
 if (c==TK_NAME || c==TK_STRING)
  lua_pushlstring(L,getstr(seminfo.ts),seminfo.ts->tsv.len);
 else if (c==TK_NUMBER)
  lua_pushnumber(L,seminfo.r);
 else
  lua_pushnil(L);
 if (c==TK_EOS && ls!=NULL)
 {
  lua_pushnil(L);
  lua_replace(L,lua_upvalueindex(1));
 }
 return 3;
}

void luaX_setinput(lua_State *L, LexState *ls, ZIO *z, TString *source)
{
 setinput(L,ls,z,source);
 lua_getglobal(L,FILTER);
 if (lua_isnil(L,-1))
  lua_pop(L,1);
 else
 {
  lua_pushlightuserdata(L,ls);
  lua_pushcclosure(L,gettoken,1);
  lua_pushlstring(L,getstr(source),source->tsv.len);
  lua_call(L,2,0);
 }
}

static int nexttoken(LexState *ls, SemInfo *seminfo)
{
 lua_State *L=ls->L;
 lua_getglobal(L,FILTER);
 if (lua_isnil(L,-1))
 {
  lua_pop(L,1);
  return llex(ls,seminfo);
 }
 else
 {
  int c;
  const char *token;
  lua_call(L,0,3);
  token=lua_tostring(L,-2);
  if (token==NULL)
   c=TK_EOS;
  else
  {
   int i;
   c=-1;
   for (i=0; luaX_tokens[i]; i++)
   {
    if (strcmp(token,luaX_tokens[i])==0)
    {
     c=i+FIRST_RESERVED;
     break;
    }
   }
   if (c==-1)
    c=token[0];
   else if (c==TK_NAME || c==TK_STRING)
   {
    if (lua_tostring(L,-1)==NULL) 
     seminfo->ts=luaS_newliteral(L,"?");
    else
     seminfo->ts=luaS_newlstr(L,lua_tostring(L,-1),lua_strlen(L,-1));
   }
   else if (c==TK_NUMBER)
    seminfo->r=lua_tonumber(L,-1);
  }
  ls->linenumber=lua_tointeger(L,-3);
  lua_pop(L,3);
  return c;
 }
}

#define llex nexttoken
