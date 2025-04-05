:
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 street_address_start city" 1>&2
	exit 1
fi

street_name_start="$1"
city="$2"

where="street_name = '$street_address_start' and city = '$city'"
statement="select longitude,latitude from street where $where;"
start_record=`echo "$statement" | sql.e`

echo $start_record

exit 0
