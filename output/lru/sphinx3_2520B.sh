#!/bin/bash
/u/vkarthik/Documents/cs378_arch/tlb_prefetcher/bin/perceptron-no-no-lru-1core -warmup_instructions 0000000 -simulation_instructions 100000000 -hide_heartbeat -traces /scratch/cluster/akanksha/CRCRealTraces/sphinx3_2520B.trace.gz &> /u/vkarthik/Documents/cs378_arch/tlb_prefetcher/output/lru/sphinx3_2520B.txt
