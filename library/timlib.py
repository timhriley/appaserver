#!/usr/bin/python
#---------------------------
# Generic library routines.
#
# -- Tim Riley
# 1/99
# --------------------------

import os, string, sys, myarray, regsub, regex

def pipe2string_array( pipe_string ):
	return_array = myarray.myarray()
	return return_array.pipe2string_array( pipe_string )

def y2k_comply_string( two_character_year_date_string ):
	sys_string = "y2k_comply.sh %s 0" % \
		two_character_year_date_string
	return_string = pipe2string( sys_string )
	return return_string

def popen( pipe_string, mode_string ):
	return os.popen( pipe_string, mode_string )

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
		write_stderr( msg )
		msg = "invalid month_string = (%s)" % month_string
		write_stderr( msg )
		return ''
	return month_name_string_array[ month_number ]

def get_where_clause_string( 	column_string_array,
				data_string_array ):
	data_string_array_len = data_string_array.len()
	i = 0

	where_clause_string = 'where 1 = 1'

	for column_string in column_string_array:
		if ( i == data_string_array_len ):
			msg = \
"ERROR get_where_clause_string(): not enough data: columns = (%s) data = (%s)"%\
			( column_string_array.display(),
			  data_string_array.display() )
			write_stderr( msg )
			exit( 1 )

		append_string = " and %s = %s" % \
			( column_string,
			  data_string_array[ i ] )
		where_clause_string = where_clause_string + append_string
		i = i + 1
	return where_clause_string
# end get_where_clause_string() 

def read_stdin(): return get_stdin_string()

def get_stdin_string():
	return string.rstrip( sys.stdin.readline() )

def get_from_file_string( filename_string ):
	f = open( filename_string, "r" )
	for line in f.readlines():
		f.close()
		return string.rstrip( line )
	f.close()
	return ''
# end get_from_file_string()

def pipe2string( pipe_string ): return get_from_pipe_string( pipe_string )

def get_from_pipe_string( pipe_string ):
	p = os.popen( pipe_string, "r" )
	for line in p.readlines():
		p.close()
		return string.rstrip( line )
	p.close()
	return ''

def exit( code ): sys.exit( code )
def system( sys_string ): os.system( sys_string )

def trim( s ): return string.strip( s )

def single_quotes_around( s ):
	sys_string = 'single_quotes_around.e %s' % s 
	p = os.popen( sys_string, "r" )
	for line in p.readlines(): return string.rstrip( line )

def trim_quotes( s0 ):
	if ( len( s0 ) == 0 ): return s0

	if ( string.find( s0, '"' ) == -1 and string.find( s0, "'" ) == -1 ):
		return s0

	if ( s0[ 0 ] == "'" or s0[ 0 ] == '"' ):
		s1 = s0[1:]
	else:
		s1 = s0

	strlen_minus_one = len( s1 ) - 1
	if ( s1[ strlen_minus_one ] == "'" or s1[ strlen_minus_one ] == '"' ):
		s2 = s1[0:strlen_minus_one]
	else:
		s2 = s1
	
	return s2

def stderr_write_flush( s ):
	sys.stderr.write( s )
	sys.stderr.flush()

def stdout_write_flush( s ):
	sys.stdout.write( s )
	sys.stdout.flush()

def write_stderr( s ): return print_stderr( s )
def write_stdout( s ): return print_stdout( s )
def stderr_write( s ): return print_stderr( s )
def stdout_write( s ): return print_stdout( s )

def write_stderr_nocr( s ): return stderr_write_flush( s )
def write_stdout_nocr( s ): return stdout_write_flush( s )

def print_stderr( s1 ):
	s2 = s1 + '\n'
	sys.stderr.write( s2 )
	sys.stderr.flush()

def print_stdout( s1 ):
	s2 = s1 + '\n'
	sys.stdout.write( s2 )
	sys.stdout.flush()

def get_from_process_string( process_string ):
	return get_from_pipe_string( process_string )

def printf( using_string, space_delimited_string, debug_string ):
	if ( debug_string == 'yes' ):
		process = "printf '%s' %s" % \
			( using_string, space_delimited_string )
	else:
		process = "printf '%s' %s 2>/dev/null" % \
			( using_string, space_delimited_string )

	if ( debug_string == 'yes' ):
		s = "%s\n" % process
		sys.stderr.write( s )

	return get_from_pipe_string( process )

def list2array( delimiter, list_string ):
	a = myarray.myarray()
	r = regsub.split( string.strip( list_string ), delimiter )
	for x in r: a.append( x )
	return a

def piece( delimiter, source, index_zero_based ):
	stripped_string = string.strip( source )
	record = regsub.split( stripped_string, delimiter )
	if index_zero_based >= len( record ): return ""
	stripped_string = string.strip( record[ index_zero_based ] )
	return stripped_string

def piece_replace( delimiter, source, replace, index_zero_based ):
	stripped_string = string.strip( source )
	record = regsub.split( stripped_string, delimiter )
	if index_zero_based >= len( record ): return ""
	record[ index_zero_based ] = replace
	record_join_string = get_record_join_string( record, delimiter )
	return record_join_string

def get_record_join_string( record, delimiter ):
	first_time = 1
	return_string = ""
	for item in record:
		if ( first_time ):
			return_string = item
			first_time = 0
		else:
			return_string = return_string + delimiter + item
		msg = "return_string now = (%s)" % return_string
	return return_string

def array2string( l, delimiter ):
	return list2string( l, delimiter )

def set2string( l, delimiter ):
	return list2string( l, delimiter )

def list2string( l, delimiter ):
	s = ''
	first_time = 1
	l.go_head()
	while ( l.at_end() == 0 ):
		a = l.get_current_data()
		a = string.strip( a )
		if ( first_time ):
			s = s + a
			first_time = 0
		else:
			s = s + delimiter
			s = s + a
		l.next_item()
	return s
		
def variable2words( variable ):
	line = ''
	sys_str = "variable2words.e %s" % variable
	p = os.popen( sys_str, "r" )
	for line in p.readlines(): return line

def load_data_file( filename ):
	data = []
	f = open( filename, "r" )
	for line in f.readlines():
		data.append( line )
	return data

def load_data_process( process ):
	data = []
	p = os.popen( process, "r" )
	for line in p.readlines():
		data.append( line )
	return data

# atoi()
# ------
def atoi( str ):
	first_time = 1
	if len( str ) == 0: return 0
	for c in str:
		if c in string.letters: return 0
		if c in [ '.' ] : return 0
		if ( first_time ):
			first_time = 0
		else:
			if c in [ '-' ] : return 0
	return string.atoi( str )
	
# make_key()
# -----------
# Sample input: s = "fla.  bay"
# Output:	    "FLA BAY"
# -----------------------------
def make_key( s ):
	if ( s == '' ): return ''
	sys_str = "echo " + s + " | make_key.e"
	return_string = get_from_pipe_string( sys_str )
	return return_string

def make_pleasant( s ):
	if ( s == '' ): return ''
	sys_str = "echo " + s + " | make_pleasant.e"
	return_string = get_from_pipe_string( sys_str )
	return return_string

def test1():
	results = make_key( 'carp. and  ..minnow,' )
	print "Results = (%s)" % results
	results = printf( "%3d %30s", "2 'Tim Riley'", 'no' )
	print "Checking printf = (%s)" % results
	print "should be without quotes = (%s)" % \
		trim_quotes( "'this is a quoted string'" )

# Example: regular_expression_match( 'tim riley', '.*riley$' )
# ------------------------------------------------------------
def regular_expression_match( source_string, expression_string ):
	regx = regex.compile( expression_string )
	return regx.match( source_string )

	
def test2():
	print "testing regular_expression_match"
	full_name = "tim riley"
	looking_for = ".*riley$"
	print "should be >= 0 is %d" % \
		regular_expression_match( full_name, looking_for )

def test3():
	str = "zero|one|two|three"
	replace_string = piece_replace( '|', str, "changed", 1 )
	print "should be (zero|changed|two|three) is (%s)\n" % replace_string

if __name__ == "__main__":
	# test1()
	# test2()
	test3()
