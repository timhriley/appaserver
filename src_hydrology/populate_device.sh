:

echo Starting: $0 $* 1>&2

if [ "$#" -ne 5 ]
then
	echo "Usage: $0 application many_folder state station where" 1>&2
	exit 1
fi

application=$1
many_folder=$2
state=$3
station=$4
parameter_where=$5

select="device_type,manufacturer,model,serial_number"

many_where="1 = 1"

if [ "$state" = "insert" ]
then
	if [ "$many_folder" = "site_visit_device_addition" ]
	then
		many_where="current_station is null"
	elif [ "$many_folder" = "site_visit_device_removal" ]
	then
		many_where="current_station = '$station'"
	elif [ "$many_folder" = "device_repair" ]
	then
		many_where="current_station is null"
	fi
fi

if [ "$state" = "lookup" ]
then
	if [ "$many_folder" = "site_visit_device_addition" ]
	then
		many_where="current_station = '$station'"
	fi
fi

where="$parameter_where and $many_where"

echo "select $select from device where $where order by $select;"	|
sql.e

exit 0
