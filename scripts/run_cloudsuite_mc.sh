#!/bin/bash
if [ $# -lt 1 ] 
then
    echo "Usage : ./run_all_sims.sh <binary> <output>"
    exit
fi

binary=$1
echo $binary
output=$2
echo $output

for f in /scratch/cluster/akanksha/CloudSuiteTraces/*core0.trace.gz
do
    benchmark=$(basename "$f")
    benchmark="${benchmark%.*}"
    benchmark="${benchmark%.*}"
    benchmark="${benchmark%_*}"
    trace_file=$benchmark
    echo $f $benchmark 

    output_dir="/scratch/cluster/akanksha/CRCRealOutput/4coreCRC/cloudsuite_ARM/$output/"

    if [ ! -e "$output_dir" ] ; then
        mkdir $output_dir
        mkdir "$output_dir/scripts"
    fi
    condor_dir="$output_dir"
    script_name="$benchmark"

    #echo $output_dir
    #echo $trace_file
    command="/u/akanksha/MyChampSim/ChampSim/scripts/run_champsim_cloudsuite_4core.sh $binary 50 50 $trace_file $output_dir"

    /u/akanksha/cache_study/condor_shell --silent --log --condor_dir="$condor_dir" --condor_suffix="$benchmark" --output_dir="$output_dir/scripts" --simulate --script_name="$script_name" --cmdline="$command"

        #Submit the condor file
     /lusr/opt/condor/bin/condor_submit $output_dir/scripts/$script_name.condor

done
