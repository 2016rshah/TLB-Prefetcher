binary=${1}
n_warm=${2}
n_sim=${3}
trace=${4}
outputdir=${5}
option=${6}
TRACE_DIR=${7}
SCRIPT_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" > /dev/null 2>&1 && pwd )"

if [ -z "$7" ]; then
    (${SCRIPT_PATH}/../bin/${binary} -warmup_instructions ${n_warm}000000 -simulation_instructions ${n_sim}000000 -hide_heartbeat -traces ${option}/${trace}.trace.gz) &> ${outputdir}/${trace}.txt
else
    (${SCRIPT_PATH}/../bin/${binary} -warmup_instructions ${n_warm}000000 -simulation_instructions ${n_sim}000000 -hide_heartbeat -${option} -traces ${TRACE_DIR}/${trace}.trace.gz) &> ${outputdir}/${trace}.txt
fi


