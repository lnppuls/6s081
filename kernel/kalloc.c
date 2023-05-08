// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

struct cow_cnt{
  struct spinlock lock;
  int cnt;
};

struct cow_cnt cow_cnt[(PHYSTOP - KERNBASE)/PGSIZE];

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
  for(int i = 0;i < (PHYSTOP-KERNBASE)/PGSIZE;i++){
    initlock(&cow_cnt[i].lock,"cow");
  }
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  int num = pa2num((uint64)pa);
  if(num == -1)
    panic("kfree");
  if(cow_cnt[num].cnt == 0){
    struct run *r;

    if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
      panic("kfree");

    // Fill with junk to catch dangling refs.
    memset(pa, 1, PGSIZE);

    r = (struct run*)pa;

    acquire(&kmem.lock);
    r->next = kmem.freelist;
    kmem.freelist = r;
    release(&kmem.lock);
  }
  else{
    cow_add((uint64)pa,-1);
  }
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);
  if(r){  
    uint64 pa = (uint64)r;
    int num = pa2num(pa);
    acquire(&cow_cnt[num].lock);
    cow_cnt[num].cnt = 1;
    release(&cow_cnt[num].lock);
  }
  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
  
}

int pa2num(uint64 pa){
  if(pa > PHYSTOP)
    return -1;
  if(pa < KERNBASE)
   return -1;
  int num = (pa - KERNBASE)/PGSIZE;
  int end = (PHYSTOP-KERNBASE)/PGSIZE;
  if(num > end)
    return -1;
  return num;
}

int cow_add(uint64 pa,int size){
  int num = pa2num(pa);
  if(num == -1)
    return -1;
  if(num < 0)
    return -1;
  acquire(&cow_cnt[num].lock);
  cow_cnt[num].cnt += size;
  if(cow_cnt[num].cnt < 0)
   return -1;
  release(&cow_cnt[num].lock);
  return 1;
}