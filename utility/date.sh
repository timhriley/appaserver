:
# date.sh
# -------

if [ "$#" -eq 0 ]
then
	TZ=`appaserver_tz.sh` /bin/date '+%m/%d/%Y %H:%M'
else
	days_offset=$1

	TZ=`appaserver_tz.sh` julian_time.e $days_offset | piece.e ':' 0
fi

exit 0
