import nautilus
import os
import urllib
import sys
from threading import Thread

class RunThread(Thread):
	
	def __init__(self, filename):
		
		Thread.__init__(self)
		self.filename = filename
	
	def run(self):
		
		os.popen('timeline '+self.filename)

class RunThread2(Thread):
	
	def __init__(self):
		
		Thread.__init__(self)
	
	def run(self):
		
		os.popen('piechart')

class RVFS(nautilus.MenuProvider):
    
	# Nautilus crashes if a plugin doesn't implement the __init__ method.
	# See Bug #374958
	def __init__(self):
		pass
	
	def show_timeline(self, menu, filename):
		
		f = file('/tmp/log','w')
		f.write('Called timeline with '+filename)
		f.close()
		RunThread(filename).start()
	
	def show_disk_usage(self, menu, filename):
		
		f = file('/tmp/log','a')
		f.write('Called Perf with '+filename)
		f.close()
		RunThread2().start()

	def get_file_items(self, window, files):
		f = file('/tmp/test','w')
		f.write(str(files[0].get_uri_scheme()))
		f.close()

		if len(files)>1:
			return
		if files[0].is_directory():
			return
			
		if files[0].get_uri_scheme() != 'file':
			top_menuitem = nautilus.MenuItem('NautilusPython::RVFS', 'RVFS', '')
			submenu = nautilus.Menu()
			top_menuitem.set_submenu(submenu)
			sub_menuitem2 = nautilus.MenuItem('NautilusPython::su', 'Disk Usage','')
			submenu.append_item(sub_menuitem2)
			return
			
		filename = urllib.unquote(files[0].get_uri()[7:])
		

		test_command = 'file -i -b '+filename+' | cut -d \/ -f 1'
		a = os.popen(test_command)
		res = a.read().strip()
		if res != 'text' and res != 'empty':
			return
		
		top_menuitem = nautilus.MenuItem('NautilusPython::RVFS', 'RVFS', '')
		submenu = nautilus.Menu()
		top_menuitem.set_submenu(submenu)
		
		sub_menuitem = nautilus.MenuItem('NautilusPython::st', 'Versions', '')
		submenu.append_item(sub_menuitem)

		sub_menuitem.connect('activate',self.show_timeline, filename)

		return top_menuitem,

