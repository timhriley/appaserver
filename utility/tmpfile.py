#!/usr/bin/python3
# ---------------------------------------------------------------
# $APPASERVER_HOME/utility/tmpfile.py
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

import sys, os, posix

class tmpfile:

	def __init__( self, key_string ):
		self.key_string = key_string
		self.tmpfile_template_string = '/tmp/%s_%d.dat'

	def get_string( self ): return self.display()

	def display( self ):
		return self.get_tmpfile_string()

	def get_tmpfile_string( self ):
		return self.tmpfile_template_string % \
			( self.key_string, posix.getpid() )

	def remove( self ):
		s = "rm %s" % self.display()
		os.system( s )

	def zap( self ):
		s = "> %s" % self.display()
		os.system( s )

# Driver
# ------
if ( __name__ == "__main__" ):

	t = tmpfile( 'tmpfile' )
	print ( "%s" % t.display() )

	sys.exit( 0 )
