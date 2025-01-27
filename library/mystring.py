#!/usr/bin/python
#---------------------------
# mystring.py
#
# The class string defines operations to store strings.
#
# Tim Riley
# 1/99
#-----------------------------------------------------------

import string

class timstring:
	def __init__( self, *data ):
		self.data = data

	def __getitem__( self, i ):
		return self.data[ i ]

	def __len__( self ):
		return self.len()

	def zap( self ):
		self.data = ''

	def display( self ):
		return self.data

	def len( self ):
		return len( self.data )

	def set( self, data ):
		self.data = data

	def get( self ):
		return self.data

	def find( self, substring ):
		return string.find( self.data, substring )

# Main
# ----
if ( __name__ == "__main__" ):
	s = timstring()
	s.set( 'hello world' )
	print "should be hello world is %s" % s.get()
	print "find should return 6 is %d" % s.find( 'world' )
