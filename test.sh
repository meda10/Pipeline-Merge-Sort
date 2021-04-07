#!/bin/bash

log2()
{
  # m = log_2(n)
  n=$1
  m=1

  while [[ $n -gt 2 ]]; do
    let n=n/2
    let m=m+1
  done

  echo $m
}

getProcessCount()
{
  log=$(log2 $1)
  let pc=log+1
  echo $pc
}

# ==================================== MAIN ====================================

# nacteni poctu cisel z parametru
if [ $# -lt 1 ];then
    echo "Usage: test.sh NUMBER_COUNT"
    exit 1
else
    numberCount=$1;
fi

filename="numbers"
processCount=$(getProcessCount "$numberCount") # pocet procesu

# preklad
mpic++ -Wall -std=c++11 -pedantic -O3 --prefix /usr/local/share/OpenMPI pms.cpp -o pms >/dev/null

# vytvoreni souboru s nahodnymi cislu
dd if=/dev/urandom of="$filename" bs=1 count="$numberCount" >/dev/null 2>&1

# beh programu
mpirun --prefix /usr/local/share/OpenMPI -np $processCount pms

# uklid

rm -f "pms" "$filename"


