#!/bin/bash
/u/vkarthik/Documents/cs378_arch/tlb_prefetcher/bin/perceptron-no-no-delta-lru-1core -warmup_instructions 0000000 -simulation_instructions 100000000 -hide_heartbeat -traces /scratch/cluster/akanksha/CRCRealTraces/astar_163B.trace.gz &> /u/vkarthik/Documents/cs378_arch/tlb_prefetcher/output/lru/astar_163B.txt
