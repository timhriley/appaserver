#!/usr/bin/python
#---------------------------
# stack.py
#
# The class stack defines operations to store lists of items in a stack.
#
# Tim Riley
# 1/99
#-----------------------------------------------------------------------

import regsub, sys, os, string, oracle_application, timlib, myarray

class stack:
	def __init__( self ):
		self.stack = myarray.myarray()

	def __getitem__( self, i ):
		return self.stack[ i ]

	def __len__( self ):
		return self.len()

	def display( self ):
		return self.stack.display()

	def len( self ):
		return self.stack.len()

	def is_empty( self ):
		if ( self.stack.len() == 0 ):
			return 1
		else:
			return 0

	def push( self, item ):
		self.stack.prepend( item )

	def find_data( self, data ):
		return self.stack.find_data( data )

	def pop( self ):
		self.stack.go_head()
		data = self.stack.get_current_data()
		self.stack.delete_current()
		return data

# Main
# ----
if ( __name__ == "__main__" ):
	q = stack()
	q.push( '1' )
	q.push( '2' )
	q.push( '3' )
	print "stack = %s" % q.display()
	results = q.find_data( '2' )
	print "2 exists = %d" % results

	number = q.pop()
	print "should be 3 is %s" % number

	number = q.pop()
	print "should be 2 is %s" % number

	number = q.pop()
	print "should be 1 is %s" % number

