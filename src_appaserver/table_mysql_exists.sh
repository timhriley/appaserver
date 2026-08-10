#!/bin/bash
# ------------------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/table_mysql_exists.sh
# ------------------------------------------------------------------
# Returns 0 (shell success) if table exists in Mysql.
# ------------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ------------------------------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: `basename.e $0 n` folder" 1>&2
	exit 1
fi

folder=$1

database_table=`database_table $folder`

result=$(echo "describe ${database_table}"	| \
	 sql 2>/dev/null)

if [ "$result" = "" ]
then
	exit 1
fi

exit 0
