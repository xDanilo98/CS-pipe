#!/bin/bash

./main 8 >>supserout 2>>supsererr &
sleep 2

for i in `seq 1 10`
do
	./mainclient 5 8 20 1>>cliout &
	./mainclient 5 8 20 1>>cliout &
	sleep 1
done

#./mainclient 5 8 20 1>>cliout &
#./mainclient 5 8 20 1>>cliout &

for i in `seq 1 5`
do
	sleep 10
	pkill -2 main
done

sleep 10
pkill -2 main
pkill -2 main
./misura.sh cliout supserout
