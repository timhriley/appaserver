:
# $APPASERVER_HOME/src_hydrology/measurement_year_list.sh
# -------------------------------------------------------
# Freely available software. See appaserver.org
# -------------------------------------------------------

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

s=station
m=measurement

select="$m.station,$s.station_type,$m.datatype"

from="$m, $s"

join="$m.station = $s.station"

filter="$s.station_type is not null"

#current_month=`date.sh 0 | piece.e '-' 1`

#if [ "$current_month" = "01" ]
#then
#	first_where="1 = 1"
#else
#	first_where="$m.measurement_date = '$year-01-01'"
#fi

#current_where="$m.measurement_date = '$year-${current_month}-01'"

#where="$join and $filter and ($first_where or $current_where)"

first_where="$m.measurement_date = '$year-01-01'"
where="$join and $filter and $first_where"

echo "select $select from $from where $where;"		|
sql.e							|
sort -u							|
cat

exit 0

