#!/bin/bash
binary=${1}
n_warm=${2}
n_sim=${3}
num=${4}
outputdir=${5}
option=${6}
TRACE_DIR=${7}
CHAMP_PATH=${8}
TRACE_LIST="$CHAMP_PATH/sim_lists/cloudsuite/4core_workloads.txt"


trace1=`sed -n ''$num'p' $TRACE_LIST | awk '{print $1}'`
trace2=`sed -n ''$num'p' $TRACE_LIST | awk '{print $2}'`
trace3=`sed -n ''$num'p' $TRACE_LIST | awk '{print $3}'`
trace4=`sed -n ''$num'p' $TRACE_LIST | awk '{print $4}'`

echo "(${CHAMP_PATH}/bin/${binary} -hide_heartbeat -warmup_instructions ${n_warm}000000 -simulation_instructions ${n_sim}000000 -${option} -traces ${TRACE_DIR}/${trace1}.trace.gz ${TRACE_DIR}/${trace2}.trace.gz ${TRACE_DIR}/${trace3}.trace.gz ${TRACE_DIR}/${trace4}.trace.gz) &> ${outputdir}/mix${num}.txt"
(${CHAMP_PATH}/bin/${binary} -hide_heartbeat -warmup_instructions ${n_warm}000000 -simulation_instructions ${n_sim}000000 -${option} -traces ${TRACE_DIR}/${trace1}.trace.gz ${TRACE_DIR}/${trace2}.trace.gz ${TRACE_DIR}/${trace3}.trace.gz ${TRACE_DIR}/${trace4}.trace.gz) &> ${outputdir}/mix${num}.txt
