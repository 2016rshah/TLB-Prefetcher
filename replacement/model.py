import sys

filename = sys.argv[1]

trace_file = open(filename, 'r')
branches = {}
for line in trace_file:
	print line