#!/usr/bin/python
#---------------------------
# test_readline.py
#
# Tim Riley
# 1/2010
#-----------------------------------------------------------

import sys, string

# Main
# ----
if ( __name__ == "__main__" ):
	while 1:
		try:
			line = raw_input()
		except EOFError:
			break
		print "%s" % line
