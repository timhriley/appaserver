#!/usr/bin/python
#---------------------------
# name_arg_test.py
#
# Tim Riley
# 1/99
#-----------------------------------------------------------

import sys, name_arg

# Main
# ----
if ( __name__ == "__main__" ):

	n = name_arg.name_arg( sys.argv[ 0 ] )
	n.set_variable( "operation" )
	n.set_from_argv( sys.argv )

	volume = n.get_value( sys.argv, "volume" )
	print "Got volume = (%s)" % volume

	color = n.get_value( sys.argv, "color" )
	print "Got color = (%s)" % color

	operation = n.get_value( sys.argv, "operation" )
	print "Got operation = (%s)" % operation

	sys.exit( 0 )
	
