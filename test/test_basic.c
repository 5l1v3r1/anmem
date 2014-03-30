#include <stdio.h>
#include <stdlib.h> // posix_memalign
#include <assert.h>
#include <sys/mman.h>
#include <anmem/config.h>
#include <anmem/alloc.h>

static anmem_t mem;
static anmem_section_t sections[3];
static void * buffer;

static void * allocate_buffer();
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

static void * allocate_buffer() {
  // allocate 256K and find a 128K aligned part of it
  
  // only works on linux, which is just as well.
  void * p = mmap(0, 0x40000, PROT_READ | PROT_WRITE,
                  MAP_32BIT | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  uint64_t num = (uint64_t)p;
  assert(num < 0x80000000L);
  if (num % 0x20000) {
    num += 0x20000 - (num % 0x20000);
  }
  return (void *)num;
}

void test_initialize() {
  buffer = allocate_buffer();
  printf("buffer is at %p\n", buffer);
  
  uint64_t firstPage = ((uint64_t)buffer) >> 12;
  uint64_t sizes[2] = {firstPage, 0x20};
  
  anmem_config_t config;
  config.structs = sizes;
  config.sizeOffset = 0;
  config.structSize = 8;
  config.structCount = 2;
  
  mem.allocators = sections;
  mem.maximum = 3;
  mem.count = 0;
  
  // grab 8 pages of anlock
  bool result = anmem_configure(&config, &mem, 3, firstPage);
  assert(result);
  
  assert(mem.count == 3);
  assert(sections[0].type == 0);
  assert(sections[1].type == 0);
  assert(sections[2].type == 0);
  assert(sections[0].start == firstPage);
  assert(sections[0].len == 8);
  assert(sections[1].start == 8);
  assert(sections[1].len == 8);
  assert(sections[2].start == 0x10);
  assert(sections[2].len == 0x10);
}

void test_alloc_pages() {
  
}

void test_alloc_aligned() {
  
}

void test_alloc_pages_overflow() {
  
}
