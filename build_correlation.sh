#!/bin/bash

if [ "$#" -ne 4 ]; then
	echo "Incorrect # of arguments"
	echo "Usage: ./build_correlation.sh tlb_prefetcher num_cores S stream_index"
	exit
fi

BRANCH="perceptron"  # branch/*.bpred
L1D_PREFETCHER="no"   # prefetcher/*.l1d_pref
L2C_PREFETCHER="no"   # prefetcher/*.l2c_pref
TLB_PREFETCHER="${1}"
LLC_REPLACEMENT="lru"  # replacement/*.llc_repl
NUM_CORE=${2}            # tested up to 8-core system

S=${3} # The number of slots per TLB prefetcher entry
STREAM_START=${4}

if [ "$S" != 2 ] 
then
	echo "Updating S variable for '${TLB_PREFETCHER}' build"
	sed -i.bak 's/\<uint64_t S = 2\>/uint64_t S = '${S}'/g' prefetcher/${TLB_PREFETCHER}.tlb_pref
else
	echo "Using default variable for '${TLB_PREFETCHER}' build"
fi

if [ "$STREAM_START" != 1 ]
then
	echo "Updating the stream start position for '${TLB_PREFETCHER}' build"
	sed -i.bak 's/\<uint64_t stream_start = 1\>/uint64_t stream_start = '${STREAM_START}'/g' prefetcher/${TLB_PREFETCHER}.tlb_pref
fi

#./build.sh ${TLB_PREFETCHER} ${NUM_CORES}


sed -i.bak 's/\<uint64_t S = '${S}'\>/uint64_t S = 2/g' prefetcher/${TLB_PREFETCHER}.tlb_pref
sed -i.bak 's/\<uint64_t stream_start = '${STREAM_START}'\>/uint64_t stream_start = 1/g' prefetcher/${TLB_PREFETCHER}.tlb_pref


