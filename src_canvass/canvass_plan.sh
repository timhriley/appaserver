:
# $APPASERVER_HOME/src_canvass/canvass_plan.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

if [ "$#" -ne 2 ]
then
	echo "Usage $0 process maximum_weight" 1>&2
	exit 1
fi

process="$1"
maximum_weight=$2

start_street_address="DELGADO WAY"
city="SACRAMENTO"
state_code="CA"
canvass_name="discovery 1"
utm_zone=10

document_body.sh

title_html="<h1>`echo "$process" | format_initial_capital.e`</h1>"
echo "$title_html"
echo "<h2> `now.sh 19` </h2>"

heading="Street,Apartments,Houses,Total"

# Send to screen
# --------------
canvass_execute	"$start_street_address"		\
		"$city"				\
		"$state_code"			\
		"$canvass_name"			\
		"$maximum_weight"		\
		$utm_zone			|
piece_sum.e '^' 1,2,3				|
html_table.e '' "$heading" '^' "left,right"
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

echo "Created: /tmp/household.csv"		|
html_paragraph_wrapper.e

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

echo "Created: /tmp/apartment.csv"		|
html_paragraph_wrapper.e

# Send to lookup.sh
# -----------------
canvass_execute	"$start_street_address"		\
		"$city"				\
		"$state_code"			\
		"$canvass_name"			\
		"$maximum_weight"		\
		$utm_zone			|
lookup.sh					|
html_paragraph_wrapper.e

document_close.sh

exit 0
