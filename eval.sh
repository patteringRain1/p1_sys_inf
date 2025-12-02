#!/bin/bash

# CSV Files
echo "threads,time" > measures_philosophes.csv
echo "threads,time" > measures_rw.csv
echo "threads,time" > measures_prodcons.csv

THREADS="2 4 8 16 32"

# PHILOSOPHES

cd .

for t in $THREADS; do
    for j in 1 2 3 4 5; do
        echo "Philosophes : $t threads (run $j/5)"
        /usr/bin/time -f %e -o /tmp/measure philosophes $t > /dev/null 2>&1
        echo "$t,$(cat /tmp/measure)" >> measures_philosophes.csv
    done
done

# LECTEURS & ECRIVAINS

for t in $THREADS; do
    lec=$((t/2))
    ecr=$((t/2))

    for j in 1 2 3 4 5; do
        echo "Lecteurs/Ecrivains : $t total → $lec lecteurs / $ecr écrivains (run $j/5)"
        /usr/bin/time -f %e -o /tmp/measure lecteurs_ecrivains $ecr $lec > /dev/null 2>&1
        echo "$t,$(cat /tmp/measure)" >> measures_rw.csv
    done
done

# PRODUCTEURS & CONSOMMATEURS

for t in $THREADS; do
    prod=$((t/2))
    cons=$((t/2))

    for j in 1 2 3 4 5; do
        echo "Prod/Cons : $t total → $prod producteurs / $cons consommateurs (run $j/5)"
        /usr/bin/time -f %e -o /tmp/measure prodcons $prod $cons > /dev/null 2>&1
        echo "$t,$(cat /tmp/measure)" >> measures_prodcons.csv
    done
done

echo "Mesures terminées"
