:
if [ "$#" -ne 1 ]
then
	echo "Usage $0 maximum_weight" 1>&2
	exit 1
fi

canvass_name="discovery 1"
street_address_start="DELGADO WAY"
maximum_weight=$1

export DATABASE=discovery

# Send to screen
# --------------
household.sh "$canvass_name" "$street_address_start" $maximum_weight	|
piece_quote_comma 0,3,4,5 '^'						|
double_quote_comma_delimited.e '^'					|
cat

# Send to file
# ------------
household.sh "$canvass_name" "$street_address_start" $maximum_weight	|
piece_quote_comma 0,3,4,5 '^'						|
double_quote_comma_delimited.e '^'					|
cat > /tmp/household.csv
echo "Created: /tmp/household.csv"

# Send to apartment.sh
# --------------------
household.sh "$canvass_name" "$street_address_start" $maximum_weight	|
apartment.sh 								|
cat > /tmp/apartment.csv
echo "Created: /tmp/apartment.csv"

# Send to lookup.sh
# -----------------
household.sh "$canvass_name" "$street_address_start" $maximum_weight	|
lookup.sh

exit 0
