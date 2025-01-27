:
# $APPASERVER_HOME/utility/date_count.sh
# --------------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 start_yyyy_mm_dd count" 1>&2
	exit 1
fi

start_date=$1
count=$2

count=`expr $count - 1`

end_date=`echo $start_date | date_add_days.e $count 0 0 '|'`

julian_time_list.e $start_date $end_date 1			|
piece.e '=' 1							|
piece.e ':' 0							|
cat

exit 0
