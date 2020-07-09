:

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 year" 1>&2
	exit 1
fi

year=$1

s=station
sd=station_datatype
m=measurement

select="$sd.station, $s.station_type, $sd.datatype"

from="$sd, $s"

join="$sd.station = $s.station"

filter="$s.station_type is not null"

where="$join and $filter and 						\
	exists (select 1						\
		from $m							\
		where $m.station = $sd.station				\
		  and $m.datatype = $sd.datatype			\
		  and $m.measurement_date between '$year-01-01'		\
					      and '$year-01-02')"

echo "select $select from $from where $where order by $select;"		|
sql.e

exit 0

