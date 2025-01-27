:
# last_day_of_month.sh
# --------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 uid_pwd MON-YY" 1>&2
	exit 1
fi

echo "select last_day( '01-$2' ) from dual;" | select.sh $1

exit 1

