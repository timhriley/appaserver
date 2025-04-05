:

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 canvass_name street_address_start maximum_weight" 1>&2
	exit 1
fi

canvass_name="$1"
street_address_start="$2"
maximum_weight=$3

echo "Street,Longitude,Latitude,Houses,Apartments,Total"

(
canvass_street "$canvass_name" "$street_address_start" $maximum_weight	|
while read street_name
do
	canvass_street.sh						\
		"$canvass_name"						\
		y							|
		grep "^$street_name\^"

	canvass_street.sh						\
		"$canvass_name"						\
		n							|
		grep "^$street_name\^"
done
)									|
piece_sum.e '^' 3,4,5							|
double_quote_comma_delimited.e '^'					|
cat

exit $?
