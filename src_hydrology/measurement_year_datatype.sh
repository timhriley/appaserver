:
# $APPASERVER_HOME/src_hydrology/measurement_year_datatype.sh
# -----------------------------------------------------------
# Freely available software. See appaserver.org
# -----------------------------------------------------------

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

if [ "$#" -lt 2 ]
then
	echo "Usage: $0 year staiton [html_yn]" 1>&2
	exit 1
fi

year=$1
station=$2

if [ "$#" -eq 3 ]
then
	html_yn=$3
fi

if [ "$html_yn" = "y" ]
then
	content_type_cgi.sh

	wrapper="html_table.e '' Datatype ^"
else
	wrapper="cat"
fi

measurement_year_list.sh $year			|
grep -i "^$station^"				|
piece.e '^' 2					|
sort -uf					|
$wrapper					|
cat

exit 0

