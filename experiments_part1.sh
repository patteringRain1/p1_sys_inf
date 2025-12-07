#!/bin/bash
#
# Script d'expérimentation permettant de mesurer le temps d’exécution
# de trois programmes concurrents :
#   - philosophes          (verrous test-and-set / test-and-test-and-set)
#   - lecteurs_ecrivains   (RW problem)
#   - producteurs_consommateurs
#
# Fonctionnement :
#   - Les tests sont effectués pour 5 tailles de charge : 2, 4, 8, 16, 32 threads.
#   - Chaque configuration est exécutée 5 fois afin d'obtenir des mesures
#     plus fiables.
#   - Le temps "réel" (format %e) fourni par /usr/bin/time est capturé
#     dans /tmp/measurements puis ajouté dans les CSV.
#
# Fichiers générés :
#   - measures_philosophes.csv
#   - measures_rw.csv
#   - measures_producteurs_consommateurs.csv
#
# Structure des CSV :
#   threads,time
#   2,0.123
#   2,0.119
#   ...
#
# Prérequis :
#   - Les exécutables philosophes, lecteurs_ecrivains et
#     producteurs_consommateurs doivent être compilés.
#   - Exécuter le script avec :
#         chmod +x experiments.sh
#         ./experiments.sh


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
