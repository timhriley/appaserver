:
# $APPASERVER_HOME/src_predictive/ledger_debit_credit_audit.sh
# ------------------------------------------------------------

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

echo $0 $* 1>&2

# Input
# -----
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 process_name begin_date" 1>&2
	exit 1
fi

process_name=$1                       	# Assumed letters_and_underbars
begin_date=$2

heading="full_name,street_address,transaction_date_time,transaction_difference,difference_type"

justification="left,left,left,right,left"

process_title=`echo "$process_name" | format_initial_capital.e`

content_type_cgi.sh

echo "<html><head><link rel=stylesheet type=text/css href=/appaserver/$application/style.css></head>"
echo "<body><h1>$process_title</h1>"

ledger_debit_credit_audit "$begin_date"		|
html_table.e "" "$heading" "^" "$justification"	|
cat

echo "</body></html>"

exit 0

