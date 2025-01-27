#!/usr/bin/python
#---------------------------
# array_dictionary.py
#
# The class array_dictionary is inheriting the operations 
# of both array (myarray) and dictionary.
#
# Tim Riley
# 4/99
#----------------------------------------------------------

import sys, os, timlib, myarray, dictionary, string

class array_dictionary:
	# Instantiation section
	# ---------------------
	def __init__( self ):
		self.dictionary = dictionary.dictionary()

	# Set section
	# -----------
	def set_key_from_pipe( 	self, 
				pipe_string ):
		p = timlib.popen( pipe_string, "r" )
		for key_string in p.readlines():
			self.set_key( string.strip( key_string ) )
		p.close()

	def load_pair_from_pipe( 	self, 
					pipe_string ):
		p = timlib.popen( pipe_string, "r" )
		for input_string in p.readlines():
			key_string = 				\
				timlib.piece( '|', 		\
					      input_string,	\
					      0 )
			value_string = 				\
				timlib.piece(	'|', 		\
					input_string,		\
					1 )
			self.append_data( key_string, value_string )
		p.close()

	def set_key( self, key_string ):
		data_array = myarray.myarray()
		self.dictionary.set( key_string, data_array )

	def append_data( self, key_string, data ):
		data_array = self.dictionary.get( key_string )
		if ( data_array == '' ):
			data_array = myarray.myarray()
		data_array.append( data )
		self.dictionary.set( key_string, data_array )

	# Get section
	# -----------
	def get_key_string_array( self ):
		key_string_array = self.dictionary.get_key_string_array()
		return key_string_array

	def get_string_array( self, key_string ):
		return self.get_array( key_string )

	def get( self, key_string ): return self.get_array( key_string )

	def get_array( self, key_string ):
		data_array = self.dictionary.get( key_string )
		return data_array

	def __len__( self ):
		return self.dictionary.__len__()

	def __getitem__( self, i ):
		return self.dictionary.__getitem__( i )

	def display( self ):
		return self.dictionary.display()

	def len( self ):
		return self.dictionary.__len__()

# Main
# ----
if ( __name__ == "__main__" ):
	a = array_dictionary()
	a.append_data( 'first', '1' )
	a.append_data( 'first', '2' )
	a.append_data( 'first', '3' )
	a.append_data( 'second', '4' )
	array = a.get_array( 'first' )
	timlib.write_stdout( "first = %s" % array.display() )
	array = a.get_array( 'second' )
	timlib.write_stdout( "second = %s" % array.display() )
