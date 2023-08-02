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

// Changed
// 为每一个CPU生成一个freelist 对应一个自旋锁
struct {
  struct spinlock lock;
  struct run *freelist;
} kmem[NCPU];

void
kinit()
{
  // Changed
  // 初始化所有锁
  // initlock(&kmem.lock, "kmem");
  for(int i=0;i<NCPU;i++){
    char str[10];
    snprintf(str, 9, "kmem %d", i);
    initlock(&kmem[i].lock, str);
  }

  freerange(end, (void*)PHYSTOP);
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
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  /** 获取 cpuid 时要关中断 然后再打开 */
  push_off();
  int id = cpuid();
  pop_off();

  // Changed
  // 对每个CPU的链表进行free
  acquire(&kmem[id].lock);
  r->next = kmem[id].freelist;
  kmem[id].freelist = r;
  release(&kmem[id].lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;
  // NEW
  // 按照提示获取cpuid 并在前后执行开关中断的操作
  push_off();
  int id = cpuid();
  pop_off(); 

  /*尝试获取剩余空闲空间page*/
  acquire(&kmem[id].lock);
  r = kmem[id].freelist;
  if(r)
    kmem[id].freelist = r->next;
  else{
    for(int i=0;i<NCPU;i++){
      if(i == id) // 偷page时跳过自身
        continue;
      
      /*尝试偷一个其它CPU的空闲page*/
      acquire(&kmem[i].lock);
      if(!kmem[i].freelist){
        release(&kmem[i].lock);
        continue;
      }

      r = kmem[i].freelist;
      kmem[i].freelist = r->next;
      release(&kmem[i].lock);
      break;
    }
  }
  release(&kmem[id].lock);

  /*第id个CPU没有空闲page 也没偷到*/ 
  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
