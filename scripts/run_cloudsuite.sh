#!/bin/bash
GPU=false

SIGNATURE="cloudsuite"

if [ "$#" -ne 1 ]; then
	echo "Incorrect # of arguments: expected tlb prefetcher policy"
	echo "Usage: ./run_cloudsuite.sh tlb_prefetcher"
	exit
fi

TRACE_PATH="/scratch/cluster/akanksha/CloudSuiteTraces"
CHAMP_PATH="$(pwd)/.."
BRANCH="perceptron"
L1P="no"
L2P="no"
TLB_PREFETCHER=${1}
CACHE="lru"
CORES=4

# build champsim
cd $CHAMP_PATH

WARM_INS=0
INS=100

#OUTPUT_DIR="/scratch/cluster/kbaldauf/output/$SIGNATURE/${TLB_PREFETCHER}"
OUTPUT_DIR="${CHAMP_PATH}/output/$SIGNATURE/${TLB_PREFETCHER}"

# ensure output dir exists
if test ! -d $OUTPUT_DIR; then
    mkdir -p $OUTPUT_DIR
fi

num=0

while read line ;
do
    num=$(($num+1))
	BENCHMARK=$line
    SCRIPT_FILE="$OUTPUT_DIR/$BENCHMARK.sh"
    CONDOR_FILE="$OUTPUT_DIR/$BENCHMARK.condor"
    
    # create script file
    echo "#!/bin/bash" > $SCRIPT_FILE
    echo "$CHAMP_PATH/scripts/run_4core.sh ${BRANCH}-${L1P}-${L2P}-${TLB_PREFETCHER}-${CACHE}-${CORES}core $WARM_INS $INS $num $OUTPUT_DIR cloudsuite $TRACE_PATH $CHAMP_PATH" >> $SCRIPT_FILE
    chmod +x $SCRIPT_FILE

    # create condor file
    /u/matthewp/research/scripts/condorize.sh $GPU $OUTPUT_DIR $BENCHMARK
    
    # submit the condor file
    /lusr/opt/condor/bin/condor_submit $CONDOR_FILE
done < $CHAMP_PATH/sim_lists/$SIGNATURE/traces.txt
