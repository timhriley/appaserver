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

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 year" 1>&2
	exit 1
fi

year=$1

measurement_year_list.sh $year			|
piece.e '^' 0					|
sort -uf					|
cat

exit 0

