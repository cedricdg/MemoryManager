# MemoryManager
Very basic static Memory Manager for allocations from 1 up to 65528 bytes

The Memory Manager allocates 65536 bytes from the beginning in an char array.
All Data is stored within the Array. 
## How it works
The pointing to the next avariable Free or Used space is done by short numbers (2 bytes size). 
The Very first 4 bytes are pointing by short numbers to the first free (1st and 2nd byte) and used (3rd and 4th) space.

One Free space is pointing at the next free space until it reached the last free space. Same procedure for used space.
One Allocation of space has an overhead of 2 short numbers (4 bytes):
The first short number (bytes 1 and 2) points to the next avariable free/used space.
The second short number (bytes 3 and 4) is giving the size of the space.

### Overhead
In total you have an overhead of 4 bytes at the initialization and another 4 bytes for every allocation.
