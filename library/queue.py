#!/usr/bin/python
#---------------------------
# queue.py
#
# The class queue defines operations to store lists of items in a queue.
#
# Tim Riley
# 1/99
#-----------------------------------------------------------------------

import myarray

class queue:
	def __init__( self ):
		self.queue = myarray.myarray()

	def __len__( self ):
		return self.queue.len()

	def __getitem__( self, i ):
		return self.queue[ i ]

	def display( self ):
		return self.queue.display()

	def len( self ):
		return self.queue.len()

	def is_empty( self ):
		if ( self.queue.len() == 0 ):
			return 1
		else:
			return 0

	def enqueue( self, data ):
		self.queue.append( data )

	def append( self, data ):
		self.enqueue( data )

	def find_data( self, data ):
		return self.queue.find_data( data )

	def string_exists( self, data ):
		return self.queue.string_exists( data )

	def dequeue( self ):
		self.queue.go_head()
		data = self.queue.get_current_data()
		self.queue.delete_current()
		return data

# Main
# ----
if ( __name__ == "__main__" ):
	q = queue()
	q.enqueue( '1' )
	q.enqueue( '2' )
	q.enqueue( '3' )
	print "queue = %s" % q.display()
	results = q.find_data( '2' )
	print "2 exists = %d" % results

	number = q.dequeue()
	print "should be 1 is %s" % number

	number = q.dequeue()
	print "should be 2 is %s" % number

	number = q.dequeue()
	print "should be 3 is %s" % number

