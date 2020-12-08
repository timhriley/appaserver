:
if [ "$#" -ne 3 ]
then
	echo "Usage: $0 station datatype begin_measurement_date" 1>&2
	exit 1
fi

station=$1
datatype=$2
begin_measurement_date=$3

select="station,datatype,measurement_date,measurement_time"

from=measurement_backup

station_where="station = '$station' and datatype = '$datatype'"

where="$station_where and measurement_date >= '$begin_measurement_date'"

echo "select $select from $from where $where;"		|
sql							|
group.e '|' count					|
piece_shift_right.e '|'					|
sed 's/|/ [/'						|
sed 's/$/]/'						|
cat

exit 0
