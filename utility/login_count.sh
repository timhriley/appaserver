:
# utility/login_count.sh
# ----------------------

if [ $# -ne 2 ]
then
	echo "Usage: `echo $0 | basename.e` begin_date end_date" 1>&2
	exit 1
fi

begin_date=$1
end_date=$2

days_between=`days_between.sh "$begin_date" "$end_date"`

expression='date_ticker.e day $days_between "$begin_date" | column.e 0'

for last_access_date in `eval $expression`
do
	/bin/echo -n "${last_access_date}: "

	echo "	select count(*)
		from appaserver_sessions 
		where last_access_date = '$last_access_date';" |
	sql.e
done

exit 0
