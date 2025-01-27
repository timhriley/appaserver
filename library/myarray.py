#!/usr/bin/python
#---------------------------
# myarray.py
#
# The class array defines operations to store lists of items.
#
# Tim Riley
# 1/99
#----------------------------------------------------------

import sys, os, string, regsub, mydata, timlib

class myarray:
	# Instantiation section
	# ---------------------
	def __init__( self ):
		self.zap()

	# Set section
	# -----------
	def set( self, row, item ): return self.set_data( row, item )

	def array_string2string_array( self, array_string, delimiter ):
		return self.set_from_string( array_string, delimiter )

	def set_from_string( self, array_string, delimiter ):
		record = regsub.split( string.strip( array_string ), delimiter )
		for r in record: self.append( r )

	def process2string_array( self, process_string, debug_string ):
		return self.load_process( process_string, debug_string )

	def pipe2string_array( self, pipe_string ):
		return self.load_process( pipe_string, "" )

	def pipe2array( self, pipe_string, debug_string ):
		return self.load_process( pipe_string, debug_string )

	def load_from_pipe( self, pipe_string, debug_string ):
		return self.load_process( pipe_string, debug_string )

	def load_from_file( self, file_string ):
		return self.set_from_file( file_string )

	def set_from_pipe( self, pipe_string, debug_string ):
		return self.load_process( pipe_string, debug_string )

	def set_from_file_crypt( self, file_string, parameter_string ):
		sys_string = "cat %s 				|\
			      crypt %s				|\
			      grep -v '^#'" 			%\
			( file_string, parameter_string )
		return self.set_from_pipe( sys_string, "no" )

	def set_from_file( self, file_string ):
		f = open( file_string, "r" )

		for line in f.readlines():
			line_string = string.rstrip( line )
			self.append( line_string )
		f.close()

	def append_string_array( self, string_array ):
		for s in string_array: self.append( s )

	def set_data( self, row, item ):
		a = self.get( row )
		if ( a == '' ): a = mydata.mydata()
		a.set_data( item )
		self.set( row, a )

	def set( self, row, data ):
		self.make_room_maybe( row )
		self.remove_index( row )
		self.data_array.insert( row, data )
		self.current = row

	def append( self, item ):
		self.data_array.append( item )
		self.current = len( self.data_array ) - 1

	# insert is same as append
	# ------------------------
	def insert( self, item ):
		return self.append( item )

	# add_item is same as append
	# --------------------------
	def add_item( self, item ):
		self.append( item )

	# insert at the beginning
	# -----------------------
	def prepend( self, item ):
		self.data_array.insert( 0, item )
		self.current = 0

	def go_record( self, record_number ):
		self.current = record_number

	def find_data( self, search_data ):
		i = -1
		for data in self.data_array:
			i = i + 1
			if ( data == search_data ):
				self.current = i
				return i
		return -1

	def string_exists( self, data_string ):
		results = self.find_data( data_string )
		if ( results == -1 ):
			return 0
		else:
			return 1

	def go_head( self ):
		self.current = 0

	def go_tail( self ):
		self.current = self.num_items() - 1

	def load_process( self, process_string, debug ):
		if ( debug == 'yes' ):
			s = "%s\n" % process_string
			sys.stderr.write( s )

		p = os.popen( process_string, "r" )
		for line in p.readlines():
			# line = string.rstrip( line )
			self.append( string.rstrip( line ) )

		# Tim, check this later.
		# ----------------------
		return self

	# Operations
	# ----------
	def sort( self ): self.data_array.sort()

	def straddle_character( self, character_string ):
		i = 0
		for data in self.data_array:
			new_data = character_string +		\
				   data +			\
				   character_string
			self.set( i, new_data )
			i = i + 1

	def next_item( self ):
		self.current = self.current + 1
		return self.at_end()

	def to_file( self, filename_string ):
		f = open( filename_string, "w" )

		for data in self.data_array:
			output_string = "%s\n" % data
			f.write( output_string )
		f.close()

	def zap( self ):
		self.data_array = []
		self.current = 0

	def remove_index( self, index ):
		length = self.len()
		if ( length > 0 ):
			del self.data_array[ index ]

	def remove_data( self, data ):
		if ( data not in self.data_array ):
			return 0
		else:
			self.data_array.remove( data )
			return 1

	def delete_current( self ):
		length = self.len()
		if ( length > 0 ):
			self.remove_index( self.current )
			if ( self.current > 0 ):
				self.current = self.current - 1
				
	def delete_row( self, row ): return self.delete( row )

	def delete( self, row ):
		return self.remove_index( row )
				
	def make_room_maybe( self, row ):
		length = self.len()
		while( length <= row ): 
			self.data_array.append( 'null' )
			length = length + 1

	# Get section
	# -----------
	def get_tuple( self ): return tuple( self.data_array )

	def subtract( self, another_array ):
		results = myarray()
		self.go_head()
		while( self.at_end() == 0 ):
			data = self.get_current_data()
			if ( another_array.find_data( data ) == -1 ):
				results.append( data )
			self.next_item()
		return results

	def string2array( self, s, delimiter ):
		if ( s ):
			record = regsub.split( string.strip( s ), delimiter )
			for i in record:
				self.append( i )

	# with data wrapper
	# -----------------
	def data_array2string( self, delimiter ):
		s = ''
		first_time = 1
		for data in self.data_array:
			if ( first_time ):
				first_time = 0
			else:
				s = s + delimiter
			s = s + data.get()
		return s

	# w/o data wrapper
	# ----------------
	def array2string( self, delimiter ):
		s = ''
		first_time = 1
		for data in self.data_array:
			if ( first_time ):
				first_time = 0
			else:
				s = s + delimiter
			s = s + data
		return s


	def is_subset_of( self, another_array ):
		self.go_head()
		while( self.at_end() == 0 ):
			data = self.get_current_data()
			if ( another_array.find_data( data ) == -1 ):
				return 0
			self.next_item()
		return 1

	def is_empty( self ):
		if ( self.len() == 0 ):
			return 1
		else:
			return 0

	def len( self ):
		return len( self.data_array )

	def __len__( self ):
		return self.len()

	def __getitem__( self, i ):
		return self.data_array[ i ]

	def get_offset( self, offset ): return self.get( offset )

	def get( self, offset ):
		number_elements = len( self.data_array )
		if ( offset >= number_elements ): 
			return ''
		else:
			return self.data_array[ offset ]

	def get_data( self, data ):
		if ( self.find_data( data ) != -1 ):
			return self.data_array[ self.current ]
		else:
			return ''

	def get_current( self ):
		return self.current

	def get_index( self, index ):
		return self.data_array[ index ]

	def get_offset( self, index ): return self.get_index( index )

	def get_current_data( self ):
		return self.data_array[ self.current ]

	def get_raw( self ):
		return self.data_array

	def num_items( self ):
		return len( self.data_array )

	def at_end( self ):
		num_items = self.num_items()
		if ( self.current >= num_items ):
			return 1
		else:
			return 0

	# Display section
	# ---------------
	def display( self ):
		return self.array2string( ',' )

	def display_count_more( self ):
		process_string = "count.e 1 | more"
		p = os.popen( process_string, "w" )
		for line_string in self.data_array:
			line_string = line_string + '\n'
			p.write( line_string )
		p.close()

	# Output section
	# --------------
	def send_to_pipe( self, pipe_string ):
		p = os.popen( pipe_string, "w" )
		for line_string in self.data_array:
			p.write( line_string + '\n' )
		p.close()

# end class myarray
# -----------------

def do_test1():
	l = myarray()
	l.append( 'tim' )
	l.append( 'jim' )
	results = l.find_data( 'tim' )
	print "checking tim exists: should be >= 0 is %d" % results

	l.go_head()
	while( l.at_end() == 0 ):
		first_name = l.get_current_data()
		print "first = (%s)" % ( first_name )
		l.next_item()

	print "display = %s" % l.display()
	l.remove_data( 'jim' )
	print "after remove jim = %s" % l.display()

	m = myarray()
	m.append( 'george' )
	first_name = m[ 0 ]
	print "first = (%s)" % ( first_name )

	m.remove_index( 0 )
	print "after remove_index = %s" % m.display()
	
	a = myarray()
	a.append( 'a' )
	a.append( 'b' )
	a.append( 'c' )

	b = myarray()
	b.append( 'a' )
	b.append( 'c' )

	print "checking subset: should be 1 is %d" % b.is_subset_of( a )
	print "checking len: should be 2 is %d" % b.len()

	print "doing test c"
	c = myarray()
	c.set( 5, 'hello there' )
	print "seeing array c = %s" % c.display()
	print "checking get: should be hello there is (%s)" % \
		c.get( 5 )

	d = myarray()
	print "checking get if not set: should be blank is (%s)" % \
		d.get( 5 )
	print "testing set_data()"
	d = myarray()
	d.set_data( 5, 5 )
	r = d.get( 5 )
	print "with the unwrapper should be (5,set) is: (%d,%s)" % \
		( d.get( 5 ).get(), d.get( 5 ).get_state_string() )

def do_test2():
	a = myarray()
	a.append( "one" )
	a.append( "two" )
	a.append( "three" )
	t = a.get_tuple()
	print "tuple = %s" % `t`

def do_test3():
	a = myarray()
	a.string2array( "zero,one,two,three", ',' )
	print "len should be 4 is %d" % a.len()
	
# Main
# ----
if ( __name__ == "__main__" ):
	# do_test1()
	# do_test2()
	do_test3()
