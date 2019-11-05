#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "list_sol.h"
#include "kallocator.h"

struct KAllocator {
    enum allocation_algorithm aalgorithm;
    int size;
    void* memory;
    // Some other data members you want, 
    // such as lists to record allocated/free memory
};

struct KAllocator kallocator;
struct nodeStruct* free_head = NULL;
struct nodeStruct* occupied_head = NULL;


void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm) {
    assert(_size > 0);
    kallocator.aalgorithm = _aalgorithm;
    kallocator.size = _size;
    kallocator.memory = malloc((size_t)kallocator.size);
    free_head = List_createNode(_size,0);
    occupied_head = NULL;
    // Add some other initialization 
}

void destroy_allocator() {
    free(occupied_head);
    free(free_head);
    free(kallocator.memory);
    
    // free other dynamic allocated memory to avoid memory leak
}

void* kalloc(int _size) {
    void* ptr = NULL;
    struct nodeStruct* iter = free_head;
   // struct nodeStruct* it = occupied_head;
    if(kallocator.aalgorithm == FIRST_FIT){
      while(iter!=NULL){
          if(iter->size>_size){
            struct nodeStruct* insert = List_createNode(_size,iter->index);
            List_insertHead(&occupied_head,insert);
            iter->size -= _size;
            iter->index += _size;
            //  while(it!=NULL){
            //     printf("\nsize is\t%d\tand index is\t%d\n",it->size,it->index);
            //     it = it->next;
            // }
          
            return kallocator.memory + insert->index;
          }
          if(iter->size == _size){
              struct nodeStruct* insert = List_createNode(_size,iter->index);
              List_deleteNode(&free_head,iter);
              List_insertHead(&occupied_head,insert);
              return kallocator.memory + insert->index;
          }
          iter = iter->next;
      }  
    }
    
     else if(kallocator.aalgorithm == BEST_FIT){
         struct nodeStruct* temp = iter;
         while(iter!=NULL){
             if((temp->size>iter->size)&&(iter->size>=_size)){
                 temp = iter;
             }
             iter = iter->next;
         }
         if(temp->size > _size){
            struct nodeStruct* insert = List_createNode(_size,temp->index);
            temp->size -= _size;
            temp->index += _size;
            List_insertHead(&occupied_head,insert);
            return kallocator.memory + insert->index;
         }
         else if(temp->size == _size){
              struct nodeStruct* insert = List_createNode(_size,temp->index);
              List_insertHead(&occupied_head,insert);
              List_deleteNode(&free_head,temp);
              return kallocator.memory + insert->index;

         }
    }
     else if(kallocator.aalgorithm == WORST_FIT){
         struct nodeStruct* temp = iter;
         while(iter!=NULL){
             if((temp->size<iter->size)&&(iter->size>=_size)){
                 temp = iter;
             }
             iter = iter->next;
         }
         if(temp->size > _size){
            struct nodeStruct* insert = List_createNode(_size,temp->index);
            temp->size -= _size;
            temp->index += _size;
            List_insertHead(&occupied_head,insert);
            return kallocator.memory + insert->index;
         }
         else if(temp->size == _size){
              struct nodeStruct* insert = List_createNode(_size,temp->index);
              List_insertHead(&occupied_head,insert);
              List_deleteNode(&free_head,temp);
              return kallocator.memory + insert->index;

         }

         
    }
    // Allocate memory from kallocator.memory 
    // ptr = address of allocated memory

    return ptr;
}

void kfree(void* _ptr) {
    assert(_ptr != NULL);
    //exist a free block after _ptr in memory so we need to merge 
    struct nodeStruct* merge = List_findNode1(occupied_head, _ptr - kallocator.memory);
   // printf("print out delete node from occupied list's index and size\t%d\t%d",merge->index,merge->size);
    struct nodeStruct* merge1 = List_findNode1(free_head, merge->index+merge->size);
    //exist a free block before _ptr in memory so we need to merge
    struct nodeStruct* merge2 = List_findNode2(free_head, merge->index);
    if(merge1!=NULL&&merge2!=NULL){
        merge2->size = merge2->size + merge1->size + merge->size;
        List_deleteNode(&free_head,merge1);
    }
    else if(merge1!=NULL){
        merge1->index = merge->index;
        merge1->size += merge->size;
    } 
    else if(merge2!=NULL){
        merge2->size += merge->size;
    }
    else{
        struct nodeStruct* insert = List_createNode(merge->size,merge->index);
        List_insertHead(&free_head,insert);
    }
    List_deleteNode(&occupied_head,merge);
    
}

int compact_allocation(void** _before, void** _after) {
    int compacted_size = 0;
    struct nodeStruct* temp1 = occupied_head;
    if(temp1==NULL){
        *_before = NULL;
        *_after = NULL;
        return compacted_size;
    }
    int start = 0;
    while(temp1!=NULL){
        *(_before+start) = temp1->index + kallocator.memory;
        temp1 = temp1->next;
        start++;
    }

    List_sort(&occupied_head);
    int index = 0;
    struct nodeStruct* temp2 = occupied_head;
    while(temp2!=NULL){
        temp2->index = index;
        index += temp2->size;
        temp2 = temp2->next;
    }
    int change = 0;
     while(temp2!=NULL){
        *(_after+change) = temp2->index + kallocator.memory;
        temp2 = temp2->next;
        change++;
    }
    free_head->index = index;
    free_head->size = kallocator.size - index;
    struct nodeStruct* iter = free_head;
    while(iter->next!=NULL){
        List_deleteNode(&free_head,iter->next);

    }
    

    // compact allocated memory
    // update _before, _after and compacted_size
    compacted_size = index;
   // printf("print out the compated size of memory\t%d\t",compacted_size);
    return compacted_size;
}

int available_memory() {
    int available_memory_size = 0;
    struct nodeStruct* iter = free_head;
    while (iter!=NULL)
    {
        available_memory_size += iter->size;
        iter = iter->next;
    }
    

    // Calculate available memory size
    return available_memory_size;
}

void print_statistics() {
    struct nodeStruct* iter2 = free_head;
    List_sort(&free_head);
    while (iter2->next!=NULL)
    {
       // printf("the free size is\t%d in free list\n", iter2->size);
        iter2 = iter2->next;
    }
    //int sum =0;
//     struct nodeStruct* iter3 = occupied_head;
//    //  printf("the allocated HEAD size is\t%d in occupied list\n", iter3->size);
//     while (iter3!=NULL)
//     {
//         sum += iter3->size;
//         printf("the allocated size is\t%d in occupied list\n", iter3->size);
//         iter3 = iter3->next;
//     }
    
    
    int allocated_size = kallocator.size - available_memory();
   // int allocated_size = sum;
    int allocated_chunks = List_countNodes(occupied_head);
    int free_size = available_memory();
    int free_chunks = List_countNodes(free_head);
    int smallest_free_chunk_size = free_head->size;
    int largest_free_chunk_size = iter2->size;

    // Calculate the statistics

    printf("Allocated size = %d\n", allocated_size);
    printf("Allocated chunks = %d\n", allocated_chunks);
    printf("Free size = %d\n", free_size);
    printf("Free chunks = %d\n", free_chunks);
    printf("Largest free chunk size = %d\n", largest_free_chunk_size);
    printf("Smallest free chunk size = %d\n", smallest_free_chunk_size);

}

