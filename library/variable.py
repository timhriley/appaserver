#!/usr/bin/python
#---------------------------
# variable.py
#
# Tim Riley
# 1/99
#---------------------------

import sys, queue, dictionary, timlib

class variable:
	# Instantiation section
	# ---------------------
	def __init__( self ):
		self.value = ''
		self.default_value = 'null'
		self.variable = {}
		self.possible_value = queue.queue()

	# Set section
	# -----------
	def set_possible_value( self, possible_value ):
		self.possible_value.append( possible_value )

	def set_default_value( self, default_value ):
		self.default_value = default_value

	def set_value( self, value ):
		if ( self.possible_value.len() >= 1 ):
			if ( self.possible_value.string_exists( value ) ):
				self.value = value
				return 1
			else:
				return 0
		else:
			self.value = value
			return 1


	# Get section
	# -----------
	def __len__( self ):
		return 

	def is_value_ok( self, value ):
		if ( value == self.default_value ):
			return 1
		else:
			return self.possible_value.string_exists( value )

	def get_value( self ):
		if ( self.value == '' and self.default_value != 'null' ):
			return self.default_value
		else:
			return self.value

# Main
# ----
if ( __name__ == "__main__" ):
	color = variable()
	color.set_possible_value( "blue" )
	color.set_possible_value( "green" )
	color.set_default_value( "green" )

	if ( color.set_value( "red" ) ):
		print "BAD set red succeeded."
	else:
		print "GOOD set red failed."

	results = color.is_value_ok( "green" )
	if ( results == 1 ):
		print "Good green is OK"
	else:
		print "BAD green is broken"

	print "should be green is (%s)" % color.get_value()
	color.set_value( "blue" )
	print "should be blue is (%s)" % color.get_value()

	results = color.is_value_ok( "purple" )
	if ( results == 0 ):
		print "Good purple is not OK"
	else:
		print "BAD purple is broken"

	sys.exit( 0 )
	
