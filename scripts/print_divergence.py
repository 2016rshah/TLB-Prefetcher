#!/usr/bin/python3

import os
champ_path = os.chdir("..") 

suite = input('Enter the benchmark suite: ') 
print('\n')

file_path='sim_lists/'+suite+'/traces.txt'

if suite == 'spec':
	file_path='sim_lists/'+suite+'/comp_traces.txt'

i = int(1)
sim_list = open(file_path)
for bench in sim_list:
	bench_name = bench[:-1]

	if "gap" in suite:
		output= open('output/'+suite+'/divergence_test/'+bench_name+'.txt', 'r')
	elif "cloudsuite" in suite:
		output= open('output/'+suite+'/divergence_test/mix'+str(i)+'.txt', 'r')
		i+= 1
	else:
		output= open('output/divergence_test/'+bench_name+'.txt', 'r')		
	
	# print(bench_name+':\n')
	iterator = iter(output)
	cpu = int(0)
	for line in iterator:
		if line.startswith('#correlated pages'):
			for line in iterator:
				if cpu >= 4:
					break;
				if line.startswith('DRAM') or line.startswith('LLC') or line.startswith('TLB prefetcher')  or line == '\n':
					cpu += 1
					break
				else:
					if suite == "cloudsuite":
						print('%s, %s' % (bench_name[:-6] +'_cpu' +str(cpu), line[:-1])) 
					else:
						print('%s, %s' % (bench_name,line[:-1] ))
	

