#!/usr/bin/python
#---------------------------
# environ.py
#
# The class array defines operations to access your environment.
#
# Tim Riley
# 1/99
#---------------------------------------------------------------

import sys, os, string, regsub, dictionary

class environ:
	# Instantiation section
	# ---------------------
	def __init__( self ):
		self.environ_dictionary = dictionary.dictionary()
		self.environ_dictionary.set_dictionary( os.environ )

	# Set section
	# -----------

	# Operations
	# ----------

	# Get section
	# -----------
	def get( self, s ):
		return self.environ_dictionary.get( s )

# Main
# ----
if ( __name__ == "__main__" ):
	e = environ()
	print e.get( "HOME" )
	print "Testing not found"
	print e.get( "NOT_FOUND" )
