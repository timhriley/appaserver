#!/usr/bin/python
#-----------------------------
# Oracle application routines.
#
# -- Tim Riley
# 1/99
# ----------------------------

import sys, os, string

def get_data_from_table( owner, table, index ):
	sys_str = "select_table.py %s %s | extract_count.py %d" %\
		  ( owner, table, index )
	p = os.popen( sys_str, "r" )
	data = string.strip( p.readline() )

	if ( data == '' ):
		print \
		"ERROR in get_data_from_table: cannot find row %d in %s.\n" % \
		( index, table )
		sys.exit( 1 )

	return data

def key_exists_error_or_warning( results, table_name, column_name, override ):
	if ( results >= "1" ):
		if ( override == 0 ):
			print 						\
			"ERROR: key exists in %s for (%s)\n" % 		\
			( table_name, column_name )
			sys.exit( 1 )
		else:
			print 						\
			"WARNING: key exists in %s for (%s)\n" % 	\
			( table_name, column_name )

def create_delete_sys_str( 	table_name,				\
				primary_key_field,			\
				primary_key_data,			\
				oracle_owner ):

	sys_str = "	echo \"delete from %s				 \
			      where %s = '%s';\"			|\
			sqlplus `get_uid_pwd.e %s`			|\
			grep deleted" % ( table_name, 			 \
					  primary_key_field,		 \
					  primary_key_data,		 \
					  oracle_owner )

	return sys_str

def key_exists( table, column, value, db_key ):
	sys_str = "echo "						+\
		  "\"select count(*)"					+\
		  " from " + table 					+\
		  " where " + column + " = '" + value + "';\" 	      |"+\
		  "select.sh " + db_key

	p = os.popen( sys_str, "r" )
	results = p.readline()
	p.close()
	return string.strip( results )

if __name__ == "__main__":
	results = key_exists( "kingdom", "kingdom_name", "ANIMAL", "riley" )
	print "Results = (%s)" % results
