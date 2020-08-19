
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "ObjectManager.h"

//compact() function
void compact();
//check for invariant, it check every nodes in the linked list
void checkState();
//startPtr of usable memory, increase by adding size of the next insert
static ulong startPtr;
//count how many node are created,increse like 1,2,3,4,5...
Ref idRef;
//count how many nodes are in the linkedlist
static ulong numNode;

typedef struct NODE Node;
struct NODE
{
    //this is the "id"
    Ref ref;
    //how many references reference to this node
    int count;
    //size of memory use
    ulong size;
    ulong startAt;
    Node *next;
};

//the top node of the linkedlist
Node *top;

//two buchar buffer
uchar *b_1;
uchar *b_2;
//start with a pointer pointing to the current buffer(will be initial in initPool)
uchar *B;

void initPool()
{
    //pre-condition
    checkState();
    b_1 = (uchar *)malloc(sizeof(uchar *) * (MEMORY_SIZE));
    b_2 = (uchar *)malloc(sizeof(uchar *) * (MEMORY_SIZE));
    //startPtr at 0 in the memory
    startPtr = 0;
    idRef = 0;
    top = NULL;
    //startPtr with b_1 the targer buffer
    B = b_1;
    numNode = 0;
    //post-condition
    //malloc success
    assert(b_1 != NULL);
    assert(b_2 != NULL);
    checkState();
}
//insert an ojb in the linkedlist
Ref insertObject(ulong size)
{
    //pre-condition
    checkState();
    //1.the pool is initialized
    assert((b_1 != NULL) && (b_2 != NULL));
    //2.its size meet ulong condition
    assert(size >= 0);
    Ref ret = NULL_REF;
    if ((startPtr + size) >= MEMORY_SIZE)
    {
        compact();
    }
    if ((startPtr + size) < MEMORY_SIZE && size != 0)
    {
        Node *newNode = (Node *)malloc(sizeof(Node));
        assert(newNode != NULL);
        if (newNode != NULL)
        {
            newNode->ref = idRef + 1;
            newNode->size = size;
            newNode->startAt = startPtr;
            newNode->count = 1;
            newNode->next = NULL;
            Node *current = top;
            if (top == NULL)
            {
                top = newNode;
            }
            else
            {
                while (current->next != NULL)
                {
                    current = current->next;
                }
                current->next = newNode;
            }
            idRef = idRef + 1;
            numNode = numNode + 1;
            startPtr = startPtr + size;
            ret = newNode->ref;
        }
        //post-condition if insert success
        assert(numNode > 0);
    }
    else
    {
        printf("Unable to successfully complete memory allocation request.\n");
    }
    //post-condition
    checkState();
    return ret;
}

//when you run out of memory(NOMEM),copy content of B into b_2 and set B=b_2
//(MOMEM) copy contents of B into b_1 and set B=b_1
void compact()
{
    //pre-condition
    checkState();
    assert(B != NULL);//after initial
    uchar *newBuffer;
    ulong newStartPtr = 0;
    if (B == b_2)
    {
        newBuffer = b_1;
    }
    else
    {
        newBuffer = b_2;
    }
    Node *current = top;
    while (current != NULL)
    {
        assert(newStartPtr + current->size < MEMORY_SIZE);
        memcpy(&newBuffer[newStartPtr], &B[current->startAt], current->size);
        current->startAt = newStartPtr;
        newStartPtr += current->size;
        current = current->next;
    }
    B = newBuffer;
    printf("\nGarbage collector statistics:\n");
    printf("objects: %lu\t", numNode);
    printf("byte in use: %lu\t", newStartPtr);
    printf("freed: %lu\n\n", startPtr - newStartPtr);
    startPtr = newStartPtr;
    //post-condition
    checkState();
    assert(B != NULL);
}

//retrieve object, if not found, return NULL
void *retrieveObject(Ref ref)
{
    //pre-condition
    assert(ref >= 0 && ref<= idRef);
    checkState();
    void *ret = NULL;
    Node *current = top;
    bool found = false;
    while (current != NULL && !found)
    {
        if (current->ref == ref)
        {
            found = true;
            break;
        }
        current = current->next;
    }
    if (found)
    {
        ret = &B[current->startAt];
    }
    else
    {
        printf("Invalid reference exception with reference %lu, terminating process.\n", ref);
    }
    //post-condition
    checkState();
    return ret;
}
//add reference 
void addReference(Ref ref)
{
    //pre-condition
    assert(ref >= 0 && ref <= idRef);
    checkState();
    Node *current = top;
    while (current != NULL)
    {
        if (current->ref == ref)
        {
            current->count++;
            //printf("added ref,count:%d\n", current->count);
            break;
        }
        current = current->next;
    }
    //post-condition
    checkState();
}
//drop reference
void dropReference(Ref ref)
{
    //pre-conditon
    assert(ref >= 0 && ref <= idRef);
    checkState();
    Node *current = top;
    Node *previous = top;
    while (current != NULL)
    {
        if (current->ref == ref)
        {
            //printf("find ref,count:%d\n", current->count);
            current->count--;
            //free the node and re-connect america by delivery
            if (current->count == 0)
            {
                //case 1, top is the node to be deleted
                if (current == top)
                {
                    top = current->next;
                }
                //case2, normal case
                else
                {
                    previous->next = current->next;
                }
                free(current);
                numNode = numNode - 1;
            }
            //printf("droped ref,count:%d\n", current->count);
            break;
        }
        previous = current;
        current = current->next;
    }
    checkState();
}

//destroy pool will destroy the pool and free the memory space
void destroyPool()
{
    //pre-conditon
    assert(b_1 != NULL);//its after init
    assert(b_2 != NULL);
    checkState();
    Node *temp;
    while (top != NULL)
    {
        temp = top;
        top = top->next;
        free(temp);
    }
    numNode = 0;
    //free the buffer1
    free(b_1);
    b_1 = NULL;
    //free the buffer2
    free(b_2);
    b_2 = NULL;
    B = NULL;
    //post-condition
    checkState();
    assert(numNode == 0);
    assert(b_1 == NULL && b_2 == NULL && B == NULL);
    assert(top == NULL);
}
//dumpPool will print every node that exist
void dumpPool()
{
    //pre-condition
    checkState();
    assert(B != NULL);//current buffer is not null(after init)
    ulong counter = 0;
    Node *current = top;
    while (current != NULL)
    {
        printf("memory No.%lu ,Ref: %lu, startPtr at: %lu, size: %lu, ref count: %d\n", counter, current->ref, current->startAt, current->size, current->count);
        current = current->next;
        counter++;
    }
    //post-condition
    checkState();//check dumpPool
}

//checkState will check every node that in the linked list
void checkState()
{
    //after initPool()
    if (B != NULL)
    {
        //not empty linkedlist
        if (top != NULL)
        {
            Node *current = top;
            while (current != NULL)
            {
                assert(current->ref >= 0);
                assert(current->count > 0);
                assert(current->size > 0);
                assert(current->startAt >= 0);

                current = current->next;
            }
        }
        else
        {
            //every node is deleted or no node is added
            assert(numNode == 0);
        }
    }else
    {
        //not yet initPool()
        assert(b_1 == NULL);
        assert(b_2 == NULL);
    }
    
}
