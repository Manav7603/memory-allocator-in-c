#include "malloc.h"
#include <stdint.h>  // SIZE_MAX
#include <string.h> // memset, memcpy
// #include <unistd.h>  // sbrk
#include <sys/mman.h>  // mmap

// The first block in our heap managed linked list
// NULL means no allocation till now

block_header_t *heap_start = NULL;

#define ALIGNMENT 8
#define MIN_SPLIT_SIZE 8

// Round a size up to the nearest multiple of 8
// This help allocations alligned
static size_t align8(size_t size)
{
    return (size + (ALIGNMENT - 1)) & ~((size_t)(ALIGNMENT - 1));

}

// Return the last block in the heap linked list
static block_header_t *get_last_block(void)
{
    block_header_t *curr =heap_start;

    if(curr==NULL)
    {
        return NULL;
    }

    while(curr->next != NULL)
    {
        curr =curr->next;
    }

    return curr;
}

// Earlier Version // Finding the first free block that satisfy the request i.e 1st free block whose size is greater than the requested size
// Earlier Version // For now making it first fit strategy

// Finding the smallest free block that satisfy the request
// Collective Best-fit strategy: finds the free block with the smallest size that still fits
// This reduces fragmentation by leaving larger blocks for future allocations

static block_header_t *find_free_block (size_t size)
{
    block_header_t *curr = heap_start;
    block_header_t *best_fit = NULL;
    size_t best_size = SIZE_MAX;
    
    while(curr != NULL)
    {
        if(curr->is_free && curr->size >= size)
        {
            // return curr; // For First Fit
            if(curr->size == size)
            {
                return curr;
            }

            if(curr->size < best_size)
            {
                best_fit = curr;
                best_size = curr->size;
            }
        }
        curr = curr->next;
    }
    return best_fit;
}

// Asking the "OS" for more heap memory using mmap
// Returned block contains both the header and the user region

static block_header_t *request_space(block_header_t *last,size_t size)
{

    // In sbrk we did (sbrk(sizeof(block_header_t)+size));
    void * block_mem = mmap(NULL, sizeof(block_header_t)+size,
                           PROT_READ | PROT_WRITE,
                           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if(block_mem == MAP_FAILED) // In sbrk we did ((void *)-1)
    {
        return NULL;
    }

    block_header_t * block = (block_header_t *)block_mem;
    block-> size=size;
    block-> is_free=0;
    block->next =NULL;
    block-> prev =last;

    if(last != NULL)
    {
        last->next=block;
    }
    return block;
 
}

// Splitting a large free block into : 
// - 1 block of requested size
// - Left over as free block

// Splitting will happen only of the requested size large enough ti hold another header plus MIN_SPLIT_SIZE Bytes;

static void split_block(block_header_t *block, size_t size)
{
    if(block==NULL)
    {
         return;
    }

    // Splinter prevention, if after splitting it is so small that a header also couldn't be inserted than it is of no use, hence it will return without splitting
    if(block->size < size + sizeof(block_header_t) + MIN_SPLIT_SIZE)
    {
        return;
    }

    block_header_t *new_block = (block_header_t *)((char *)(block+1) +size); 
    // Converted to char to make compiler understand not to follow pointer arithmetic

    new_block->size=block->size -size -sizeof(block_header_t);
    new_block->is_free=1;
    new_block->next=block->next;
    new_block->prev=block;

    if(new_block->next != NULL)
    {
        new_block->next->prev = new_block;
    }

    block->next=new_block;
    block->size=size;
    
}

// IMP: The original block is shrinking to fit the memory requested by user. So the new block which we are making will be of free space will be at higer memory addresses. 
// Lower Memory Addresses -----------------------------> Higher Memory Addresses
// [ Original Header ] [ User Payload ] | [ New Header ] [ Leftover Free Space ]
// ^^^^^^^^^^^^^^^^^^^
// Stays exactly where it is. For minimum changes


// Coalesce
// Merging a free block with nearby free block 
// This reduces the fragmentation and keeps memory reusable

static void coalesce(block_header_t * block)
{
    if(block==NULL)
    {
        return;
    }

    // Merge with next free block if possible
    if(block->next != NULL && block->next->is_free)
    {
        block_header_t *next=block->next;
        block->size+= ( sizeof(block_header_t)+next->size);
        block->next =next->next;

        if(block->next !=NULL)
        {
            block->next->prev =block;
        }
    }

    // Merge with previous free block if possible
    
    if(block->prev != NULL && block->prev->is_free)
    {
        block_header_t * prev =block->prev;
        prev->size+= sizeof(block_header_t) +block->size;
        prev->next= block->next;

        if(prev->next !=NULL)
        {
            prev->next->prev =prev;
        }

        block=prev;


        // After merging backward let's also cehck if it can again merge forward

        if(block->next != NULL && block->next->is_free)
        {
            block_header_t *next=block->next;
            block->size+= ( sizeof(block_header_t)+next->size);
            block->next =next->next;

            if(block->next !=NULL)
            {
                block->next->prev =block;
            }
        }
    }

}


// The main program will call this 'my_malloc' function

void * my_malloc(size_t size)
{
    block_header_t * block;
    size_t aligned_size;
    if(size ==0)
    {
        return NULL;
    }

    aligned_size=align8(size);

    // 1st allocation: Create the initial block if it the list is empty

    if( heap_start ==NULL)
    {
        block= request_space(NULL, aligned_size);
        if(block == NULL)
        {
            return NULL;
        }

        heap_start = block;

        return (void *)(block +1);
    }

    // If the list if alreay there:
    // Trying to reuse the exsisting free blocks

    block = find_free_block(aligned_size);
    if(block != NULL)
    {
        split_block(block,aligned_size);
        block->is_free=0;
        return (void *)(block +1);
    }

    // Still not suitable memory block found, so requestiong fresh memory from OS

    block_header_t * last =get_last_block();
    block = request_space(last,aligned_size);

    if(block==NULL)
    {
        return NULL;
    }

    return (void *)(block +1);


}

// As no garbage collector, manually the code will need to free the space which was previously allocated by 'my_alloc'

void my_free(void *ptr)
{
    block_header_t * block;

    if(ptr ==NULL)
    {
        return;
    }

    // This will be user pointer
    // So first step will be manually steping back so we point it to block header

    block= ((block_header_t *)ptr)-1;
    // Everwhere we are using pointer arithmetic so the C compiler knows that to come back a whole unit of 'block_header_t'

    block->is_free=1;

    // Now let's try to merge them

    coalesce(block);

}

// Custom calloc which will set default value 0 in the memory before giving it to code

void * my_calloc(size_t nelem,size_t elem_size)
{
    size_t total;
    void *ptr;

    // To Prevent integer overflow in multiplication

    if(nelem != 0 && elem_size > SIZE_MAX /nelem)
    {
        return NULL;
    }

    total = nelem * elem_size;
    ptr= my_malloc(total);

    if( ptr != NULL)
    {
        memset (ptr,0, total);
    }

    return ptr;


}


// Multi purpose Realloc

void *my_realloc(void *ptr, size_t size)
{

    block_header_t * block;
    void * new_ptr;
    size_t aligned_size;
    size_t copy_size;

    // realloc(NULL, Size) behaves like malloc(size)
    if(ptr==NULL)
    {
        return my_malloc(size);
    }
    // realloc(ptr, 0) behaves like free(ptr)
    if(size == 0)
    {
        my_free(ptr);
        return NULL;
    }

    aligned_size =align8(size);
    block=((block_header_t *)ptr) -1;

    // If the current block is large enough, Reuse it
    // Also split it if any leftover space is there

    if(block->size >= aligned_size)
    {
        split_block(block,aligned_size);
        return ptr;
    }

    // If not allocate a new block, copy data as it is, free old block

    new_ptr =my_malloc(aligned_size);
    if(new_ptr==NULL)
    {
        return NULL;
    }

    copy_size =block->size;
    memcpy(new_ptr,ptr,copy_size);

    my_free(ptr);

    return new_ptr;


}