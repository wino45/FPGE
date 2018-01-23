# FPGE
Fred's Panzer General Editor

Scenario editor for Panzer General.

Based on FPGE for DOS, Version 0.1 by Fred Chlanda

Compiling:
- use Allegro library 4.4.X with Loadpng 1.5

For Windows use MinGW, check Eclispse project for details on needed libraries


Compiling under Linux:
- install dependencies
sudo apt-get install liballegro4-dev libloadpng4-dev

- compile program from top-level directory using cmake
mkdir build && cd build
cmake ..
make

Program file will be in build/bin directory
