#!/bin/sh
# -------------------------------------------------------------
# $APPASERVER_HOME/src_hydrology/datatype_name_list
# -------------------------------------------------------------
#
# No warranty and freely available software: see Appaserver.org
# -------------------------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 application [station] [state]" 1>&2
	exit 1
fi

application=$1
filterstation=0

if [ "$#" -ge 2 -a "$2" != "station" ]
then
	station=$2
	filterstation=1

	if [ "$#" -eq 3 ]
	then
		state=$3

		if [ "$state" = "insert" ]
		then
			filterstation=0
		fi
	fi
fi

if [ "$filterstation" -eq 1 ]
then
#	in_clause=`echo $station | sed 's/,/\n/g' | in_clause.e`
	in_clause=`echo $station | in_clause.e`

	echo "select datatype				\
      	from station_datatype 				\
	where station ${in_clause}			\
      	order by datatype;" 				|
	sql.e
else
	echo "select datatype				\
      	from datatype		 			\
      	order by datatype;" 				|
	sql.e
fi

