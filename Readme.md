# Control Unit Logic Operator (aka CULO) 

This is the software for the new Main Control Unit (MCU) of the Raceup Car.

## Setup

To set up the project clone the repository  and generate the files for 
the dbc can bus with the following command:
(Because the repo is quite big is better to use --depth=1)

```sh
    git clone --depth=1 --recursive https://github.com/raceup-electric/ControlUnitLogicOperator.git
    cd ./ControlUnitLogicOperator/lib/board_dbc/dbc
    ./refresh_dbc.sh
    cd -
```

## I/O connections

This list below represents all the I/O port used in the code:

    - 3 can node
    - 1 serial (debug)
    - 12 gpio

## Cores

The code is based on the assumption that there are at least 3 cores

## Project Organization

To detach the software from the hardware in this repo there is an abstraction layer,
defined through header files, which allow us to compile the same logic code for different 
architectures. The headers are define in lib/raceup_board
All the hardware supported are implemented in the instances directory except for linux,
which is used only for testing purpose, and is implemented in tests/linux_board.

The logic entry point of the code is in src/ControlUnitLogicOperator.h and 
src/ControlUnitLogicOperator.c which defines a common main for every core.

## Test

The project uses the linux implementation to run a series of unit-tests which are defined 
in the directory tests. For more info check [test](./tests/Readme.md)

## Doc

The repo also contains in itself a lot of documentation that may be useful to understand 
how the code works. All the documents can be found in the doc directory
There is also a Doxygen documentation. To create it run the command:
```sh
    doxygen Doxyfile
```
the doc will be generated in doc/doxygen and to open it you can open in a browser the file
doc/doxygen/html/index.html
