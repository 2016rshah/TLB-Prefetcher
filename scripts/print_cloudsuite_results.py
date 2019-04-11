#!/usr/bin/python3

import os

print('(name of benchmark, CPU, IPC)')
print('----------------------------------------')

champ_path = os.chdir("..") 
i = int(0)
trace_list = open('sim_lists/cloudsuite/4core_workloads.txt', 'r')
for trace in trace_list:
	i+=1
	bench_name = trace.split()[0][:-6]
	trace_output = open('output/cloudsuite/no/mix'+str(i)+'.txt', 'r')
	for line in trace_output:
		if line.startswith('Finished CPU 0 instructions:'):
			ipc_rate = float(line.split()[9])
			print('%s, 0, %.6f' % (bench_name, ipc_rate))
		elif line.startswith('Finished CPU 1 instructions:'):
			ipc_rate = float(line.split()[9])
			print('%s, 1, %.6f' % (bench_name, ipc_rate))
		elif line.startswith('Finished CPU 2 instructions:'):
			ipc_rate = float(line.split()[9])
			print('%s, 2, %.6f' % (bench_name, ipc_rate))
		elif line.startswith('Finished CPU 3 instructions:'):
			ipc_rate = float(line.split()[9])
			print('%s, 3, %.6f' % (bench_name, ipc_rate))
		
