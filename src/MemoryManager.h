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

#ifndef _MEMORYMANAGER
#define _MEMORYMANAGER

#include "Process.h"
#include <list>
#include <iostream>
#include <vector>
#include <fstream>

#define DEFAULT_OUTPUTFILE "partitions.txt"

enum Alloc {
    Ff = 0, // First fit
    Bf      // Best fit
};

static const char* AlgorithmNames[] {
    "First fit",
    "Best fit",
    NULL
};

class MemoryManager {
    public:
        /* Contains all running processes */
        std::vector<std::list<Process>> memory;

        /* Processes that do not fit */
        std::vector<std::list<Process>> waiting;

        /* Introduced processes which arrivalTime > time */
        std::vector<std::list<Process>> toArrive;

    private:
        /* ID generator for processes, it also tells us
         * how many processes the manager has allocated 
         */
        int processCounter = 0;
        int time;
        int freeMemory;

    public:
        int maxMemory;
        Alloc allocationAlgorithm;

        /*
         * Iterator to the last process allocated
         * 
         * Should be a vector in the future when 
         * we manage time properly
         */
        //std::vector<std::list<Process>::iterator> lastAllocation;
        std::list<Process>::iterator lastAllocation;

        /*
         * Default memory size = 2000
         * Default allocation algorithm = Best fit
         */
        MemoryManager(int _maxMemory = 2000, Alloc _allocationAlgorithm = Alloc::Bf) :
            maxMemory(_maxMemory), allocationAlgorithm(_allocationAlgorithm) {
                freeMemory = maxMemory;
                time = 0;

                /* We create an empty list and push back to both waiting and toArrive */
                std::list<Process> emptyList;
                waiting.push_back(emptyList);
                toArrive.push_back(emptyList);

                /* We add extra nodes to simplify the algorithms */
                char head_name[] = "Head";
                char tail_name[] = "Tail";
                Process process_head(0, 0, -1, head_name, -1, 0, false); 
                Process process_tail(0, 0, -1, tail_name, -1, _maxMemory, false); 
                emptyList.push_front(process_head);
                emptyList.push_back(process_tail);

                memory.push_back(emptyList);
                //lastAllocation.push_back(memory[time].begin());
            }

        int getTime() const { return time; }

        /* In format:
         * <Arrival time> <Process> <Memory required> <Runtime>
         */
        bool readFile(const string &filepath) {
            ifstream inFile(filepath);
            if(!inFile)
                return false;

            int arrivalTime, memorySize, runningTime;
            string name;

            while(inFile >> arrivalTime && inFile >> name && inFile >> memorySize && inFile >> runningTime) {
                Process p(arrivalTime, memorySize, runningTime, &name[0]);
                allocateProcess(p);
            }

            return true;
        }

        /*
         * Intento de leer el cambio de tiempo desde los widgets de la GUI 
         */
        void guiTime(int newTime) {
            newTime -= time;
            if(newTime == 0)
                return;

            while(newTime != 0) {
                if(newTime > 0) {
                    time++;
                    newTime --;

                    if((unsigned)time == memory.size()) {
                        /* We make a copy of the current lists */
                        memory.push_back(memory[time - 1]);
                        waiting.push_back(waiting[time - 1]);
                        toArrive.push_back(toArrive[time - 1]);

/*
 *                        std::list<Process>::iterator aux = lastAllocation[time - 1];
 *
 *                        for(auto itr = memory[time].begin(); itr != memory[time].end(); itr++)
 *                            if((*itr) == (*aux)) {
 *                                lastAllocation.push_back(itr);
 *                            }
 */

                        /* We advance one second in time and modify these copies */
                        updateRunningProcesses();
                        updateWaitingProcesses();
                        updateToArriveProcesses();
                    }
                } else {
                    time--;
                    newTime++;
                }
                log();
            }
        }

        void updateTime() {
            char response;
            cout << "Do you want to advance [a] or go back [b] one second in time? [a/b]" << endl;
            cin >> response;
            if(response == 'a') {
                time++;

                if((unsigned)time == memory.size()) {
                    /* We make a copy of the current lists */
                    memory.push_back(memory[time - 1]);
                    waiting.push_back(waiting[time - 1]);
                    toArrive.push_back(toArrive[time - 1]);

                    /* We advance one second in time and modify these copies */
                    updateRunningProcesses();
                    updateWaitingProcesses();
                    updateToArriveProcesses();
                }

            } else if(response == 'b') {
                if(time != 0)
                    time--;
            }
        }

        /*
         * We decrease the remainingTime of running processes
         * If their time gets to 0, we erase them from memory.
         */
        void updateRunningProcesses() {
            for(std::list<Process>::iterator itr = memory[time].begin(); itr != memory[time].end(); itr++)
                /* Permitir procesos con remainingTime < 0, 
                 * para mantener el head y el tail 
                 */
                if((--((*itr).remainingTime)) == 0) {
                    freeMemory += (*itr).memorySize;
                    //if(itr == lastAllocation[time]) {
                        //(*itr).memorySize = 0;
                    //} else
                    itr = memory[time].erase(itr);
                    itr--;
                }
        }

        /*
         * Function to introduce waiting processess
         * to memory if possible
         *
         * Return: Number of processess introduced
         */
        size_t updateWaitingProcesses(){
            size_t procNum = 0;
            for(std::list<Process>::iterator itr = waiting[time].begin(); itr != waiting[time].end(); itr++) {
                switch(allocationAlgorithm) {
                case Ff:
                    if(firstFit(*(itr), false)) {
                        itr = waiting[time].erase(itr);
                        itr--;
                        procNum++;
                    }
                    break;
                case Bf:
                    if(bestFit(*(itr), false)) {
                        itr = waiting[time].erase(itr);
                        itr--;
                        procNum++;
                    }
                    break;
                default:
                    break;
                }
            }
            return procNum;
        }

        /*
         * Function to introduce arriving processess
         * to memory if possible
         *
         * Return: Number of processess introduced
         */
        size_t updateToArriveProcesses(){
            size_t procNum = 0;
            for(std::list<Process>::iterator itr = toArrive[time].begin(); itr != toArrive[time].end(); /* itr++ */) {
                if((*itr).arrivalTime != time){ //If the process arrivalTime doesn't correspond to current time, skip
                    itr++;
                    continue;
                }

                switch(allocationAlgorithm) {
                case Ff:
                    if(firstFit(*(itr)))
                        procNum++;
                    itr = toArrive[time].erase(itr);
                    break;
                case Bf:
                    if(bestFit(*(itr)))
                        procNum++;
                    itr = toArrive[time].erase(itr);
                    break;
                default:
                    break;
                }
            }
            return procNum;
        }

        /* It changes the algorithm in use */
        void setAlgorithm(Alloc algo) { allocationAlgorithm = algo; }

        /* It gets the name of the algorithm in use */
        const char * getAlgorithm() { return AlgorithmNames[allocationAlgorithm]; } 

        void log() {
            string log = std::to_string(time); 
            log += " ";
            for(auto itr = (++memory[time].begin()); itr != (--memory[time].end()); itr++) {
                log += "[" + std::to_string((*itr).startingAddress) + " " + (*itr).name + " ";
                log += std::to_string((*itr).memorySize) + "] ";
            }
            std::cout << log << std::endl;
        }

        void allocateProcess(Process &p) {
            /* Show error message? */
            if(p.memorySize > maxMemory || p.memorySize <= 0)
                return;

            /* If the process time is bigger than the actual time */
            /* If the process is bigger than free space, we dont even try */
            if(p.arrivalTime > time || p.memorySize > freeMemory) {
                p.ID = -1;
                p.startingAddress = -1;
                p.running = false;
                if(p.arrivalTime > time) {
                    toArrive[time].insert(toArrive[time].end(), p);
                }
                else {
                    waiting[time].insert(waiting[time].end(), p);
                }
                return;
            }

            /* It fits and it is the first process.
             * Algorithm independent
             */
            if(memory.size() == 0) {
                p.ID = processCounter++;
                p.running = true;
                p.startingAddress = 0;
                freeMemory -= p.memorySize;

                /* We update lastAllocation */
                //lastAllocation[time] = memory[time].insert(memory[time].end(), p);
                return;
            }

            switch(allocationAlgorithm) {
                case Ff:
                    firstFit(p);
                    break;
                case Bf:
                    bestFit(p);
                    break;
                default:
                    break;
            }

        }

/*
 *  Pending: next fit 
 *
 *        bool nextFit(Process &p, bool newProcess = true) {
 *            std::list<Process>::iterator itr = lastAllocation[time];
 *
 *            do {
 *                if(itr == memory[time].end())
 *                    itr = memory[time].begin();
 *
 *                int freeSize = 0;
 *
 *                 //If the next node is an 'empty' node, there is no other running process until the
 *                 //end of memory, so we take nextStartingAddr = maxMemory
 *                int nextStartingAddr = (*(++itr)).startingAddress;
 *
 *                freeSize = nextStartingAddr - (*(--itr)).startingAddress - (*itr).memorySize;
 *
 *                //[> If the process fits, we insert it into memory <]
 *                if(freeSize >= p.memorySize) {
 *                    p.ID = processCounter++;
 *                    p.startingAddress = (*itr).startingAddress + (*itr).memorySize;
 *                    p.running = true;
 *                    freeMemory -= p.memorySize;
 *
 *                    //[> We update the lastAllocation attribute <]
 *                    lastAllocation[time] = memory[time].insert((++itr), p);
 *                    return true;
 *                } else ++itr;
 *
 *            } while(itr != lastAllocation[time]);
 *
 *             //If the process is not new and comes from
 *             //waiting, we dont duplicate the data.
 *            if(newProcess)
 *                waiting[time].insert(waiting[time].end(), p);
 *
 *            return false;
 *        }
 */
        bool firstFit(Process &p, bool newProcess = true) {
            for(std::list<Process>::iterator itr = memory[time].begin(); itr != (--memory[time].end()); itr++) {
                int freeSize = 0;
                int nextStartingAddr = (*(++itr)).startingAddress;
                freeSize = nextStartingAddr - (*(--itr)).startingAddress - (*itr).memorySize;

                if(freeSize >= p.memorySize) {
                    p.ID = processCounter++;
                    p.startingAddress = (*itr).startingAddress + (*itr).memorySize;
                    p.running = true;
                    lastAllocation = memory[time].insert(++itr, p);
                    --itr;
                    return true;
                }
            }

            if(newProcess)
                waiting[time].insert(waiting[time].end(), p);
            return false;
        }

        bool bestFit(Process &p, bool newProcess = true) {
            std::list<Process>::iterator insertItr = memory[time].begin();
            int size = -1;

            for(std::list<Process>::iterator itr = memory[time].begin(); itr != (--memory[time].end()); itr++) {
                int freeSize = 0;
                int nextStartingAddr = (*(++itr)).startingAddress;
                freeSize = nextStartingAddr - (*(--itr)).startingAddress - (*itr).memorySize;

                if(freeSize >= p.memorySize && (size == -1 || freeSize < size)) {
                    size = freeSize;
                    insertItr = itr;
                }
            }

            if(size != -1) {
                p.ID = processCounter++;
                p.startingAddress = (*insertItr).startingAddress + (*insertItr).memorySize;
                p.running = true;
                lastAllocation = memory[time].insert((++insertItr), p);
                freeMemory -= p.memorySize;
                return true;
            } else if(newProcess) {
                /* Avoid duplicating data */
                waiting[time].insert(waiting[time].end(), p);
            }
            return false;
        }

};

#endif
