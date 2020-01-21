#!/bin/bash

for (( i=2; i<10; i++ ))
do
    ./mainclient $i 12 30 >>testout.log &
done
