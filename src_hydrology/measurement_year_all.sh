:
# $APPASERVER_HOME/src_hydrology/measurement_year_all.sh
# ------------------------------------------------------
# Freely available software. See appaserver.org
# ------------------------------------------------------

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

if [ "$#" -eq 1 ]
then
	html_yn=$1
fi

if [ "$html_yn" = "y" ]
then
	content_type_cgi.sh

	wrapper="html_table.e '' year,station_datatype_count :"
else
	wrapper="cat"
fi

year=`date.sh 0 | piece.e '-' 0`

(
while [ true ]
do
	results=`measurement_year_list.sh $year		|\
		 wc -l`

	echo "${year}: $results"

	year=`expr $year - 1`

	if [ "$year" -eq 1890 ]
	then
		break
	fi
done
) | $wrapper

exit 0
