#!/bin/bash

# CSV Files
echo "threads,time" > measures_philosophes.csv
echo "threads,time" > measures_rw.csv
echo "threads,time" > measures_producteurs_consommateurs.csv

THREADS="2 4 8 16 32"

# PHILOSOPHES

for t in $THREADS; do
    for j in 1 2 3 4 5; do
        echo "Philosophes : $t threads (run $j/5)"
        /usr/bin/time -f %e -o /tmp/measurements ./philosophe $t > /dev/null
        TIME=$(cat /tmp/measurements)
        echo "    → Temps: ${TIME}s"
        echo "$t,$(cat /tmp/measurements)" >> measures_philosophes.csv
    done
done

# LECTEURS & ECRIVAINS

for t in $THREADS; do
    lec=$((t/2))
    ecr=$((t/2))

    for j in 1 2 3 4 5; do
        echo "Lecteurs/Ecrivains : $t total → $lec lecteurs / $ecr écrivains (run $j/5)"
        /usr/bin/time -f %e -o /tmp/measurements ./lecteurs_ecrivains $ecr $lec > /dev/null
        TIME=$(cat /tmp/measurements)
        echo "    → Temps: ${TIME}s"
        echo "$t,$(cat /tmp/measurements)" >> measures_rw.csv
    done
done

# PRODUCTEURS & CONSOMMATEURS

for t in $THREADS; do
    prod=$((t/2))
    cons=$((t/2))

    for j in 1 2 3 4 5; do
        echo "Prod/Cons : $t total → $prod producteurs / $cons consommateurs (run $j/5)"
        /usr/bin/time -f %e -o /tmp/measurements ./producteurs_consommateurs $prod $cons > /dev/null
        TIME=$(cat /tmp/measurements)
        echo "    → Temps: ${TIME}s"
        echo "$t,$(cat /tmp/measurements)" >> ./measures/measures_producteurs_consommateurs.csv
    done
done

echo "Mesures terminées"
