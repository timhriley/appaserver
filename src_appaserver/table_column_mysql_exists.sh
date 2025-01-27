#!/bin/bash
# ------------------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/table_column_mysql_exists.sh
# ------------------------------------------------------------------
# Returns 0 (shell success) if both table and column exist in Mysql.
# ------------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ------------------------------------------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: `basename.e $0 n` table column" 1>&2
	exit 1
fi

table=$1
column=$2

database_table=`database_table $table`

result=$(echo "describe ${database_table}"	| \
	 sql 2>/dev/null			| \
	 grep "^$column\^")

if [ "$result" = "" ]
then
	exit 1
fi

exit 0
