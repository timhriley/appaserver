#!/usr/bin/python
#---------------------------
# string_dictionary.py
#
# Tim Riley
# 2/99
#---------------------------

import sys, queue, dictionary, myarray

class string_dictionary:

	# Instantiation
	# -------------
	def __init__( self ):
		self.string_dictionary = dictionary.dictionary()

	# Set section
	# -----------
	def set( self, variable_string, value_string ):
		self.string_dictionary.set( variable_string, value_string )

	# Get section
	# -----------
	def get_key_string( self, delimiter_string ):
		return \
		self.string_dictionary.get_key_string( delimiter_string )

	def get_value_string( self, delimiter_string ):
		return \
		self.string_dictionary.get_value_string( delimiter_string )

	def get_key_array( self ):
		return self.string_dictionary.get_key_array()

	def get_value_array( self ):
		return self.string_dictionary.get_value_array()

	def key_exists( self, key ):
		return self.string_dictionary.key_exists( key )

	def get_pair_string_array( self, prefix_string ):
		return \
		self.string_dictionary.get_pair_string_array( prefix_string )

	def display( self ):
		return self.string_dictionary.display()

	def get_pair_string( self, prefix_string ):
		return self.string_dictionary.get_pair_string( prefix_string )

	def get( self, variable_string ):
		return self.string_dictionary.get( variable_string )

	def get_key_string_array( self ):
		return self.string_dictionary.get_key_string_array()

	def get_value_string_array( self ):
		return self.string_dictionary.get_value_string_array()

	def __len__( self ): return self.string_dictionary.len()

	def is_value_ok( self, value ):
		if ( value == self.default_value ): return 1
		return self.possible_value.find_data( value )

	def get_value( self ):
		if ( self.value == '' and self.default_value != 'null' ):
			return self.default_value
		else: return self.value

# Main
# ----
if ( __name__ == "__main__" ):
	variable = string_dictionary()

	variable.set( 'color', 'blue' )
	print "should be blue is (%s)" % variable.get( 'color' )

	print "checking subtract_everything_but()"
	variable.set( 'volume', 'loud' )
	variable.set( 'texture', 'soft' )
	keep = myarray.myarray()
	keep.append( 'texture' )
	variable.subtract_everything_but( keep )

	print "should be texture is %s" % \
		variable.get_key_string_array().display()

	sys.exit( 0 )
	
