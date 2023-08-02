// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

struct {
  // struct spinlock lock;
  struct buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  // NEW
  struct buf buckets[NBUCKET];
  struct spinlock lks[NBUCKET];
} bcache;

void
binit(void)
{
  struct buf *b;

  // Changed
  // 初始化每个桶的锁 
  for(int i=0;i<NBUCKET;i++)
  {
    char str[10];
    snprintf(str, 9, "bcache %d", i);
    initlock(&bcache.lks[i], str);
  }

  // initlock(&bcache.lock, "bcache");

  // Create linked list of buffers
  // Changed
  // 仍需保持哈希桶双链表的特性 prev和next均指向自己
  for(int i=0;i<NBUCKET;i++){
    bcache.buckets[i].prev = &bcache.buckets[i];
    bcache.buckets[i].next = &bcache.buckets[i];
  }
  
  for(b = bcache.buf; b < bcache.buf+NBUF; b++){
    b->next = bcache.buckets[0].next;
    b->prev = &bcache.buckets[0];
    initsleeplock(&b->lock, "buffer");
    bcache.buckets[0].next->prev = b;
    bcache.buckets[0].next = b;
  }
}

// NEW
// 抽象一个bufinit 用于覆盖ticks最小的buf
static void
bufinit(struct buf* b, uint dev, uint blockno)
{
  b->dev = dev;
  b->blockno = blockno;
  b->valid = 0;
  b->refcnt = 1;
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;

  // Changed
  int id = myhash(b->blockno); // 计算出哈希值 挂载到相应的桶

  acquire(&bcache.lks[id]);

  // Is the block already cached?
  for(b = bcache.buckets[id].next; b != &bcache.buckets[id]; b = b->next){
    if(b->dev == dev && b->blockno == blockno){
      b->refcnt++;
      release(&bcache.lks[id]);
      acquiresleep(&b->lock);
      return b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  // 但是此处应采用ticks最小的值淘汰
  struct buf *victim = 0; // 选择要淘汰的buf
  uint minticks = ticks; // 记录最小的ticks

  /*找出最小ticks对应的buf*/
  for(b = bcache.buckets[id].next; b != &bcache.buckets[id]; b = b->next){
    if(b->refcnt == dev && b->lastuse <= minticks){
      minticks = b->lastuse;
      victim = b;
    }
  }

  if(victim){
    /* 找到了相应的buf 直接将其初始化(覆盖) */
    bufinit(victim, dev, blockno);
    release(&bcache.lks[id]);
    acquiresleep(&victm->lock);
    return victim;
  }
  else{
    /* 到别的哈希桶去偷一个buf */
    for(int i=0;i<NBUCKET;i++){
      if(i == id) // 跳过自己
        continue;
      
      // 重复之前的操作 找到最小的ticks对应的buf
      acquire(&bcache.lks[i]);
      minticks = ticks;
      for(b = bcache.buckets[i].next; b != &bcache.buckets[i]; b = b->next){
        if(b->refcnt==0 && b->lastuse<=minticks) {
          minticks = b->lastuse;
          victm = b;
        }
      }
      if(victim){
        bufinit(victim,dev,blockno);

        /* 将victim从bucket中取出来 保持双链表的结构完整 */
        victim->next->prev = victim->prev;
        victim->prev->next = victim->next;
        release(&bcache.lks[i]);

        /* 将victim放到第id个bucket中 保持双链表的稳定性 */
        victim->next = bcache.buckets[id].next;
        bcache.buckets[id].next->prev = victim;
        bcache.buckets[id].next = victim;
        victim->prev = &bcache.buckets[id];

        release(&bcache.lks[id]);
        acquiresleep(&victm->lock); // 主动让出CPU
        return victm;
      }
    }
  }

  release(&bcache.lks[id]);
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// NEW
// 返回对应桶的哈希值
static int
myhash(int x)
{
  return x%NBUCKET;
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  // Changed
  int id = myhash(b->blockno); // 计算出哈希值 挂载到相应的桶

  acquire(&bcache.lks[id]);
  b->refcnt--;
  if (b->refcnt == 0)
    b->lastuse = ticks;
  
  release(&bcache.lks[id]);
}

void
bpin(struct buf *b) {
  // Changed
  int id = myhash(b->blockno); // 计算出哈希值 挂载到相应的桶
  acquire(&bcache.lks[id]);
  b->refcnt++;
  release(&bcache.lks[id]);
}

void
bunpin(struct buf *b) {
  // Changed
  int id = myhash(b->blockno); // 计算出哈希值 挂载到相应的桶
  acquire(&bcache.lks[id]);
  b->refcnt--;
  release(&bcache.lks[id]);
}


