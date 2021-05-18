:
# $APPASERVER_HOME/src_predictive/bank_upload_transaction_balance.sh
# ------------------------------------------------------------------
# Freely available software. See appaserver.org
# ------------------------------------------------------------------

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

# Input
# -----

echo "$0" "$*" 1>&2

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 process_name transaction_date_time output_medium" 1>&2
	exit 1
fi

process_name=$1
transaction_date_time=$2

process_title=`echo "$process_name" | format_initial_capital.e`

output_medium=$3
if [ "$output_medium" = "" -o "$output_medium" = "output_medium" ]
then
	output_medium="table"		# Defaults to table
fi

if [ "$output_medium" != "stdout" ]
then
	content_type_cgi.sh

	echo "<html><head><link rel=stylesheet type=text/css href=/appaserver/$application/style.css></head>"
	echo "<body><h1>$process_title</h1>"
fi

# Constants
# ---------

select="transaction_date_time,bank_date,bank_description,full_name,transaction_amount,bank_amount,cash_running_balance,bank_running_balance,sequence_number"

table="bank_upload_transaction_balance"

order="transaction_date_time"

justification="left,left,left,left,right"

if [	"$transaction_date_time" = "" \
-o	"$transaction_date_time" = "transaction_date_time" ]
then
	where="1 = 1"
else
	where="transaction_date_time >= '$transaction_date_time'"
fi

# Output
# ------
if [ $output_medium = "table" ]
then
	select.sh $select $table "$where" $order		|
	html_table '' $select '^' $justification		|
	cat
else
	echo $select | tr ',' '^'
	select.sh $select $table "$where" $order		|
	cat
fi


if [ "$output_medium" != "stdout" ]
then
	echo "</body></html>"
fi

exit 0
