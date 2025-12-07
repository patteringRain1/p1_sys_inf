#!/bin/bash
#
# Script d'expérimentation pour mesurer les performances des verrous
# test-and-set (TAS) et test-and-test-and-set (TTAS).
#
# Fonctionnement :
#   - Exécute les programmes ./test_and_set et ./test_and_test_and_set
#     pour plusieurs nombres de threads (1, 2, 4, 8, 16, 32).
#   - Chaque configuration est répétée 5 fois afin d’obtenir des mesures
#     plus stables.
#   - Le temps d’exécution "real" (en secondes) est extrait avec 'time'
#     puis formaté et enregistré dans deux fichiers CSV :
#           - test_and_set.csv
#           - test_and_test_and_set.csv
#
# Sortie :
#   - Chaque fichier CSV contient deux colonnes : threads,time
#   - Ces fichiers sont ensuite utilisés pour générer les graphiques
#     via le script Python (moyenne et écart-type).
#
# Usage :
#   ./experiments.sh
#
# Pré-requis :
#   - Les exécutables test_and_set et test_and_test_and_set doivent être
#     compilés et présents dans le même dossier.
#   - Le script doit être exécutable :
#         chmod +x experiments.sh


THREADS=(1 2 4 8 16 32)
CSV_TS="test_and_set.csv"
CSV_TTS="test_and_test_and_set.csv"

# Create CSV header
echo "threads,time" > "$CSV_TS"
echo "threads,time" > "$CSV_TTS"

# Test-and-set measurements

for t in "${THREADS[@]}"; do
    for j in 1 2 3 4 5; do
        echo "Test-and-set : $t threads (run $j/5)"
        TIME=$( { time ./test_and_set $t > /dev/null; } 2>&1 | grep real | awk '{print $2}' | sed 's/[0-9]*m//' | sed 's/s//' )
        echo "    → Temps: ${TIME}s"
        echo "$t,$TIME" >> "$CSV_TS"
    done
done

# Test-and-test-and-set measurements

for t in "${THREADS[@]}"; do
    for j in 1 2 3 4 5; do
        echo "Test-and-test-and-set : $t threads (run $j/5)"
        TIME=$( { time ./test_and_test_and_set $t > /dev/null; } 2>&1 | grep real | awk '{print $2}' | sed 's/[0-9]*m//' | sed 's/s//' )
        echo "    → Temps: ${TIME}s"
        echo "$t,$TIME" >> "$CSV_TTS"
    done
done

echo "Mesures terminées.
