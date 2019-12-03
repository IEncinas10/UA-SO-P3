/*
 * Assignment: Operating Systems
 * Practise: Third Practise
 * Deadline: 17th of December
 *
 * The goal is to simulate memory management with
 * dynamic partitions. Furthermore we implemented
 * native graphics to visualize the algorithm by
 * using TGUI as a wrapper of the SFML GUI library.
 *
 * Developed by:
 *  - Ignacio Encinas Rubio [1]
 *  - Ernesto Martínez García [1]
 *
 *  [1] Computer Science, University of Alicante Polytechnic School
 *
 * Dependencies:
 *  - sfml https://www.sfml-dev.org/
 *  - tgui https://tgui.eu/
 */

#ifndef _PROCESS
#define _PROCESS

#include <iostream>
#include <string.h>

using namespace std;

#define RED   "\033[1;31m"
#define GREEN "\033[1;32m" 
#define WHITE "\033[1;37m"
#define MAX_NAME 15

class Process {
    public:
        int       ID;
        const int arrivalTime;
        int       startingAddress;
        int       memorySize;
        int       remainingTime;
        char      name[MAX_NAME];
        bool      running;

        Process() : ID(-1), arrivalTime(-1), startingAddress(-1), memorySize(-1), remainingTime(-1), running(false) {}

        Process(const int _arrivalTime, int _memorySize, int _remainingTime, char* _name, 
                int _ID = -1, int _startingAddress = -1, bool _running = false) :
            ID(_ID), arrivalTime(_arrivalTime), startingAddress(_startingAddress) { 

                strncpy(name, _name, MAX_NAME);
                memorySize    = _memorySize;
                remainingTime = _remainingTime;
                running = _running;
            }

        bool operator== (const Process &rhs) const {
            return (ID == rhs.ID && arrivalTime == rhs.arrivalTime &&
                    startingAddress == rhs.startingAddress &&
                    memorySize == rhs.memorySize && 
                    remainingTime == rhs.remainingTime &&
                    running == rhs.running);
        }

        friend
            ostream& operator<<(ostream &os, const Process &p) {
                os << (p.running ? GREEN : RED)
                    << p.startingAddress << " " << p.ID << " " << p.memorySize << " " << (p.running ? "true" : "false")
                    << WHITE;
                return os;
            }
};

#endif
