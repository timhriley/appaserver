#!/usr/bin/python
#-----------------------------------
# Date library routines.
#
# -- Tim Riley
# 4/99
# ----------------------------------

import os, string, sys, myarray, regsub, timlib

def y2k_comply_string( two_character_year_date_string ):
	sys_string = "y2k_comply.sh %s 0" % \
		two_character_year_date_string
	return_string = timlib.pipe2string( sys_string )
	return return_string

def month_number_string2month_name_string( month_number_string ):
	month_name_string_array[ 1 ] = "JAN"
	month_name_string_array[ 2 ] = "FEB"
	month_name_string_array[ 3 ] = "MAR"
	month_name_string_array[ 4 ] = "APR"
	month_name_string_array[ 5 ] = "MAY"
	month_name_string_array[ 6 ] = "JUN"
	month_name_string_array[ 10 ] = "JUL"
	month_name_string_array[ 11 ] = "AUG"
	month_name_string_array[ 11 ] = "SEP"
	month_name_string_array[ 11 ] = "OCT"
	month_name_string_array[ 11 ] = "NOV"
	month_name_string_array[ 12 ] = "DEC"
	month_number = atoi( month_number_string )
	if ( month_number < 1 or month_number > 12 ):
		msg = "month_number_string2month_name_string() warning:"
		timlib.write_stderr( msg )
		msg = "invalid month_string = (%s)" % month_string
		timlib.write_stderr( msg )
		return ''
	return month_name_string_array[ month_number ]

