:
# $APPASERVER_HOME/utility/days_between.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

minutes_per_day=1440

if [ "$#" -ne 2 ]
then
	echo "Usage: `echo $0 | basename.e` start_yyyy_mm_dd end_yyyy_mm_dd" 1>&2
	exit 1
fi

start_date=$1
end_date=$2

start_date_time="${start_date}:000000"
end_date_time="${end_date}:235959"

minutes_between=`minutes_between.e $start_date_time $end_date_time`

days_between=$((minutes_between / minutes_per_day))

echo $days_between

exit 0
