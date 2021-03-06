#!/bin/bash
GPU=false

SIGNATURE="spec"

TRACE_PATH="/scratch/cluster/akanksha/CRCRealTraces"
CHAMP_PATH="$(pwd)/.."
BRANCH="perceptron"
L1P="no"
PREFETCH="${1}"
CACHE="lru"
CORES=1

# build champsim
cd $CHAMP_PATH
./build_champsim.sh ${BRANCH} ${L1P} ${PREFETCH} ${CACHE} ${CORES}

WARM_INS=0
INS=1000
OUTPUT_DIR="output/$SIGNATURE/${PREFETCH}"

# ensure output dir exists
if test ! -d $OUTPUT_DIR; then
    mkdir $OUTPUT_DIR
fi

while read line ;
do
    BENCHMARK=$line
    SCRIPT_FILE="$OUTPUT_DIR/${BENCHMARK}.sh"
    CONDOR_FILE="$OUTPUT_DIR/$BENCHMARK.condor"
    
    # create script file
    echo "#!/bin/bash" > $SCRIPT_FILE
    echo "$CHAMP_PATH/scripts/run_champsim.sh ${BRANCH}-${L1P}-${PREFETCH}-${CACHE}-${CORES}core $WARM_INS $INS $BENCHMARK $OUTPUT_DIR $TRACE_PATH" >> $SCRIPT_FILE
    chmod +x $SCRIPT_FILE

    # create condor file
    /u/matthewp/research/scripts/condorize.sh $GPU $OUTPUT_DIR $BENCHMARK
    
    # submit the condor file
    /lusr/opt/condor/bin/condor_submit $CONDOR_FILE
done < $CHAMP_PATH/sim_lists/spec/comp_traces.txt
