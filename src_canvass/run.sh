:
if [ "$#" -ne 1 ]
then
	echo "Usage $0 maximum_weight" 1>&2
	exit 1
fi

start_street_address="DELGADO WAY"
city="SACRAMENTO"
state_code="CA"
canvass_name="discovery 1"
utm_zone=10

maximum_weight=$1

export DATABASE=discovery

heading="Street,Houses,Apartments,Total"

# Send to screen
# --------------
echo "$heading"
canvass_execute	"$start_street_address"		\
		"$city"				\
		"$state_code"			\
		"$canvass_name"			\
		"$maximum_weight"		\
		$utm_zone			|
piece_sum.e '^' 1,2,3				|
double_quote_comma_delimited.e '^'		|
cat

# Send to file
# ------------
(
echo "$heading"
canvass_execute	"$start_street_address"		\
		"$city"				\
		"$state_code"			\
		"$canvass_name"			\
		"$maximum_weight"		\
		$utm_zone			|
piece_sum.e '^' 1,2,3				|
double_quote_comma_delimited.e '^'		|
cat
) | cat > /tmp/household.csv
echo "Created: /tmp/household.csv"

# Send to apartment.sh
# --------------------
canvass_execute	"$start_street_address"		\
		"$city"				\
		"$state_code"			\
		"$canvass_name"			\
		"$maximum_weight"		\
		$utm_zone			|
apartment.sh 					|
cat > /tmp/apartment.csv
echo "Created: /tmp/apartment.csv"

# Send to lookup.sh
# -----------------
canvass_execute	"$start_street_address"		\
		"$city"				\
		"$state_code"			\
		"$canvass_name"			\
		"$maximum_weight"		\
		$utm_zone			|
lookup.sh

exit 0
