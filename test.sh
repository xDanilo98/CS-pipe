#!/bin/bash

./main 8 >>supervisorout.log 2>>supervisorerr.log &
sleep 2

for i in `seq 1 10`
do
	./mainclient 5 8 20 1>>clientout.log &
	./mainclient 5 8 20 1>>clientout.log &
	sleep 1
done

for i in `seq 1 5`
do
	sleep 10
	pkill -2 main
done

sleep 10
pkill -2 main
pkill -2 main
./misura.sh clientout.log supervisorout.log
