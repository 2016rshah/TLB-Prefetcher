#!/bin/bash
for ((i = $1 ; i <= $2; i++))
do
    /lusr/opt/condor/bin/condor_rm $i
done
