:
# $APPASERVER_HOME/src_predictive/feeder_row_journal_audit.sh
# -----------------------------------------------------------

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

echo `basename.e $0 n` $* 1>&2

# Input
# -----
if [ "$#" -ne 3 ]
then
	echo "Usage: $0 process_name feeder_account minimum_transaction_date" 1>&2
	exit 1
fi

process_name=$1                       	# Assumed letters_and_underbars
feeder_account=$2
minimum_transaction_date=$3

title_html="<h1>`echo "$process_name" | format_initial_capital.e`</h1>"

if [ "$feeder_account" = "feeder_account" ]
then
	document_body.sh $application
	echo "$title_html"

	echo "<h3>Please select a Feeder Account</h3>"
	echo "</body></html>"
	exit 0
fi

if [	"$minimum_transaction_date" = "" -o				\
	"$minimum_transaction_date" = "minimum_transaction_date" ]
then
	sql_statement="	select min( transaction_date_time )		\
			from feeder_row					\
			where feeder_account = '$feeder_account';"

	minimum_transaction_date=`echo $sql_statement | sql.e | column.e 0`
fi

if [ "$minimum_transaction_date" = "" ]
then
	document_body.sh $application
	echo "$title_html"

	echo "<h3>There are no transactions to audit.</h3>"
	echo "</body></html>"
	exit 0
fi

sql_statement="						      		      \
	select accumulate_debit_yn					      \
	from account,subclassification,element				      \
	where account.account = '$feeder_account'			      \
	  and account.subclassification = subclassification.subclassification \
	  and subclassification.element = element.element;"

accumulate_debit_yn=`echo $sql_statement | sql.e`

if [ "$accumulate_debit_yn" != "y" ]
then
	journal_balance="-journal.balance"
else
	journal_balance="journal.balance"
fi

heading="transaction_date_time,feeder_row_number,feeder_date,feeder_load_date_time,full_name,file_row_amount,file_row_balance,calculate_balance,ledger_balance,difference"

justification="left,right,left,left,left,right"

table_title=`echo "$feeder_account $minimum_transaction_date" | format_initial_capital.e`

select="select feeder_row.transaction_date_time,feeder_row.feeder_row_number,feeder_row.feeder_date,feeder_row.feeder_load_date_time,feeder_row.full_name,file_row_amount,file_row_balance,calculate_balance,$journal_balance,calculate_balance - $journal_balance"

from="from feeder_row,journal"

where="where feeder_row.feeder_account = '$feeder_account' and feeder_row.transaction_date_time >= '$minimum_transaction_date' and feeder_row.full_name = journal.full_name and feeder_row.street_address = journal.street_address and feeder_row.transaction_date_time = journal.transaction_date_time and journal.account = '$feeder_account'"

order="order by feeder_row.transaction_date_time"

document_body.sh $application
echo "$title_html"

echo "$select $from $where $order;"				|
sql								|
html_table.e "$table_title" "$heading" "^" "$justification"	|
cat

echo "</body></html>"

exit 0

