#!/bin/bash

# formattazione degli output di client e supervisor
# in modo da ottenere per entrambi un file di N righe
# composto da due colonne, ID e SECRET, di ogni client
grep "SECRET" $1 | cut -d' ' -f2,4 | sort >formcliout
grep "BASED" $2 | cut -d' ' -f3,5 | sort -k 2 >formsupout

# dichiarazione di array in cui salvare ID e SECRET veri e stimati
declare -a c0
declare -a c1
declare -a s0
declare -a s1
declare -a temp

# veri
i=0
while read -a temp
do
	c0[$i]=${temp[0]}
	c1[$i]=${temp[1]}
	i=$[i + 1]
done <formcliout
c=$i

# stimati
i=0
while read -a temp
do
	s1[$i]=${temp[0]}
	s0[$i]=${temp[1]}
	i=$[i + 1]
done <formsupout
s=$i

t=0
j=0
good=0
count=0
declare -a err

#stima
for(( i=0;i<$s;i++ ))
do
	# se l'ID corrisponde
	if [ "${s0[$i]}" = "${c0[$j]}" ]
	then
		# t = sec_stimato - sec_vero
		t=$[${s1[$i]} - ${c1[$j]}]
		# t = |t|
		t=${t#-}
		err[$i]=$t
		# count è la sommatoria degli errori
		count=$[count+t]
		if [ "${err[$i]}" -le "25" ]
		then
			good=$[good + 1]
		fi
		j=$[j + 1]
	else
		i=$[i - 1]
		j=$[j + 1]
	fi
done

# percentuale di stime accettabili
p=$[good*100/c]
# errore medio
m=$[count/s]

echo "$good good estimations with $c clients running ($p%)"
echo "error mean: $m"
