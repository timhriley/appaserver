#!/bin/bash
# ---------------------------------------------
# $APPASERVER_HOME/utility/csvdump.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ $# -ne 1 ]
then
	echo "Usage: $0 tablename" 1>&2
	exit 1
fi

tablename=$1

output_header()
{
	tablename=$1

	echo "describe $tablename;"				|
	sql.e							|
	sed 's/\^.*//g'						|
	joinlines.e '^'						|
	sed 's/^/"/'						|
	sed 's/$/"/'						|
	sed 's/\^/","/g'					|
	cat
}

output_rows()
{
	tablename=$1

	echo "select * from $tablename;"			|
	sql.e							|
	sed 's/^/"/'						|
	sed 's/$/"/'						|
	sed 's/\^/","/g'					|
	cat
}

output_header $tablename
output_rows $tablename

exit 0
