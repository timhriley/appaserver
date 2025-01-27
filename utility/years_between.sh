:
# $APPASERVER_HOME/utility/years_between.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

minutes_per_year=525600

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 start_yyyy_mm_dd end_yyyy_mm_dd" 1>&2
	exit 1
fi

start_date=$1
end_date=$2

start_date_time="${start_date}:000000"
end_date_time="${end_date}:235959"

minutes_between=`minutes_between.e $start_date_time $end_date_time`

years_between=$((minutes_between / minutes_per_year))

echo $years_between

exit 0
