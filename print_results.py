#!/usr/bin/python3
import sys

if len(sys.argv) < 2:
    print('Need to specify a cache replacement policy!')
    print('Example usage: ./print_results.py my_policy')
    sys.exit(0)

policy = sys.argv[1]

print('Replacement Policy Performance')
print('(IPC, MPKI, name of benchmark)')
print('------------------------------')
mpki_list = []
ipc_list = []
trace_list = open('sim_list/traces.txt', 'r')
for trace in trace_list:
    trace = trace[:-1]
    trace_output = open('output/'+policy+'/'+trace+'.txt', 'r')
    mpki = 0.0
    ipc = 0.0
    for line in trace_output:
        if line.startswith('LLC TOTAL'):
            mpki = float(line.split()[7])/100000.0
        elif line.startswith('LLC WRITEBACK'):
            wb_miss = float(line.split()[7])/100000.0
            mpki -= wb_miss
        elif line.startswith('CPU 0 cumulative IPC'):
            ipc = float(line.split()[4])
    mpki_list.append(mpki)
    ipc_list.append(ipc)
    print('%.3f %2.1f %s' % (ipc, mpki, trace))

avg_mpki = sum(mpki_list)/len(mpki_list)
avg_ipc = sum(ipc_list)/len(ipc_list)
print('%.3f %2.1f Average' % (avg_ipc, avg_mpki))
