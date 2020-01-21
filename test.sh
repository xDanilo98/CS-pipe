#!/bin/bash

for (( i=1; i<10; i++ ))
do
	./mainclient &i 12 30
done
