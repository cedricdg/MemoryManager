//
//  MemoryManager.cpp
//  Memory Manager
//
//  Created by Cedric Deege on 06/01/15.
//  Copyright (c) 2015 Cedric Deege. All rights reserved.
//

#include <iostream>
#include "MemoryManager.h"

using namespace CedMemoryManager;

//---
//--- Helper functions
//---
unsigned short getNext(void* p)
{
    return *(unsigned short*)p;
}
unsigned short getSize(void* p)
{
    return (((unsigned short*)p)[1]);
}
void setNext(void* p, unsigned short next)
{
    *(unsigned short*)p = next;
}
void setSize(void* p, unsigned short size)
{
    ((unsigned short*)p)[1] = size;
}
unsigned short getNextFree(char* mmPool, unsigned short p, unsigned short size)
{
    if(p == 0)
    {
        throw OutOfMemoryException();
    }
    else if(getSize(&(mmPool[p])) >= size)
    {
        return p; }
    return getNextFree(mmPool, getNext(&(mmPool[p])), size);
}
unsigned short getPrevFree(char* mmPool, unsigned short p, unsigned short searchedP)
{
    if(getNext(&(mmPool[p])) == searchedP)
    {
        return p;
    }
    else if(getNext(&(mmPool[p])) == 0)
    {
        return 0;
    }
    return getPrevFree(mmPool, getNext(&(mmPool[p])), searchedP);
}
unsigned short getPrevUsed(char* mmPool, unsigned short p, unsigned short searchedP)
{
    if(getNext(&mmPool[p]) >= searchedP || getNext(&mmPool[p]) == 0)
    {
        return p;
    }
    else if(p == 0)
    {
        return 0;
    }
    return getPrevUsed(mmPool, getNext(&(mmPool[p])), searchedP);
}

unsigned short getUsed(char* mmPool, unsigned short p, void* searchedP)
{
    if(&mmPool[p+4] == searchedP)
    {
        return p;
    }
    else if(getNext(&mmPool[p]) == 0)
    {
        throw IllegalOperationException("Pointer not valid for deallocation");
    }
    return getUsed(mmPool, getNext(&(mmPool[p])), searchedP);
}
namespace CedMemoryManager
{
    MemoryManager::MemoryManager()
    {
        initializeMemoryManager();
    }
    
    // Initializes set up any data needed to manage the memory pool
    void MemoryManager::initializeMemoryManager()
    {
        unsigned short startUsableMem = 4;
        
        // set next Free Memory
        setNext(&(mmPool[0]), startUsableMem);
        setNext(&(mmPool[startUsableMem]), 0);
        setSize(&(mmPool[startUsableMem]), MAX_POOL_SIZE - (startUsableMem + 4));
        
        // set next Used Memory
        setSize(&(mmPool[0]), 0);
    }
    
    // returns a pointer inside the memory pool
    // If no chunk can accommodate aSize call onOutOfMemory()
    void* MemoryManager::allocate(int aSize)
    {
        // get Fitting Free Space
        unsigned short p = getNextFree(mmPool, getNext(&mmPool[0]), aSize);
        // get Free Space pointing to p
        unsigned short prevFree = getPrevFree(mmPool, START_NEXT_FREE, p);
        // get next Free space
        unsigned short nextFree = getNext(&mmPool[p]);
        if(nextFree == p)
            nextFree = START_NEXT_FREE;
        // get previous used space
        unsigned short prevUsed = getPrevUsed(mmPool, START_NEXT_USED, p);
        // get next used space
        unsigned short nextUsed;
        if(prevUsed == 0)
            nextUsed = 0;
        else
            nextUsed = getNext(&mmPool[prevUsed]);
        
        // Configuring new Free&Used areas
        // set newFree
        unsigned short newFree;
        if(getSize(&mmPool[p]) <= aSize + 4)
        {
            // no new Free
            newFree = nextFree;
            aSize = getSize(&mmPool[p]);
        }
        else
        {
            // create new Free with rest memory
            newFree = p + 4 + aSize;
            setSize(&mmPool[newFree], getSize(&mmPool[p]) - 4 - aSize);
            setNext(&mmPool[newFree], getNext(&mmPool[p]));
        }
        // set prevFree
        setNext(&mmPool[prevFree], newFree);
        
        if(prevUsed != 0)
            setNext(&mmPool[prevUsed], p);
        else
            setNext(&mmPool[2], p);
        setNext(&mmPool[p], nextUsed);
        setSize(&mmPool[p], aSize);
        
        return & mmPool[p+4];
    }
    
    // Frees up a chunk previously allocated
    void MemoryManager::deallocate(void* aPointer)
    {
        // get Fitting Free Space
        unsigned short p = getUsed(mmPool, getNext(&mmPool[START_NEXT_USED]), aPointer);
        // get Free Space pointing to p
        unsigned short prevFree = getPrevFree(mmPool, START_NEXT_FREE, p);
        // get next Free space
        unsigned short nextFree = getNext(&mmPool[prevFree]);
        if(nextFree == p)
            nextFree = START_NEXT_FREE;
        // get previous used space
        unsigned short prevUsed = getPrevUsed(mmPool, START_NEXT_USED, p);
        // get next used space
        unsigned short nextUsed = getNext(&mmPool[p]);
        
        // Configuring new Free areas
        // set nextFree for this
        setNext(&mmPool[p], nextFree);
        // size stays the same
        setNext(&mmPool[prevFree], p);
        // set prevFree
        setNext(&mmPool[0], p);
        
        // Configuring Used areas
        // set last used to point at nextUsed area
        setNext(&mmPool[prevUsed], nextUsed);
    }
    
    //---
    //--- support routines
    //---
    // Will scan the memory pool and return the total free space remaining
    int MemoryManager::freeRemaining(void)
    {
        int i = 0;
        int free = 0;
        do {
            i = getNext(&mmPool[i]);
            if(i != 0){
                free += getSize(&mmPool[i]) + 4;
            }
        } while (i != 0);
        return free;
    }
    
    // Scans the memory pool and returns the largest free space remaining
    int MemoryManager::largestFree(void)
    {
        int i = 0;
        int free = 0;
        do {
            i = getNext(&mmPool[i]);
            if(i != 0){
                if(free < getSize(&mmPool[i]))
                    free = getSize(&mmPool[i]);
            }
        } while (i != 0);
        return free;
    }
    
    // Scans the memory pool and returns the smallest free space remaining
    int MemoryManager::smallestFree(void)
    {
        int i = 0;
        int free = 0;
        do {
            i = getNext(&mmPool[i]);
            if(i != 0){
                if(free > getSize(&mmPool[i]) || free == 0)
                    free = getSize(&mmPool[i]);
            }
        } while (i != 0);
        return free;
    }
    
    // Will give an overview of the Memory Pool
    void MemoryManager::printMemory()
    {
        int i=0;
        int free = 0;
        std::cout << "Free: ";
        do{
            i = getNext(&mmPool[i]);
            if(i != 0)
            {
                std::cout << "\n\t[" << i << "]\tSize: " << getSize(&mmPool[i]) << " (+4)";
                free += getSize(&mmPool[i]) + 4;
            }
        } while (i != 0);
        std::cout << "\nTotal Free: " << free << std::endl;
        i=2;
        int used = 0;
        std::cout << "Used: ";
        do{
            i = getNext(&mmPool[i]);
            if(i != 0)
            {
                std::cout << "\n\t[" << i << "]\tSize: " << getSize(&mmPool[i]) << " (+4)";
                used += getSize(&mmPool[i]) + 4;
            }
        } while (i != 0);
        
        std::cout << "\nTotal Used: " << used << std::endl;
    }
}