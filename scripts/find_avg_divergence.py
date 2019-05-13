#!/usr/bin/python3
import os
import math

champ_path = os.chdir("..") 

suite = input("Enter suite: ")

if suite == "spec":
	sim_list = open("sim_lists/spec/comp_traces.txt")
else:
	sim_list = open("sim_lists/" +suite+"/traces.txt")

result = dict()

if suite == "spec":
	divergence = open("output/divergence_spec.csv")
elif suite == "gap-g15":
	divergence = open("output/gap15_divergence.csv")
elif suite == "gap-g17":
	divergence = open("output/gap17_divergence.csv")

elif suite == "gap-g20":
	divergence = open("output/gap20_divergence.csv")

elif suite == "gap-g22":
	divergence = open("output/gap22_divergence.csv")
elif suite == "cloudsuite":
	divergence = open("output/cloudsuite_divergence.csv")

print("bench_name, average correlated pages for 99% pages")
for bench in sim_list:
	bench_name = bench[:-1]
	if suite == "cloudsuite":
		bench_name = bench_name[:-6] +"_cpu0"

	total_instances = int(0)
	divergence.seek(0)
	for line in divergence:
		if line.startswith('bench_name'):
			continue
		if not line.startswith(bench_name):
			continue	
		total_instances += int(line.split()[2])

	ninety_nine_percent = float((total_instances) * 0.99)
	instances_seen = float(0)
	result = float(0)
	divergence.seek(0)
	for line in divergence:
		if line.startswith('bench_name'):
			continue
		if not line.startswith(bench_name):
			continue	
		instances_seen += float(line.split()[2])
		result += float(line.split()[1][:-1]) * float(line.split()[2])
		if (instances_seen >= ninety_nine_percent):
			result = math.ceil(result / instances_seen);
			print('%s, %f' % (bench_name, result))
			break






