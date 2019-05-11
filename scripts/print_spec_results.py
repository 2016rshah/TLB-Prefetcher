#!/usr/bin/python3

import os

print('(suite, name of benchmark, IPC, Coverage, Prefetch Accuracy, Redundancy)')
print('----------------------------------------')

champ_path = os.chdir("..") 


sim_list = open('sim_lists/spec/comp_traces.txt')
for line in sim_list:
	bench_name = line[:-1]
	trace_output = open('output/lru/'+bench_name +'.txt', 'r')
	result = 'spec'
	for output_line in trace_output:	
		if output_line.startswith('Finished CPU 0 instructions:'):
			ipc_rate = float(output_line.split()[9])
			result = result + ', '+ bench_name +', ' + str(ipc_rate)
		elif output_line.startswith('STLB TOTAL'):
			no_tlb_output = open('output/no_tlb/'+bench_name +'.txt', 'r')
			missed_curr = float(output_line.split()[7])
			missed_naive = 0.0
			for line_naive in no_tlb_output:
				if line_naive.startswith('STLB TOTAL'):
					missed_naive = float(line_naive.split()[7])
			if missed_curr != 0 and missed_naive != 0:
				coverage = float((missed_naive - missed_curr)/missed_naive) * 100
				result = result + ', ' + str(coverage)
			else:
				result = result + ', 0.0'
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