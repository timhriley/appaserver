#!/bin/bash
# -------------------------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/table_column_exists.sh
# -------------------------------------------------------------------------
# Returns 0 (shell success) if both table and column exist in TABLE_COLUMN.
# -------------------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# -------------------------------------------------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: `basename.e $0 n` table column" 1>&2
	exit 1
fi

table=$1
column=$2

where="table_name = '$table' and column_name = '$column'"

result=$(select.sh 'count(1)' table_column "$where")

if [ "$result" -eq 0 ]
then
	exit 1
fi

exit 0
