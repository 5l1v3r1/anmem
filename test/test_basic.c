#include <stdio.h>
#include <stdlib.h> // posix_memalign
#include <assert.h>
#include <anmem/config.h>
#include <anmem/alloc.h>

static anmem_t mem;
static anmem_section_t sections[3];
static void * buffer;

void test_initialize();
void test_alloc_pages();
void test_alloc_aligned();
void test_alloc_pages_overflow();

int main() {
  test_initialize();
  test_alloc_pages();
  test_alloc_aligned();
  test_alloc_pages_overflow();
  return 0;
}

void test_initialize() {
  printf("testing initialization...");
  
  // allocate 128K of aligned memory
  posix_memalign(&buffer, 0x20000, 0x20000);
  
  uint64_t firstPage = ((uint64_t)buffer) >> 12;
  uint64_t sizes[3] = {firstPage, 0x1, 0x1f};
  
  anmem_config_t config;
  config.structs = sizes;
  config.sizeOffset = 0;
  config.structSize = 8;
  config.structCount = 3;
  
  mem.allocators = sections;
  mem.maximum = 3;
  mem.count = 0;
  
  // grab 8 pages of anlock
  bool result = anmem_configure(&config, &mem, 3, firstPage);
  assert(result);
  
  assert(mem.count == 3);
  assert(sections[0].type == 1);
  assert(sections[1].type == 0);
  assert(sections[2].type == 0);
  
  assert(sections[0].start == 8 + firstPage);
  assert(sections[0].len == 8);
  
  assert(sections[1].start == firstPage);
  assert(sections[1].len == 8);
  assert(sections[2].start == 0x10 + firstPage);
  assert(sections[2].len == 0x10);
  
  printf(" passed!\n");
}

void test_alloc_pages() {
  printf("testing anmem_alloc_page()...");
  
  uint64_t firstPage = ((uint64_t)buffer) >> 12;
  
  // allocating 0xf pages should all be from the top allocator
  uint64_t i;
  for (i = 0; i < 0xf; i++) {
    uint64_t page = ((uint64_t)anmem_alloc_page(&mem)) >> 12;
    assert(page == firstPage + 0x1f - i);
  }
  
  // allocating the next 7 pages should be from the first allocator
  for (i = 0; i < 7; i++) {
    uint64_t page = ((uint64_t)anmem_alloc_page(&mem)) >> 12;
    assert(page == firstPage + 7 - i);
  }
  
  printf(" passed!\n");
}

void test_alloc_aligned() {
  
}

void test_alloc_pages_overflow() {
  
}
