#!/usr/bin/python3

import os

print('(name of benchmark, CPU, IPC, Prefetch Accuracy)')
print('----------------------------------------')

prefetcher = input("Enter prefetcher: ")

champ_path = os.chdir("..") 
i = int(0)

trace_list = open('sim_lists/cloudsuite/4core_workloads.txt', 'r')
for trace in trace_list:
	i+=1
	results = dict()
	cpu = int(0)
	bench_name = trace.split()[0][:-6]
	trace_output = open('output/cloudsuite/'+prefetcher+'/mix'+str(i)+'.txt', 'r')
	for line in trace_output:
		if line.startswith('Finished CPU 0 instructions:'):
			ipc_rate = float(line.split()[9])
			results[0] = bench_name +', 0, ' + str(ipc_rate)
			# print('%s, 0, %.6f' % (bench_name, ipc_rate))
		elif line.startswith('Finished CPU 1 instructions:'):
			ipc_rate = float(line.split()[9])
			results[1] = bench_name + ', 1, ' + str(ipc_rate)
			#print('%s, 1, %.6f' % (bench_name, ipc_rate))
		elif line.startswith('Finished CPU 2 instructions:'):
			ipc_rate = float(line.split()[9])
			results[2] = bench_name + ', 2, ' + str(ipc_rate)
			#print('%s, 2, %.6f' % (bench_name, ipc_rate))
		elif line.startswith('Finished CPU 3 instructions:'):
			ipc_rate = float(line.split()[9])
			results[3] = bench_name + ', 3, ' + str(ipc_rate)
			# print('%s, 3, %.6f' % (bench_name, ipc_rate))
		elif line.startswith('STLB PREFETCH  REQUESTED:'):
			issued = float(line.split()[5])
			useful = float(line.split()[9])
			accuracy = float(useful/issued) * 100
			results[cpu] = results[cpu] + ', ' + str(accuracy)
			cpu += 1
	
	for key, value in results.items():
		print(value)	
