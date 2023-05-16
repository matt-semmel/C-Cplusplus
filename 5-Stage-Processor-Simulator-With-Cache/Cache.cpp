#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <iostream>

#include "Cache.h"
#include "CPU.h"


Cache::Cache(const char *name)
: MemObj(name)
  ,readHits("readHits")
  ,readMisses("readMisses")
  ,writeHits("writeHits")
  ,writeMisses("writeMisses")
  ,writeBacks("writeBacks")
{
  GError *error = NULL;
  // Get hit delay from config file
  hitDelay = g_key_file_get_integer(config->keyfile, name, "hitDelay", NULL);
  if(error != NULL) g_error (error->message);
  // Create cacheCore after parsing config file
  int size = g_key_file_get_integer(config->keyfile, name, "size", &error);
  if(error != NULL) g_error (error->message);
  int assoc = g_key_file_get_integer(config->keyfile, name, "assoc", NULL);
  if(error != NULL) g_error (error->message);
  int bsize = g_key_file_get_integer(config->keyfile, name, "bsize", NULL);
  if(error != NULL) g_error (error->message);
  gchar* pStr = g_key_file_get_string(config->keyfile, name, "replPolicy", NULL);
  if(error != NULL) g_error (error->message);

  assert(size > 0);
  assert(assoc > 0);
  assert(bsize > 0);
  assert(pStr != NULL);

  cacheCore = new CacheCore(size, assoc, bsize, pStr);

  g_free(pStr);
}

Cache::~Cache()
{
  delete cacheCore;
}

void Cache::access(MemRequest *mreq)
{
  mreq->addLatency(hitDelay);

  if(verbose) {
    const char *memOp;
    switch(mreq->getMemOperation()){
      case MemRead: memOp = "MemRead"; break;
      case MemWrite: memOp = "MemWrite"; break;
      case MemWriteBack: memOp = "MemWriteBack"; break;
      default: assert(0); break;
    }
    printf("%s->access(%s, addr: %u, latency: %u)\n", getName().c_str(), memOp, mreq->getAddr(), mreq->getLatency());
  }

  switch(mreq->getMemOperation()){
    case MemRead:
      read(mreq);
      break;
    case MemWrite:
      write(mreq);
      break;
    case MemWriteBack:
      writeBack(mreq);
      break;
    default:
      assert(0);
      break;
  }
}

// Get string that describes MemObj
std::string Cache::toString() const
{
  std::string ret;
  ret += "[" + getName() + "]\n";
  ret += "device type = cache\n";
  ret += "write policy = " + getWritePolicy() + "\n";
  ret += "hit time = " + std::to_string(hitDelay) + "\n";
  ret += cacheCore->toString();
  ret += "lower level = " + getLowerLevel() + "\n";
  return ret;
}

// Get string that summarizes access statistics
std::string Cache::getStatString() const
{
  std::string ret;
  ret += getName() + ":";
  ret += readHits.toString() + ":";
  ret += readMisses.toString() + ":";
  ret += writeHits.toString() + ":";
  ret += writeMisses.toString() + ":";
  ret += writeBacks.toString();
  return ret;
}

// Get string that dumps all valid lines in cache
std::string Cache::getContentString() const
{
  std::string ret;
  ret += "[" + getName() + "]\n";
  ret += cacheCore->getContentString();
  return ret;
}

// WBCache: Write back cache.  Allocates a dirty block on write miss.

WBCache::WBCache(const char *name)
: Cache(name)
{
  // nothing to do
}
WBCache::~WBCache() 
{
  // nothing to do
}

// TODO: Done
void WBCache::read( MemRequest *mreq)
{
  uint32_t Addr; // eax
  const CacheLine *l; // [rsp+18h] [rbp-8h]

  Addr = mreq->getAddr();
  if ( cacheCore->accessLine(Addr) )
  {
    readHits.inc();
  }
  else
  {
    readMisses.inc();
    getLowerLevelMemObj()->access(mreq); 
    l = allocateLine(mreq->getAddr());
    if ( !l || !l->isValid() )
      __assert_fail("l && l->isValid()", "Cache.cpp", 0x95u, "virtual void WBCache::read(MemRequest*)");
  }
}

// TODO: NEEDS WORK
void WBCache::write(MemRequest *mreq)
{
  uint32_t Addr; // eax
  CacheLine *l; // [rsp+18h] [rbp-8h]
  CacheLine *la; // [rsp+18h] [rbp-8h]

  Addr = mreq->getAddr();
  l = cacheCore->accessLine(Addr);
  if ( l )
  {
    writeHits.inc();
    l->makeDirty();
  }
  else
  {
    writeMisses.inc();
    mreq->mutateWriteToRead(); 
    getLowerLevelMemObj()->access(mreq); 
    la = allocateLine(mreq->getAddr());
    if ( !la || !la->isValid() )
      __assert_fail("l && l->isValid()", "Cache.cpp", 0xA6u, "virtual void WBCache::write(MemRequest*)");
    la->makeDirty();
  }
}




// TODO: Done
void WBCache::writeBack(MemRequest *mreq)
{
  uint32_t Addr; // eax
  CacheLine *l; // [rsp+18h] [rbp-8h]

  Addr =mreq->getAddr();
  l = cacheCore->accessLine( Addr);
  if ( l )
  {
    if ( !l->isValid() )
      __assert_fail("l->isValid()", "Cache.cpp", 0xB4u, "virtual void WBCache::writeBack(MemRequest*)");
    l->makeDirty();
  }
  else
  {
    getLowerLevelMemObj()->access(mreq); //! DEFAULT
  }
}


// WTCache: Write through cache. Always propagates writes down.

WTCache::WTCache(const char *name)
: Cache(name)
{
  // nothing to do 
}

WTCache::~WTCache()
{
  // nothing to do
}

// TODO: DONE
void WTCache::read(MemRequest *mreq)
{
  uint32_t Addr; // eax
  uint32_t rplcAddr; // [rsp+1Ch] [rbp-14h] BYREF
  CacheLine *l; // [rsp+20h] [rbp-10h]

  Addr = mreq->getAddr();
  if ( cacheCore->accessLine( Addr) )
  {
    readHits.inc();
  }
  else
  {
    readMisses.inc();
    getLowerLevelMemObj()->access(mreq); //! DEFAULT
    rplcAddr = 0;
    l = cacheCore->allocateLine(mreq->getAddr(), &rplcAddr);
    if ( !l || !l->isValid() || rplcAddr )
      __assert_fail("l && l->isValid() && rplcAddr == 0", "Cache.cpp", 0xD4u, "virtual void WTCache::read(MemRequest*)");
  }
}


// TODO: DONE
void WTCache::write(MemRequest *mreq)
{
  if (cacheCore->accessLine(mreq->getAddr()))
    writeHits.inc();
  else
    writeMisses.inc(); 
  getLowerLevelMemObj()->access(mreq);
}

void WTCache::writeBack(MemRequest *mreq)
{
  // No reasonable design will have a WB cache on top of a WT cache
  assert(0);
}

// TODO: DONE
CacheLine* WBCache::allocateLine(unsigned int addr){ //add to header!
  unsigned int rplcAddr = 0;
  CacheLine * l;
  MemRequest *mreq;

  l = cacheCore->allocateLine(addr, &rplcAddr);
  if (!l || !l->isValid())
    __assert_fail("l && l->isValid()", "Cache.cpp", 0x80u, "CacheLine* WBCache::allocateLine(uint32_t)");
  if (rplcAddr)
  {
    writeBacks.inc();
    mreq = new MemRequest(rplcAddr,MemWriteBack);
    getLowerLevelMemObj()->access(mreq);
    delete mreq;
  }
  return l;
}