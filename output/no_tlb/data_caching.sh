#!/bin/bash
/u/vkarthik/Documents/cs378_arch/tlb_prefetcher/bin/perceptron-no-no-gac-lru-1core -warmup_instructions 0000000 -simulation_instructions 100000000 -hide_heartbeat -traces /scratch/cluster/akanksha/CRCRealTraces/data_caching.trace.gz &> /u/vkarthik/Documents/cs378_arch/tlb_prefetcher/output/lru/data_caching.txt
