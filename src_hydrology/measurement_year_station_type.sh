:
# $APPASERVER_HOME/src_hydrology/measurement_year_station_type.sh
# ---------------------------------------------------------------
# Freely available software. See appaserver.org
# ---------------------------------------------------------------

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
	echo "Usage: $0 year [html_yn]" 1>&2
	exit 1
fi

year=$1

if [ "$#" -eq 2 ]
then
	html_yn=$2
fi

if [ "$html_yn" = "y" ]
then
	content_type_cgi.sh

	wrapper="html_table.e '' station_type ^"
else
	wrapper="cat"
fi

measurement_year_list.sh $year			|
piece.e '^' 1					|
sort -uf					|
$wrapper					|
cat

exit 0

