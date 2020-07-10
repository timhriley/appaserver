:
# $APPASERVER_HOME/src_hydrology/measurement_year_station.sh
# ----------------------------------------------------------
# Freely available software. See appaserver.org
# ----------------------------------------------------------

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 year [station_type]" 1>&2
	exit 1
fi

year=$1

if [ "$#" -eq 2 -a "$2" != "" ]
then
	station_type_grep="grep -i \\^$2$"
else
	station_type_grep="cat"
fi

measurement_year_list.sh $year			|
piece.e '^' 0,1					|
$station_type_grep				|
piece.e '^' 0					|
sort -uf					|
cat

exit 0

