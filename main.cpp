//
//  main.cpp
//  Memory Manager
//
//  Created by Cedric Deege on 06/01/15.
//  Copyright (c) 2015 Cedric Deege. All rights reserved.
//

#include <iostream>
#include "MemoryManager.h"
using namespace std;
using namespace CedMemoryManager;

void pause()
{
	cin.ignore();
}

int main(int argc, const char * argv[])
{
	double* pointer[10];
	char* newPointer[4];

    // Create MemoryManager
    MemoryManager m1 = *new MemoryManager();
    cout << "-- Initially free: " << m1.freeRemaining() << endl;
	
	// Allocate Memory
	for (int i = 0; i < 10; i++)
		pointer[i] = (double*)m1.allocate(sizeof(double));
	cout << "-- Allocate 10 double:" << endl;
	m1.printMemory();

	pause();

	// Deallocate Memory
	cout << "-- Dealocate 3rd double:" << endl;
	m1.deallocate(pointer[3]);
	m1.printMemory();
	cout << "-- Smallest free: " << m1.smallestFree() << endl;
	cout << "-- Largest free: " << m1.largestFree() << endl;

	pause();

	// Allocate Memory
	cout << "-- Allocate 4 int:" << endl;
	for (int i = 0; i < 4; i++)
		newPointer[i] = (char*)m1.allocate(sizeof(char));
	m1.printMemory();

	pause();

    return 0;
}
