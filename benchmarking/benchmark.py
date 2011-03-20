import random
import os
import time
# Benchmarks the files

# Parameters
S_SIZE_L = 20*1024
S_SIZE_H = 1024*1024
M_SIZE_L = 1024*1024
M_SIZE_H = 1*1024*1024
L_SIZE_L = M_SIZE_H
L_SIZE_H = 2*1024*1024

S_BR_L = 3
S_BR_H = 5
M_BR_L = 25
M_BR_H = 35
L_BR_L = 10
L_BR_H = 100

S_DIFF_L = 1
S_DIFF_H = 10
M_DIFF_L = 10
M_DIFF_H = 30
L_DIFF_L = 30
L_DIFF_H = 60

S_SIZE = 0
M_SIZE = 1
L_SIZE = 2

S_BR = 0
M_BR = 1
L_BR = 2

S_DIFF = 0
M_DIFF = 1
L_DIFF = 2

# Prefix for the work to be done

PREFIX = '/home/prashant/Downloads/timeline/testing/mountdir/'

# Total nodes in any tree

NODES_L = 10
NODES_H = 50

class Node:
	
	def __init__(self, parent, id):
		
		self.parent = parent
		self.children = []
		self.id = id
		self.timestamp = -1
	
	def add_child(self, child):
		
		self.children.append(child)
	
	def printMe(self):
		if self.parent:
			print '<'+str(self.id)+'>'+' parent '+'<'+str(self.parent.id)+'>'
		else:
			print '<'+str(self.id)+'>'+' parent '+'<root>'


class Tree:
	
	def __init__(self, treename, bt, size):
		
		self.treename = treename
		self.bt = bt
		self.heads = []
		self.nodes = []

		if self.bt == S_BR:
			self.BR_L = S_BR_L
			self.BR_H = S_BR_H

		elif self.bt == M_BR:
			self.BR_L = M_BR_L
			self.BR_H = M_BR_H
	
		elif self.bt == L_BR:
			self.BR_L = L_BR_L
			self.BR_H = L_BR_H
	
		self.size = size
		self.printStats()

	def visualize(self, root):
		
		# visualizes the tree using himmeli
		print 'Writing edges to edges.txt'
		f = file('graphs/edges.txt','w')
		f.write('HEAD\tTAIL\n')

		queue = [root]

		while len(queue) > 0:

			node = queue[0]
			queue.remove(node)
			if(node.parent):
				f.write('a'+str(node.id)+'\ta'+str(node.parent.id)+'\n')

			queue = list(set(queue) | set(node.children))

		f.close()
		os.system('cd graphs; himmeli config.txt; cd ..')


		

	def printStats(self):
		
		print 'Size: '+str(self.size)
		print 'Type: '+str(self.bt)
		print 'Bound of tree: '+str(self.BR_L)+' '+str(self.BR_H)
	

	def construct(self):
		
		# Construct the tree

		nodes = 0

		root = Node(None, 0) # construct the root
		present_head = root
		self.heads.append(root)
		self.nodes.append(root)

		while nodes < self.size:
			prob = random.randint(1,100)
			if prob <self.BR_H:
				
				# Do branching
				branch_list = list(set(self.nodes) - set(self.heads))

				if len(branch_list) != 0:
					
					branch_node = branch_list[random.randint(0,len(branch_list)-1)]
					nodes = nodes + 1
					new_node = Node(branch_node, nodes)
					branch_node.children.append(new_node)
					self.heads.append(new_node)
					self.nodes.append(new_node)


			else:
				
				# Dont do branching
				head_node = self.heads[random.randint(0, len(self.heads)-1)]
				nodes = nodes+1
				new_node = Node(head_node, nodes)
				head_node.children.append(new_node)
				self.heads.remove(head_node)
				self.nodes.append(new_node)
				self.heads.append(new_node)

		return root

	def printTree(self, root):
			
		queue = [root]

		while len(queue) > 0:

			node = queue[0]
			queue.remove(node)
			node.printMe()

			queue = list(set(queue) | set(node.children))


class FileHandle:
	
	def __init__(self, filename, st, bt, dt, nodes):
		
		self.filename = filename
		self.st = st
		self.bt = bt
		self.dt = dt
		self.nodes = nodes
		
		if dt == S_DIFF:
			
			self.DIFF_L = S_DIFF_L
			self.DIFF_H = S_DIFF_H

		elif dt == M_DIFF:
			
			self.DIFF_L = M_DIFF_L
			self.DIFF_H = M_DIFF_H

		else:
			self.DIFF_L = L_DIFF_L
			self.DIFF_H = L_DIFF_H

		if st == S_SIZE:
			
			self.SIZE_L = S_SIZE_L
			self.SIZE_H = S_SIZE_H

		elif st == M_SIZE:
			
			self.SIZE_L = M_SIZE_L
			self.SIZE_H = M_SIZE_H

		else:
			self.SIZE_L = L_SIZE_L
			self.SIZE_H = L_SIZE_H


		self.size = random.randint(self.SIZE_L, self.SIZE_H)
		print 'Actual size: '+str(self.size)
			
	
	def genRandomInput(self):
		
		f = file(PREFIX+self.filename,'w')
		i = 0
		while i < self.size:
			for j in range(20):
				f.write(chr(65+random.randint(1,25)))
				i = i + 1
			f.write('\n')

		f.close()
			
	def patchDiff(self):
		
		# takes the file inside temp, modifies
		# it to a suitable diff

		f = file(PREFIX+self.filename,'a')
	
		diff_val = int((random.randint(self.DIFF_L, self.DIFF_H)*self.size)/100)
		print 'Diff for present patch: '+str(diff_val)
		i = 0
		while i < diff_val:
			for j in range(20):
				f.write(chr(65+random.randint(1,25)))
				i = i  + 1
			f.write('\n')

		f.close()

	def genFile(self):
		
		# Construct a random file with given file size
		
		self.genRandomInput()		

		# Construct a sample tree file

		t = Tree('TestTree', self.bt, self.nodes)
		root = t.construct()
		t.visualize(root)


		# Traverse the tree inserting the various diffs, and reverting etc
		# In essence do a DFS

		stck = [root]
		last = None
		while(len(stck)>0):
			time.sleep(3)
			top = stck.pop()

			# check if the new node is an extension
			# if not then you need to checkout old branch

			if not last or (last and top.parent == last):
				
				# we have a simple extension
				# generate a diff file and move to rvfs
				self.patchDiff()
				print 'Patching '+ str(top.id)
				ts_d = os.popen('lsver '+PREFIX+self.filename+' | grep IST | cut -d \  -f 1 | sort -r | head -n 1')
				ts = int(ts_d.read())
				top.timestamp = ts
				print 'Patch timestamp '+str(ts)

			else:
				# 1. checkout 2. extend

				# system command for checking out( need to parse the snapshot damn! )
				os.system('checkout '+PREFIX+self.filename+' '+str(top.parent.timestamp))
				# patch the diff
				self.patchDiff()
				print 'Checkout '+str(top.parent.id) + ' and patch '+str(top.id)
				ts_d = os.popen('lsver '+PREFIX+self.filename+' | grep IST | cut -d \  -f 1 | sort -r | head -n 1')
				ts = int(ts_d.read())
				top.timestamp = ts
				
			# update the last
			last = top

			# insert the elements inside the stack again
			for c in top.children:
				stck.append(c)

if __name__=='__main__':
	random.seed(time.time())	
	# t = Tree('TestTree', S_BR, 250)
	# root = t.construct()
	# t.printTree(root)
	# t.visualize(root)
	# size, branches, diffs, no. of nodes
	file_handle = FileHandle('testfile', M_SIZE, S_BR, S_DIFF, 20)
	file_handle.genFile()

