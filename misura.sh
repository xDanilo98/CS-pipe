#!/bin/sh

corretti=0
differenza=0

grep -i "secret" $1 > tempclient.log
client="tempclient.log"

while read -r line; do
    ID=$(echo $line | cut -d' ' -f 2)
    secret=$(echo $line | cut -d' ' -f 4)
    grep -i "$ID" $2 | grep -i "supervisor" | grep -i "based on" > tempsupervisor.log
    supervisor="tempsupervisor.log"
    while read -r sline; do
        stima=$(echo "$sline" | cut -d' ' -f 3)
        diff=$(($stima-$secret))
        ((differenza+=$diff))
        if [$diff -lt 25 ]; then
            ((corretti++))
        fi
    done < "$supervisor"
    unlink tempsupervisor.log
done < "$client"
echo "$corretti/20 Corretti"
errorem=$((differenza/20))
echo "Errore medio: $errorem"
unlink tempclient.log
