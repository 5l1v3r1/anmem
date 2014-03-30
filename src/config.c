#include <anmem/config.h>
#include <analloc.h>
#include <anpages.h>

#define PAGE_INVAL 0xffffffffffffffffL

static bool _create_controllable(anmem_config_t * config,
                                 anmem_t * mem,
                                 uint64_t pagesExp,
                                 uint64_t pageSkip);
static bool _add_allocator(anmem_t * mem,
                           uint64_t start,
                           uint64_t len,
                           uint64_t type);
static uint64_t _sum_regions(anmem_config_t * config);
static uint64_t _next_analloc(anmem_t * mem, uint64_t start, uint64_t * len);

bool anmem_configure(anmem_config_t * config,
                     anmem_t * mem,
                     uint64_t maxControllable,
                     uint64_t pageSkip) {
  if (!mem->maximum) return false;
  bool result = _create_controllable(config, mem, maxControllable, pageSkip);
  
  uint64_t allocs = 0;
  uint64_t sum = _sum_regions(config);
  bool hadToCut = false;
  
  // NOTE: anpages cannot be used on single-page regions.
  
  do {
    // calculate the number of anpages_t we will need
    uint64_t page = pageSkip;
    while (page + 1 < sum) {
      uint64_t len;
      uint64_t next = _next_analloc(mem, page, &len);
      if (!(next + 1)) {
        allocs++;
        break;
      }
      if (next - page > 1) allocs++;
      page = next + len;
    }
    
    if (allocs + mem->count > mem->maximum) {
      mem->count--;
      hadToCut = true;
    }
  } while (allocs + mem->count > mem->maximum);
  
  // go through and create the allocators
  uint64_t startPage = 0;
  
  while (startPage + 1 < sum) {
    uint64_t len;
    uint64_t next = _next_analloc(mem, page, &len);
    if (!(next + 1)) {
      // fill up the rest of the memory
      _add_allocator(mem, startPage, sum - startPage, 0);
      break;
    }
    
    if (next - startPage > 1) {
      _add_allocator(mem, startPage, next - startPage, 0);
    }
    
    startPage = next + len;
  }
  
  return result && !hadToCut;
}

static bool _create_controllable(anmem_config_t * config,
                                 anmem_t * mem,
                                 uint64_t pagesExp,
                                 uint64_t pageSkip) {
  uint64_t i, curPage = 0;
  void * structs = config->structs;
  
  uint64_t grabSize = 1 << pagesExp;
  uint64_t sizeRemaining = grabSize;
  while (grabSize >= 0x4 && sizeRemaining > 0) {
    for (i = 0; i < config->structCount && sizeRemaining > 0; i++) {
      // read the structure
      uint64_t fullSize = *((uint64_t *)(structs + config->sizeOffset));
      structs += config->structSize;
      // calculate the available bounds
      uint64_t lowerBound = curPage, upperBound = curPage + fullSize;
      if (upperBound > 0x100000) {
        upperBound = 0x100000;
      } else if (lowerBound < pageSkip) {
        lowerBound = pageSkip;
      }
      if (lowerBound >= 0x100000) break;
      curPage += fullSize;
      // while there are pages left to grab, grab available aligned regions
      while (sizeRemaining) {
        // make sure there is a possibility of finding a region
        if (lowerBound >= upperBound) break;
        if (upperBound - lowerBound < grabSize) break;
        // look for an aligned address between lowerBound and upperBound
        if (lowerBound % grabSize) {
          uint64_t nextBound = lowerBound + (lowerBound % grabSize);
          if (nextBound + grabSize > upperBound) break;
          
          // we found a region for our grab size
          if (!_add_allocator(mem, nextBound, grabSize, 1)) {
            return false;
          }
          
          lowerBound = nextBound + grabSize;
          sizeRemaining -= grabSize;
        } else {
          // we found a region for our grab size
          if (!_add_allocator(mem, lowerBound, grabSize, 1)) {
            return false;
          }
          
          lowerBound += grabSize;
          sizeRemaining -= grabSize;
        }
      }
    }
    grabSize >>= 1;
  }
  
  return true;
}

static bool _add_allocator(anmem_t * mem,
                           uint64_t start,
                           uint64_t len,
                           uint64_t type) {
  if (mem->count == maximum) return false;
  mem->allocators[mem->count].type = type;
  mem->allocators[mem->count].start = start;
  mem->allocators[mem->count].len = len;
  if (type == 0) { // anpages
    anpages_t pages = &mem->allocators[mem->count].anpagesRoot;
    if (!anpages_initialize(pages, start, len)) {
      return false;
    }
  } else if (type == 1) { // analloc
    void * buffer = start << 12;
    analloc_t alloc = &mem->allocators[mem->count].anallocRoot;
    if (!analloc_with_chunk(alloc, len << 12, 0, 0x1000)) {
      return false;
    }
  }
  mem->count++;
  return true;
}

static uint64_t _sum_regions(anmem_config_t * config) {
  uint64_t i, curPage = 0;
  void * structs = config->structs;
  for (i = 0; i < config->structCount && sizeRemaining > 0; i++) {
    // read the structure
    uint64_t fullSize = *((uint64_t *)(structs + config->sizeOffset));
    structs += config->structSize;
    curPage += fullSize;
  }
  return curPage;
}

static uint64_t _next_analloc(anmem_t * mem, uint64_t start, uint64_t * len) {
  uint64_t i, firstPlace = 0xffffffffffffffffL;
  for (i = 0; i < mem->count; i++) {
    if (mem->allocators[i].start < firstPlace
        && mem->allocators[i].start >= start) {
      firstPlace = mem->allocators[i].start;
      *len = mem->allocators[i].len;
    }
  }
  return firstPlace;
}
