#!/bin/bash

unlink supervisor.log
unlink client.log

echo "Avvio Supervisor"
./main 8 1>>supervisor.log &
sleep 2

echo "Avvio 20 client"
for (( i=0; i<10; i++ ))
do
    ./mainclient 5 8 20 >>client.log &
    ./mainclient 5 8 20 >>client.log &
    sleep 1
done

for (( j=0; j<6; j++))
do
    pkill -INT main
    sleep 10
done

echo "doppio sig int"
pkill -INT main
pkill -INT main

echo "print stats"
bash misura.sh client.log supervisor.log
