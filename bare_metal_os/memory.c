#include <stddef.h>
#include <stdint.h>

typedef struct Block {
    size_t size;
    struct Block *next;
    int free;
} Block;

static uint8_t heap_area[4096];
static Block *heap_base;
static Block *free_list;
static size_t heap_total;

void mem_init_bare(void) {
    heap_base = (Block*)heap_area;
    heap_total = sizeof(heap_area);
    heap_base->size = heap_total - sizeof(Block);
    heap_base->next = NULL;
    heap_base->free = 1;
    free_list = heap_base;
}

static void split_block(Block *b,size_t size){
    if(b->size >= size + sizeof(Block) + 8){
        Block *n=(Block*)((uint8_t*)(b+1)+size);
        n->size = b->size - size - sizeof(Block);
        n->next = b->next;
        n->free = 1;
        b->next = n;
        b->size = size;
    }
}

void *mem_alloc(size_t size){
    if(size==0) return NULL;
    if(size % 8) size += 8 - (size % 8);
    for(Block *b=free_list;b;b=b->next){
        if(b->free && b->size>=size){
            split_block(b,size);
            b->free=0;
            return b+1;
        }
    }
    return NULL;
}

void mem_free(void *ptr){
    if(!ptr) return;
    Block *b=(Block*)ptr-1;
    b->free=1;
    // coalesce
    for(Block *c=free_list;c && c->next;){
        Block *n=c->next;
        if(c->free && n->free && (uint8_t*)(c+1)+c->size==(uint8_t*)n){
            c->size += sizeof(Block)+n->size;
            c->next = n->next;
            continue;
        }
        c=c->next;
    }
}

void mem_reset(void){
    heap_base->size = heap_total - sizeof(Block);
    heap_base->next = NULL;
    heap_base->free = 1;
    free_list = heap_base;
}
