:

echo Starting: $0 $* 1>&2

if [ "$#" -lt 3 ]
then
	echo "Usage: $0 many_folder state station [where]" 1>&2
	exit 1
fi

many_folder=$1
state=$2
station=$3

if [ $# -eq 4 -a "$4" != "\$where" ]
then
	parameter_where="$4"
else
	parameter_where="1 = 1"
fi

select="device_type,manufacturer,model,serial_number"

many_where="1 = 1"
inventory_status_where="1 = 1"

if [ "$state" = "insert" ]
then
	if [ "$many_folder" = "site_visit_device_addition" ]
	then
		many_where="current_station is null"
		inventory_status_where="(inventory_status = 'inventory' or inventory_status is null)"
	elif [ "$many_folder" = "site_visit_device_removal" ]
	then
		many_where="current_station = '$station'"
	elif [ "$many_folder" = "device_repair" ]
	then
		many_where="current_station is null"
	fi
fi

#if [ "$state" = "lookup" ]
#then
#	if [ "$many_folder" = "site_visit_device_addition" ]
#	then
#		many_where="current_station = '$station'"
#	fi
#fi

where="$parameter_where and $many_where and $inventory_status_where"

echo "select $select from device where $where order by $select;"	|
sql.e

exit 0
