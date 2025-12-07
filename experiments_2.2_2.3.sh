#!/bin/bash

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
