i = 0
f = file('testfile','w')

while i < 1024*1024:
	
	for j in range(10):
		
		f.write('A')
		i = i + 1
	f.write('\n')
f.close()

	

