#!/bin/bash

# Check that args are correct
if [ "$#" -ne 1 ]; then
    echo "Incorrect # of arguments: expected cache replacement policy"
    echo "Usage: ./run.sh name_of_policy"
    exit
fi

# Simulation parameters
TRACE_DIR="/u/matthewp/traces"
WARM_INS=0
SIM_INS=10
CACHE=${1}

BINARY="bin/perceptron-no-no-${CACHE}-1core"

# Run our cache replacement policy on each trace
echo "Running ${CACHE} on astar_163B ..."
(${BINARY} -warmup_instructions ${WARM_INS}000000 -simulation_instructions ${SIM_INS}000000 -hide_heartbeat -traces ${TRACE_DIR}/astar_163B.trace.gz) &> output/astar_163B.txt

