# Michael Goltz
# EdgeTech Programming Screen
# Problem 2 - Given a number N, print a "special" NxN multiplication table
#		where any number that contains a 5 is replaced with the letter 'k'.

# Time complexity of O(n^2) as it calculates every multiplication combo
# Memory complexity is small; only tmpString constains each row of multiplication results
def doubleNestedMultiplication(N):
	for i in range(1,N+1):
		myString = ""
		for j in range(1,N+1):
			tmpString = str(i*j)
			# evaluate multiplication result's digits to see if it has a '5'
			for k in range(len(tmpString)): # for each digit
				digit = int(tmpString[k])
				if ((digit%5 == 0) & (digit != 0)): # see if it is divisible by 5, has no remainder, and not 0
					tmpString = 'k'
					break
			myString = myString + tmpString.rjust(4)
				
		print  myString

def singleLoopMultiplication(N):
	row = 1
	col = 1

	# loop through N*N 
	for i in range(1,N*N+1):
		tmpString = str(row*col)
		# evaluate multiplication result's digits to see if it has a '5'
		for k in range(len(tmpString)): # for each digit
			digit = int(tmpString[k])
			if ((digit%5 == 0) & (digit != 0)): # see if it is divisible by 5, has no remainder, and not 0
				tmpString = 'k'
				break
		print tmpString.rjust(3),
		col+=1
		if (i % N == 0):
			print  " "
			row+=1
			col = 1

def saveResults(N):

	saved = [] # see notes about size N*(N/2)
	tmpString = ""
	# looop through N and save duplicated result
	for i in range(0,N):
		myString = ""
		for j in range(0,N):
			tmpString = str((i+1)*(j+1))
			# evaluate multiplication result's digits to see if it has a '5'
			if i <= j:
				for k in range(len(tmpString)): # for each digit
					digit = int(tmpString[k])
					if ((digit%5 == 0) & (digit != 0)): # see if it is divisible by 5, has no remainder, and not 0
						tmpString = 'k' # k = -1
						break
				saved.append(tmpString)
			else: # get saved result
				tmpString = saved[j*N-j*(j+1)/2+i]

			myString = myString + tmpString.rjust(4)
				
		print  myString
	print "\nSaved results"
	print saved


if __name__ == "__main__":
	import sys
	N = int(sys.argv[1])

	print "Solution 1"
	doubleNestedMultiplication(N)
	print "\nSolution 2"
	singleLoopMultiplication(N)
	print "\nSolution 3"
	saveResults(N)