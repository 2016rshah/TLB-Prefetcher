#!/usr/bin/python3

import os

print('(suite, name of benchmark, IPC, Prefetch Accuracy, Redundancy)')
print('----------------------------------------')

prefetcher = input("Enter prefetcher: ")
print('\n')
champ_path = os.chdir("..") 
suites = ["gap-g15", "gap-g17", "gap-g20"] #, "gap-g22"]

for suite in suites:

	sim_list = open('sim_lists/'+suite+'/traces.txt')
	for line in sim_list:
		bench_name = line[:-1]
		trace_output = open('output/'+suite+'/'+prefetcher+'/'+bench_name +'.txt', 'r')
		result = suite
		for output_line in trace_output:	
			if output_line.startswith('Finished CPU 0 instructions:'):
				ipc_rate = float(output_line.split()[9])
				result = result + ', '+ bench_name +', ' + str(ipc_rate)
			elif output_line.startswith('STLB PREFETCH  REQUESTED:'):
				issued = float(output_line.split()[5])
				useful = float(output_line.split()[9])
				if issued != 0 and useful != 0:
					accuracy = float(useful/issued) * 100
					result = result + ', ' + str(accuracy)
				else:
					result = result + ', 0.0'
			elif output_line.startswith('\tredundant prefetches vs'):
				redundant = float(output_line.split()[5])
				total = float(output_line.split()[7])
				if redundant != 0 and total != 0:	
					redundancy_percent = float(redundant / total) * 100
					result = result + ', ' + str(redundancy_percent) 
				else:
					result = result + ', 0.0'	

		print(result)
