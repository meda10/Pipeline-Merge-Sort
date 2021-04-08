#!/bin/bash

if [ $# -lt 1 ];then
    numbers=16;
else
    numbers=$1;
fi;

number_of_processors=`echo "(l($numbers)/l(2)) + 1" | bc -l | awk '{print int($1)}'`

mpic++ -std=c++11 --prefix /usr/local/share/OpenMPI pms.cpp -o pms
dd if=/dev/urandom bs=1 count=$numbers of=numbers >/dev/null 2>&1
mpirun --prefix /usr/local/share/OpenMPI -np $number_of_processors pms
rm -f pms numbers


