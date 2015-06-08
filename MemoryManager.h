//
//  MemoryManager.h
//  Memory Manager
//
//  Created by Cedric Deege on 06/01/15.
//  Copyright (c) 2015 Cedric Deege. All rights reserved.
//

#pragma once

#ifndef __MEMORY_MANAGER_H__
#define __MEMORY_MANAGER_H__


namespace CedMemoryManager
{
    
    const int MAX_POOL_SIZE = 65536;
    const int START_NEXT_FREE = 0;
    const int START_NEXT_USED = 2;
    
	// Memory 
    class MemoryManager
    {
        
    private:
        // mmPool is the only global variable that will be used
        char mmPool[MAX_POOL_SIZE];
        
        // Initialize any data needed to manage the memory pool
        void initializeMemoryManager();
        
    public:
        //---
        //--- CORE Functions
        //---
        
        // Constructor calls initializeMemoryManager
        MemoryManager();
        
        // returns a pointer inside the memory pool
        // If no chunk can accommodate aSize call OnAllocFail()
        void* allocate(int aSize);
        
        // Free up a chunk previously allocated
        void deallocate(void* aPointer);
        
        //---
        //--- Support routines
        //---
        
        // Will scan the memory pool and return the total free space remaining
        int freeRemaining(void);
        
        // Will scan the memory pool and return the largest free space remaining
        int largestFree(void);
        
        // will scan the memory pool and return the smallest free space remaining
        int smallestFree(void);
        
        // Will give an overview of the Memory Pool
        void printMemory();
    };
    
    //---
    //--- Exceptions
    //---
    
    // Call if no space is left for the allocation request
    struct OutOfMemoryException : public std::exception
    {
        const char * what () const throw ()
        {
            return "Out of Memory!";
        }
    };
    
    // If the caller makes any illegal request
    struct IllegalOperationException : public std::exception
    {
        const char* msg;
        IllegalOperationException(char* c) : msg(c){};
        const char * what () const throw ()
        {
            return msg;
        }
    };
    
};

#endif  // __MEMORY_MANAGER_H__

