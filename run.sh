#!/bin/bash

# Check that args are correct
if [ "$#" -ne 1 ]; then
    echo "Incorrect # of arguments: expected tlb_prefetcher policy"
    echo "Usage: ./run.sh name_of_policy"
    exit
fi

# Simulation parameters
TRACE_DIR="/u/kbaldauf/Documents/comp_arch_research/TLB-Prefetcher/traces"
WARM_INS=0
SIM_INS=10
TLB_PREFETCHER=${1}
CACHE="lru"

BINARY="bin/perceptron-no-no-${TLB_PREFETCHER}-lru-1core"

# Run our cache replacement policy on each trace
echo "Running ${CACHE} on astar_163B ..."
(${BINARY} -warmup_instructions ${WARM_INS}000000 -simulation_instructions ${SIM_INS}000000 -hide_heartbeat -traces ${TRACE_DIR}/astar_163B.trace.gz) &> output/astar_163B.txt

echo "Running ${CACHE} on libquantum_1210B ..."
(${BINARY} -warmup_instructions ${WARM_INS}000000 -simulation_instructions ${SIM_INS}000000 -hide_heartbeat -traces ${TRACE_DIR}/libquantum_1210B.trace.gz) &> output/libquantum_1210B.txt

