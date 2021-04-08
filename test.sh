#!/bin/bash

function number_of_processors {
    local x=0
    for (( y=$1-1 ; $y > 0; y >>= 1 )) ; do
        let x=$x+1
    done
    let x=$x+1
    echo $x
}

if [ $# -lt 1 ];then
    numbers=16;
else
    numbers=$1;
fi;

number_of_processors=$(number_of_processors $numbers)

mpic++ -std=c++11 --prefix /usr/local/share/OpenMPI pms.cpp -o pms
dd if=/dev/random bs=1 count=$numbers of=numbers >/dev/null 2>&1
mpirun --prefix /usr/local/share/OpenMPI -np $number_of_processors pms
rm -f pms numbers


