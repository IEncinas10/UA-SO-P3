# Entities
##  Process
* ID
* arrivalTime
* startingAddress
* memorySize
* remainingTime
* name
* running (boolean)

##  MemoryManager
* Lists
  - Memory
  - Waiting
  - ToArrive
* processCounter
It is used to provide an ID for the allocated processes
* maxMemory
Size of the memory
* freeMemory
* time
Captures the current time of the memory manager
* allocationAlgorithm
Enum used to capture the executing algorithm

Class used to emulate a memory manager. It is defined by its memory size
and its allocation algorithm.
