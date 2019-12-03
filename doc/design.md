# MemoryManager

We will maintain three linked lists in order to fully describe the status of the memory.

* Memory     
It will be formed by active processes and ordered by their starting address.
* Waiting   \
It will be formed by inactive processes that have arrived but won't fit in the memory
* To arrive \
It will be formed by inactive processes whose arrival time is greater than the current time of the memory.

## Scheme

    +--------------------------+    +--------------------------+    +--------------------------+
    |         Running          |    |         Waiting          |    |        To arrive         |
    |     Limited in size      |    |    Unlimited in size     |    |    Unlimited in size     |
    |     running = true       |    |     running = false      |    |     running = false      |
    +--------------------------+    +--------------------------+    +--------------------------+

## How it works
When a second passes in our program we do the following tasks:
* Update the 'Running' list.   \
We traverse the memory updating the remaining running time of each process. If a process
has remaining time equal to 0, we erase it from memory.

* Update the 'Waiting' list.   \
We traverse the waiting list, allocating the processes that fit in the memory.

* Update the 'To arrive' list. \
We check for processes that arrive in the current instant. If there is any, we try to fit it
inside the memory. If it doesn't fit, we add it to the waiting list.

This is done in this specific order to respect the priorities.

## Notes
Running processes can't be reallocated. Once they are put into memory, they stay in the same spot until
they complete their execution time.
