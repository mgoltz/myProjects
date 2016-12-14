import snap
import sys

# return list of degress created from file
def readGraphFile(fname):
	degSeq = []
	with open(fname) as f:
		lines = f.readlines()
		for l in lines:
			if "\t" in l:
				item = l.strip("\n").split("\t")
			else:
				item = l.strip("\n").split(" ")

			deg = int(item[0])
			print deg
			degSeq.append(deg)

	return degSeq

if len(sys.argv) < 2:
	print "Error: must specify graph file"

else:
	fname = sys.argv[1]

	degSeq = readGraphFile(fname)
	print degSeq