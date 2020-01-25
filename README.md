#  Operating Systems - Third assignment
## Goal

The goal is to simulate memory management with dynamic partitions. It will offer a menu with the option to partition
assignment using first fit and best fit algorithms.

## Overview
We implement our memory as a group of linked lists whose nodes are processes. For more information, check the documentation.

At the moment we provide the following allocation algorithms:
* First fit
* Best fit

It is possible we add some algorithms in the future.

## Graphics
We use SFML and TGUI (SFML wrapper)

### Instalation
#### TGUI https://tgui.eu/tutorials/0.8/linux/ (It has SFML as a dependency)

#### Arch
```
yay -S tgui-git
```

#### Ubuntu 
```
sudo add-apt-repository ppa:texus/tgui-0.8
sudo apt-get update
sudo apt-get install libtgui-dev
```


## Execution
You should provide the file input as a parameter to the program.
Examples can be seen in test/
## GUI

### Memory representation
We display three rectangles, corresponding to the actual memory, the waiting processes and the processes that have not arrived yet respectively.

There is no limit on how many waiting/toArrive processes we can have,
but we only show some of them because of the limited space.

### Time
You can traverse the memory in time using the slider as you wish.
You can start/stop the automatic execution using the Run/Stop button.

### Algorithm selection
You can select the algorithm clicking on the button that displays the
current algorithm.

IMPORTANT: If you go backwards in time and click 'Run', you will be
shown the previously calculated allocations. If you go back and change
the algorithm, you will see no changes in the memory layout.


### Examples

![alt text](https://github.com/IEncinas10/UA-SO-P3/blob/master/assets/example.gif "Example execution")


![Alt text](assets/gui.png?raw=true "GUI")

#### Disclaimer
The GUI code pretty bad and will probably stay that way. It is something we did because it is mandatory and trying to make it less disgusting is pretty annoying. We're sorry <3
