#!/usr/bin/python
#-----------------------------------
# library.py
#
# Tim Riley
# 1/99
#-----------------------------------------------------------

import sys, os, dictionary, row2, timlib

class library:

	# Instantiation section
	# ---------------------
	def __init__( self ):
		self.catalog = dictionary.dictionary()

	# Set section
	# -----------
	def set( self, key, identifier, data ):
		if ( self.catalog.key_exists( key ) == 0 ):
			r = row2.row2( key )
		else:
			r = self.catalog.get( key )

		r.set( identifier, data )
		self.catalog.set( key, r )

	# Get section
	# -----------
	def __len__( self ):
		return self.catalog.len()

	def __getitem__( self, i ):
		return self.catalog.__getitem__( i )

	def key_exists( self, key ):
		return self.catalog.key_exists( key )

	def get( self, key, identifier ):
		if ( not self.key_exists( key ) ):
			return ''
		else:
        		r = self.catalog.get( key )
			return r.get( identifier )

	def get_keys( self ):
		return self.catalog.get_keys()

# Test drive
# ----------
if ( __name__ == "__main__" ):

	l = library()

	l.set( "tim", "first_name", "tim" )
	l.set( "tim", "last_name", "riley" )

	a = l.get( "tim", "first_name" )
	print "should be (tim) is (%s)" % a

	a = l.get( "tim", "last_name" )
	print "should be (riley) is (%s)" % a

	sys.exit( 0 )
	
