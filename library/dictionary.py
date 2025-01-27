#!/usr/bin/python
#-----------------------------------
# dictionary.py
#
# Tim Riley
# 1/99
#-----------------------------------------------------------

import sys, os, myarray, mydata, timlib

class dictionary:

	# Instantiation section
	# ---------------------
	def __init__( self ):
		self.dictionary = {}

	# Set section
	# -----------
	# Example: array_string = ["year=1997", "report=Year End"]
	# Useful on sys.argv
	# --------------------------------------------------------
	def set_from_array_string( self, array_string, delimiter ):
		for variable_data_string in array_string:
			key = \
			timlib.piece( delimiter, variable_data_string, 0 )
			data = \
			timlib.piece( delimiter, variable_data_string, 1 )

			if ( len( key ) and len( data ) ):
				self.set( key, data )

	def set_dictionary( self, d ): self.dictionary = d

	def set( self, key, data ): self.dictionary[ key ] = data

	# Operation section
	# -----------------
	def delete( self, key ): del self.dictionary[ key ]

	# Get Section
	# -----------
	def get_sed_string( self, variable_marker_string ):
		first_time = 1
		sed_string = ""
		key_string_array = self.get_key_string_array()
		for key_string in key_string_array:
			data_string = self.get( key_string )
			if ( first_time ):
				sed_string = \
				sed_string + "sed 's/\\%s%s/%s/g'" % \
					( variable_marker_string,
					  key_string,
					  data_string )
				first_time = 0
			else:
				sed_string = \
				sed_string + " | sed 's/\\%s%s/%s/g'" % \
					( variable_marker_string,
					  key_string,
					  data_string )
		return sed_string

	def get_key_string( self, delimiter_string ):
		string_array = self.get_key_string_array()
		key_string = \
			timlib.array2string( string_array, delimiter_string )
		return key_string

	def get_value_string( self, delimiter_string ):
		string_array = self.get_value_string_array()
		value_string = \
			timlib.array2string( string_array, delimiter_string )
		return value_string

	def get_pair_string_array( self, prefix_string ):
		a = myarray.myarray()

		key_string_array = self.get_key_string_array()
		for key_string in key_string_array:
			data = self.get( key_string )
			s = "%s%s='%s'" % \
				( prefix_string, key_string, data )
			a.append( s )

		return a

	def get_pair_string( self, prefix_string ):
		return_string = ''
		first_time = 1

		key_string_array = self.get_key_string_array()
		for key_string in key_string_array:
			data = self.get( key_string )
			s = "%s%s='%s'" % \
				( prefix_string, key_string, data )

			if ( first_time ):
				first_time = 0
			else:
				return_string = return_string + ' '

			return_string = return_string + s
		return return_string

	def __len__( self ):
		key_string_array = self.get_key_string_array()
		return key_string_array.len()

	def __getitem__( self, i ):
		key_string_array = self.get_key_string_array()
		return key_string_array[ i ]

	def display( self ):
		first_time = 1
		s = "["
		key_string_array = self.get_key_string_array()
		for key_string in key_string_array:
			data = self.get( key_string )

			if ( first_time ):
				s = s + "%s=%s" % ( key_string, data )
				first_time = 0
			else:
				s = s + ",%s=%s" % ( key_string, data )
		s = s + "]"
		return s

	def len( self ):
		return __len__()

	def len_key( self, key ):
		data = self.get( key )
		if ( data == '' ):
			return 0
		else:
			return data.len()

	def get( self, key ):
		return self.dictionary.get( key, '' )

	def key_exists( self, key ):
		if key in self.dictionary.keys():
			return 1
		else:
			return 0

	def get_value_array( self ):
		value_array = myarray.myarray()
		for v in self.dictionary.values():
			value_array.append( v )
		return value_array

	def get_keys( self ): return self.get_key_string_array()
	def get_key_array( self ): return self.get_key_string_array()
	def get_value_string_array( self ): return self.get_value_array()

	def get_key_string_array( self ):
		key_string_array = myarray.myarray()
		for k in self.dictionary.keys():
			key_string_array.append( k )
		return key_string_array

# Test drive
# ----------
if ( __name__ == "__main__" ):

	d = dictionary()

	d.set( "a", 'test' )
	a = d.get( "a" )
	print "should be (test) is (%s)" % a

	d.set( "b", [ 'a', 'b', 'c' ] )
	b = d.get( "b" )
	print "should be (b) is (%s)" % b[ 1 ]
	
	keys = d.get_key_string_array()
	print "key[0] = %s" % `keys[0]`
	print "key[1] = %s" % `keys[1]`

	print "keys = %s" % keys.display()

	print "dictionary = %s" % d.display()
	for i in d: print "for loop returned: %s" % `i`

	sys.exit( 0 )
	
