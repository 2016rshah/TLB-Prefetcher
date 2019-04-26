#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "Incorrect # of arguments: expected tlb prefetcher policy"
	echo "Usage: ./run_gap.sh tlb_prefetcher"
	exit
fi

GPU=false

CHAMP_PATH="$(pwd)/.."
BRANCH="perceptron"
L1P="no"
L2P="no"
TLB_PREFETCHER="${1}"
CACHE="lru"
CORES=1

# build champsim
cd $CHAMP_PATH

WARM_INS=0
INS=1000

for SUITE in "gap-g15" "gap-g17" "gap-g20" "gap-g22"
do 
	TRACE_PATH="/scratch/cluster/matthewp/$SUITE"
	OUTPUT_DIR="${CHAMP_PATH}/output/$SUITE/${TLB_PREFETCHER}"
	if test ! -d $OUTPUT_DIR; then
		mkdir -p $OUTPUT_DIR
	fi 
	
	while read line ;
	do
		BENCHMARK=$line
    	SCRIPT_FILE="$OUTPUT_DIR/${BENCHMARK}.sh"
    	CONDOR_FILE="$OUTPUT_DIR/$BENCHMARK.condor"
    
    	# create script file
    	echo "#!/bin/bash" > $SCRIPT_FILE
    	echo "$CHAMP_PATH/scripts/run_champsim.sh ${BRANCH}-${L1P}-${L2P}-${TLB_PREFETCHER}-${CACHE}-${CORES}core $WARM_INS $INS $BENCHMARK $OUTPUT_DIR $TRACE_PATH" >> $SCRIPT_FILE
    	chmod +x $SCRIPT_FILE

    	# create condor file
    	/u/matthewp/research/scripts/condorize.sh $GPU $OUTPUT_DIR $BENCHMARK
    
    	# submit the condor file
    	/lusr/opt/condor/bin/condor_submit $CONDOR_FILE
	done < $CHAMP_PATH/sim_lists/$SUITE/traces.txt
done
