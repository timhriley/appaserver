#!/usr/bin/python
#-----------------------------------
# name_arg.py
#
# This package provides the class NAME_ARG which facilitates
# named command line arguments.
#
# Tim Riley
# 1/99
#-----------------------------------------------------------

import string, library, queue, os, sys, myarray
import regsub, dictionary, timlib, string_dictionary

class name_arg:
	# Instantiation section
	# ---------------------
	def __init__( self, usage_filename ):
		self.usage_filename = usage_filename
		self.comments = queue.queue()
		self.variable_library = library.library()

		self.variable_string_dictionary = \
			string_dictionary.string_dictionary()

	# Set section
	# -----------
	def set_comment( self, comment ):
        	self.comments.enqueue( comment )

	def set_variable( self, variable_name ):
		self.variable_library.set( variable_name,
					   'variable_name',
					   variable_name )
		self.variable_library.set( variable_name,
					   'value',
					   'null' )
		self.variable_library.set( variable_name,
					   'default_value',
					   'null' )
		self.variable_library.set( variable_name,
					   'default_value_set',
					   'no' )
		self.variable_library.set( variable_name,
					   'possible_value',
					   queue.queue() )

	def set_possible_value( self, variable_name, value ):
		possible_value = self.variable_library.get( variable_name,
							    'possible_value' )
		if ( possible_value == 'null' ):
			s = "set_possible_value(%s) cannot find variable" %\
			    variable_name
			timlib.write_stderr( s )
			sys.exit( 1 )

		possible_value.enqueue( value )
		self.variable_library.set( variable_name, 
					   'possible_value',
					   possible_value )
		return 1

	def set_value( self, variable_name, value ):
		if ( self.value_is_ok( variable_name, value ) == 0 ):
			return 0
		self.variable_library.set( variable_name,
					   'value',
					   value )
		return 1

	def set_default_value( self, variable_name, default_value ):
		self.variable_library.set( variable_name,
					   'default_value',
					   default_value )

		self.variable_library.set( variable_name,
					   'default_value_set',
					   'yes' )
		return 1

	# Operations
	# ----------
	def exit_usage( self, from_here ):
		# print "from_here = %d" % from_here
		s = "Usage: %s\n" % self.usage_filename
		sys.stderr.write( s )
		keys = self.variable_library.get_keys()
		for k in keys:
			s = "\t%s=" % k
			sys.stderr.write ( s )

			possible_value = self.variable_library.get( 
							    k,
							    'possible_value' )
			if ( possible_value == ""
			or   possible_value.is_empty() ):
				sys.stderr.write ( '(?)' )
			else:
				first_time = 1
				for a in possible_value:
					if ( first_time == 1 ):
						first_time = 0
						s = "(%s)" % a
					else:
						s = " (%s)" % a
					sys.stderr.write ( s )
	
			default_value_set = \
				self.variable_library.get( k, 
							   'default_value_set' )
			if ( default_value_set == 'no' ):
				s = " [required]\n"
			else:
				default_value = \
					self.variable_library.get( 
							k, 
							'default_value' )
				if ( default_value == '' ):
					s = " [optional]\n"
				else:
					s = " (default %s) [optional]\n" %    \
				    	    default_value
			sys.stderr.write( s )

		sys.stderr.write ( "Comments:\n" )
		for a in self.comments:
			s = "\t%s\n" % a
			sys.stderr.write( s )

		sys.stderr.write( "\n" )
		sys.exit( 1 )
			
	# Get section
	# -----------
	def __len__( self ):
		return self.variable_library.len()

	def value_is_ok( self, variable_name, value ):
		default_value = self.variable_library.get( variable_name,
							   'default_value' )
		if ( value == default_value ): return 1

		possible_value = self.variable_library.get( variable_name,
							    'possible_value' )

		# If nothing in the array, then value is OK
		# -----------------------------------------
		num_possible_value = possible_value.len()
		if ( num_possible_value == 0 ): return 1

		if ( possible_value.find_data( value ) > -1 ):
			return 1
		else:
			return 0

	def get_variable_string_dictionary( self ): 
		return self.variable_string_dictionary

	def get( self, variable_name ):
		value = self.variable_library.get( variable_name, 'value' )
		if ( value != '' ): return value

		default_value_set = self.variable_library.get( variable_name,
						       'default_value_set' )
		if ( default_value_set == 'yes' ):
			return self.variable_library.get( variable_name,
							  'default_value' )
		else:
			self.exit_usage( 1 )

	def fetch( self, variable_name ):
		return self.get( variable_name )

	def get_value( self, variable_name ):
		return self.get( variable_name )

	def get_full_variable_name( self, partial_name ):
		keys = self.variable_library.get_keys()

		# First do exact matches
		# ----------------------
		for full_name in keys:
			if ( full_name == partial_name ):
				return full_name

		# Then do partial matches
		# -----------------------
		for full_name in keys:
			if ( string.find( full_name, partial_name ) == 0 ):
				return full_name

		# Return no-go
		# ------------
		return ''

	def set_variable_from_argv( self, argv ):
		for a in argv:
			if ( a == '=' ): self.exit_usage( 5 )

			if ( string.find( a, "=" ) == -1 ): continue

			variable = timlib.piece( '=', a, 0 )

			# Sample: set=first_name=tim
			# --------------------------
			if ( variable == 'set' ):
				self.variable_string_dictionary.set(
						timlib.piece( '=', a, 1 ),
						timlib.trim_quotes(
						timlib.piece( '=', a, 2 ) ) )

	def set_from_argv( self, argv ):
		for a in argv:
			if ( a == '=' ): self.exit_usage( 2 )

			if ( string.find( a, "=" ) == -1 ): continue

			variable = timlib.piece( '=', a, 0 )

			# Ignore set=first_name=tim
			# -------------------------
			if ( variable == 'set' ): continue

			# Allow for partial entries on the command line
			# ---------------------------------------------
			full_name = self.get_full_variable_name( variable )
			if ( full_name == '' ):
				self.exit_usage( 4 )

			value = timlib.piece( '=', a, 1 )
			results = self.set_value( full_name, value )

			if ( results == 0 ):
				s = "ERROR: %s is not valid for %s\n" % \
					( value, full_name )
				sys.stderr.write( s )
				self.exit_usage( 3 )

def test_1():
	n.set_comment( "Here is comment line 1" )
	n.set_comment( "Here is comment line 2" )
	n.set_comment( "Here is comment line 3" )

	n.set_variable( "color" )
	n.set_possible_value( "color", "blue" )
	n.set_possible_value( "color", "green" )
	n.set_default_value( "color", "green" )

	n.set_variable( "volume" )
	n.set_possible_value( "volume", "soft" )
	n.set_possible_value( "volume", "medium" )
	n.set_possible_value( "volume", "loud" )

	n.set_variable( "description" )
	n.set_default_value( "description", "rock and roll" )

	n.set_variable( "comments" )

	# Parse command line arguments
	# ----------------------------
	n.set_from_argv( sys.argv )

	print "getting volume"
	volume = n.get( "volume" )
	print "Got volume = (%s)" % volume

	print "getting description"
	description = n.get( "description" )
	print "Got description = (%s)" % description

	print "getting color"
	color = n.get( "color" )
	print "Got color = (%s)" % color

	print "getting comments"
	comments = n.get( "comments" )
	print "Got comments = (%s)" % comments

	print "checking variable dictionary"
	a = n.get_variable_string_dictionary()
	print "Got dictionary keys = %s" % \
		a.get_key_string_array().display()
	print "Got dictionary values = %s" % \
		a.get_value_string_array().display()
	
def test_2():
	n.set_variable( "volume" )
	n.set_possible_value( "volume", "soft" )
	n.set_possible_value( "volume", "medium" )
	n.set_possible_value( "volume", "loud" )
	n.set_default_value( "volume", "loud" )

	# Parse command line arguments
	# ----------------------------
	n.set_from_argv( sys.argv )

	print "getting volume"
	volume = n.get( "volume" )
	print "Got volume = (%s)" % volume

# Main
# ----
if ( __name__ == "__main__" ):
	n = name_arg( sys.argv[ 0 ] )

	# test_1()
	test_2()

