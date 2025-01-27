#!/usr/bin/python
#---------------------------
# row2.py
#
# The class record defines operations to store groups of items together
# under the same identifier.
#
# Tim Riley
# 1/99
#----------------------------------------------------------------------

import myarray, sys, dictionary, mydata

class row2:
	def __init__( self, column_name ):
		self.column_name = column_name
		self.data = dictionary.dictionary()

	def __len__( self ):
		return self.data.len()

	def __getitem__( self, i ):
		return self.data.__getitem__( i )

	def set( self, identifier, item ):
		d = mydata.mydata()
		d.set( item )
		self.data.set( identifier, d )

	def get( self, identifier ):
		if ( self.data.key_exists( identifier ) ):
			d = self.data.get( identifier )
			return d.get()
		else:
			return 'null'

	def display( self ):
		return self.data.display()

	def len( self ):
		return self.__len__()

	def len_identifier( self, identifier ):
		data = self.get( identifier )
		if ( data == 'null' ):
			s = "ERROR: row2.len_identifier cannot find: (%s)\n" %\
			    identifier
			sys.stderr.write( s )
			return 0
		else:
			return len( data )
# Main
# ----
if ( __name__ == "__main__" ):
	person = row2()
	location = row2()

	location.set( 'address', '8446 NW 190 Terrace' )
	location.set( 'city', 'miami' )
	location.set( 'state', 'FL' )
	location.set( 'zip', '33015' )
	person.set( 'first_name', 'tim' )
	person.set( 'last_name', 'riley' )
	person.set( 'location', location )
	print location.display()
	print person.display()

	print "should be tim is (%s)" % ( person.get( 'first_name' ) )
	print "should be riley is (%s)" % ( person.get( 'last_name' ) )
	print "should be FL is (%s)" % ( location.get( 'state' ) )

	another_location = person.get( 'location' )
	print "should be 33015 is (%s)" % ( location.get( 'zip' ) )

	print "testing len(zip)"
	print "should be 5 is %d" % location.len_identifier( 'zip' )
	print "should bring error because 'zzip' is not found"
	print location.len_identifier( 'zzip' )
	
