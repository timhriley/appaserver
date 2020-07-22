:
# date.sh
# -------

if [ "$#" -eq 0 ]
then
	#TZ=`appaserver_tz.sh` /bin/date '+%m/%d/%Y %H:%M'
	TZ=`appaserver_tz.sh` date.e 0 | sed 's/:/ /' | sed 's/[0-9][0-9]$/:&/'
else
	days_offset=$1

	TZ=`appaserver_tz.sh` julian_time.e $days_offset | piece.e ':' 0
fi

exit 0
