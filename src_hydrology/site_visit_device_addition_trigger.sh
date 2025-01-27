:
if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "APPASERVER_DATABASE is not set." 1>&2
	exit 1
fi

echo Starting: $0 $* 1>&2

if [ "$#" -ne 6 ]
then
	echo "Usage: $0 station device_type manufacturer model serial_number state" 1>&2
	exit 1
fi

station=$1
device_type="$2"
manufacturer="$3"
model="$4"
serial_number="$5"
state=$6

if [ "$state" = "insert" ]
then
	where="device_type = '$device_type' and manufacturer = '$manufacturer' and model = '$model' and serial_number = '$serial_number'"

	echo "update device set current_station = '$station', inventory_status = 'in_use' where $where;" |
	sql.e
fi

exit 0
