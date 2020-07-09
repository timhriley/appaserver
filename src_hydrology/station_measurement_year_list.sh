:

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 year" 1>&2
	exit 1
fi

year=$1

s=station
m=measurement

select="$m.station, $s.station_type, $m.datatype"

from="$m, $s"

join="$m.station = $s.station"

filter="$s.station_type is not null"

where="$join and $filter and $m.measurement_date = '$year-01-01'"

echo "select $select from $from where $where;"		|
sql.e							|
sort -u

exit 0

