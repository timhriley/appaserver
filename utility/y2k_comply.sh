:
# y2k_comply.sh
# ----------------------
# Tim Riley
# ----------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 DD-MON-YY years_future_event" 1>&2
	exit 1
fi

day=`echo $1 | piece.e '-' 0`
month=`echo $1 | piece.e '-' 1`
year=`echo $1 | piece.e '-' 2 | xargs.e full_year.e {} $2`

echo "$day-$month-$year"

exit 0

