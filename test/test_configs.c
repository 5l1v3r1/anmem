#include <stdio.h>
#include <assert.h>
#include <anmem/config.h>

void test_high_only();

int main() {
  test_high_only();
  return 0;
}

void test_high_only() {
  printf("testing >4GB only...");
  
  // there's more than enough memory, but it's all above 4GB (boohoo for you)
  uint64_t regions[3] = {0x100, 0xFFF00, 0x100000};
  anmem_section_t sections[1];
  
  anmem_config_t config;
  config.structs = regions;
  config.sizeOffset = 0;
  config.structSize = 8;
  config.structCount = 3;
  
  anmem_t mem;
  mem.allocators = sections;
  mem.count = 0;
  mem.maximum = 1;
  
  // only requesting two pages of analloc, but it's above the limit
  bool result = anmem_configure(&config, &mem, 2, 0x100000);
  assert(result);
  
  assert(mem.count == 1);
  assert(mem.allocators[0].type == 0);
  assert(mem.allocators[0].start == 0x100000);
  assert(mem.allocators[0].len == 0x100000);
  
  printf(" passed\n");
}
