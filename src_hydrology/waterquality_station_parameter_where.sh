#!/bin/bash
# ----------------------------------------------------------------------
# $APPASERVER_HOME/src_hydrology/waterquality_station_parameter_where.sh
# ----------------------------------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 station_parameter_units_list" 1>&2
	echo "Note: lists are comma separated." 2>&1
	exit 1
fi

station_parameter_units_list=$1

i=0

echo "("

while [ true ]
do
	record=`echo $station_parameter_units_list | piece.e , $i 2>/dev/null`

	if [ "$record" = "" ]
	then
		break
	fi

	station=`echo $record | piece.e '^' 0`
	parameter=`echo $record | piece.e '^' 1`
	units=`echo $record | piece.e '^' 2`

	if [ $i -ge 1 ]
	then
		echo "or"
	fi

	echo "(station = '$station' and parameter = '$parameter' and units = '$units')"

	i=`expr $i + 1`
done

echo ")"

exit 0
