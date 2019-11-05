#include "list.h"
#include <stdlib.h>

struct nodeStruct* List_createNode(int item){
    struct nodeStruct* temp=(struct nodeStruct*)malloc(sizeof(struct nodeStruct));
    temp->item=item;
    temp->next=NULL;
    return temp;
}

void List_insertHead (struct nodeStruct **headRef, struct nodeStruct *node){
    struct nodeStruct** head=headRef;
    if(head!=NULL)
        node->next=*headRef;
    *head=node;
}
void  List_insertTail (struct nodeStruct **headRef, struct nodeStruct *node){
    struct nodeStruct* head=*(headRef);
    if(head==NULL){
        return;
    }
    while(head->next!=NULL){
        head=head->next;
    }
    head->next=node;
}
int  List_countNodes (struct nodeStruct *head){
    if(head==NULL)
        return 0;
    int count=0;
    while (head!=NULL){
        count++;
        head=head->next;
    }
    return count;
    
}

struct nodeStruct*  List_findNode(struct nodeStruct *head, int item){
    if(head==NULL)
        return NULL;
    while (head!=NULL){
        if(head->item==item){
            return head;
        }
        head=head->next;
    } 
    return NULL;
}

void  List_deleteNode (struct nodeStruct **headRef, struct nodeStruct *node){
    struct nodeStruct* head=*(headRef);
    struct nodeStruct* temp=List_findNode(head,node->item);
    if(temp==NULL){
        return;
    }
    else if(head==temp){
        head=head->next;
        free(temp);
    }
    else{

        while(head->next!=temp){
            head=head->next;
        }
        head->next=temp->next;
        free(temp);
    }
}
int partition(int *arr,int start,int end){
    int pivot=arr[start];
    int partition_index=end;
    for(int i=end;i>start;i--){
        if(arr[i]>=pivot){
            int temp=arr[i];
            arr[i]=arr[partition_index];
            arr[partition_index]=temp;
            partition_index--;
        }

    }
    arr[start]=arr[partition_index];
    arr[partition_index]=pivot;
    return partition_index;

}
void quick_sort(int* arr,int start,int end)
{
    if(start>=end)
        return;
    int partition_index=partition(arr,start,end);
    quick_sort(arr, start, partition_index-1);
    quick_sort(arr,partition_index+1,end);
}
void  List_sort (struct nodeStruct **headRef){
    struct nodeStruct* head=*(headRef);
    struct nodeStruct* temp=head;
    const int size=List_countNodes(head);
    int array[size];
    int i=0;
    while (head!=NULL){
        array[i]=head->item;
        i++;
        head=head->next;
    }
    int start=0;
    int end=size-1;
    quick_sort(array,start,end);
    int j=0;
    while (temp!=NULL){
        temp->item= array[j];
        j++;
        temp=temp->next;
    }

}
