#!/bin/sh
# ---------------------------------------------
# src_hydrology/datatype_name_list
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

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
	station_datatype_table=`get_table_name $application station_datatype`

	in_clause=`echo $station | sed 's/,/\n/g' | in_clause.e`

	echo "select datatype				\
      	from $station_datatype_table 			\
	where station ${in_clause}			\
      	order by datatype;" 				|
	sql.e
else
	datatype_table=`get_table_name $application datatype`

	echo "select datatype				\
      	from $datatype_table	 			\
      	order by datatype;" 				|
	sql.e
fi

