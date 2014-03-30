#include <anmem/config.h>

#define PAGE_INVAL 0xffffffffffffffffL

bool anmem_configure(anmem_config_t * config,
                     anmem_t * mem,
                     uint64_t maxControllable,
                     uint64_t pageSkip) {
  // jesus, I am not looking forward to this
  
  // find where to put the controllable regions, and place them in the
  // resultant array.
  
  // fill the gaps with page allocators.
}
