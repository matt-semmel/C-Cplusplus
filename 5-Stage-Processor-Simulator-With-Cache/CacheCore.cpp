#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <assert.h>

#include "CacheCore.h"

CacheCore::CacheCore(uint32_t s, uint32_t a, uint32_t b, const char *pStr)
  : size(s)
  ,lineSize(b)
  ,assoc(a)
  ,numLines(s/b)
{
  if (strcasecmp(pStr, "RANDOM") == 0)
    policy = RANDOM;
  else if (strcasecmp(pStr, "LRU") == 0)
    policy = LRU;
  else {
    assert(0);
  }

  content = new CacheLine[numLines + 1];

  for(uint32_t i = 0; i < numLines; i++) {
    content[i].initialize();
  }
}

CacheCore::~CacheCore() {
  delete [] content;
}

// TODO: Implement
CacheLine *CacheCore::accessLine(uint32_t addr)
{
  CacheLine *l;
  uint32_t tag;
  uint32_t i;
  CacheLine *lineHit = NULL;

  tag = calcTag4Addr(addr);
  for(i = calcIndex4Addr(addr); i < assoc+calcIndex4Addr(addr); i++) {
    l = &content[i];
    l->incAge();
    if (l->isValid() && l->getTag() == tag){
      lineHit = l;
      l->resetAge();
    }
  }
  return lineHit;
}




// TODO: Implement
CacheLine *CacheCore::allocateLine(uint32_t addr, uint32_t *rplcAddr)
{
  uint32_t Age; // ebx
  CacheLine *l_0; // [rsp+20h] [rbp-40h]
  CacheLine *l; // [rsp+28h] [rbp-38h]
  uint32_t tag; // [rsp+34h] [rbp-2Ch]
  uint32_t i_0; // [rsp+38h] [rbp-28h]
  uint32_t indexOldest; // [rsp+3Ch] [rbp-24h]
  CacheLine *lineOldest; // [rsp+40h] [rbp-20h]
  uint32_t i; // [rsp+4Ch] [rbp-14h]

  tag = calcTag4Addr(addr);
  for ( i = calcIndex4Addr(addr); assoc + calcIndex4Addr(addr) > i; ++i )
  {
    l = &content[i];
    if ( !l->isValid() )
    {
      l->initialize();
      l->validate();
      l->setTag(tag);
      return l;
    }
  }
  lineOldest = 0LL;
  indexOldest = 0;
  for ( i_0 = calcIndex4Addr(addr); assoc + calcIndex4Addr(addr) > i_0; ++i_0 )
  {
    l_0 = &content[i_0];
    if ( !l_0->isValid() )
      __assert_fail("l->isValid()", "CacheCore.cpp", 0x4Bu, "CacheLine* CacheCore::allocateLine(uint32_t, uint32_t*)");
    if ( lineOldest )
    {
      Age = lineOldest->getAge();
      if ( Age >= l_0->getAge() )
        continue;
    }
    lineOldest = l_0;
    indexOldest = i_0;
  }
  if ( !rplcAddr )
    __assert_fail("rplcAddr", "CacheCore.cpp", 0x53u, "CacheLine* CacheCore::allocateLine(uint32_t, uint32_t*)");
  if ( lineOldest->isDirty() )
  {
    *rplcAddr = calcAddr(lineOldest->getTag(), indexOldest);
  }
  lineOldest->initialize();
  lineOldest->validate();
  lineOldest->setTag(tag);
  return lineOldest;
}

