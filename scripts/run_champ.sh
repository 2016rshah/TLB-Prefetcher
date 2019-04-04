#!/bin/bash

BENCH=$(echo "${3}" | cut -d'.' -f 1)

if [ -z "$4" ]; then
    /u/matthewp/research/champ/scripts/run_champsim.sh perceptron-no-${1}-lru-1core 0 $2 $BENCH . /scratch/cluster/akanksha/CRCRealTraces 
else 
    /u/matthewp/research/champ/scripts/run_champsim.sh perceptron-no-${1}-lru-1core 0 $2 $BENCH . low_bandwidth /scratch/cluster/akanksha/CRCRealTraces 
fi

