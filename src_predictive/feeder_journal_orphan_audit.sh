:
# $APPASERVER_HOME/src_predictive/feeder_journal_orphan_audit.sh
# --------------------------------------------------------------

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
feeder_account="$2"
minimum_transaction_date=$3

if [	"$minimum_transaction_date" = "" -o				\
	"$minimum_transaction_date" = "minimum_transaction_date" ]
then
	sql_statement="	select min( transaction_date_time )		\
			from feeder_row					\
			where feeder_account = '$feeder_account';"

	minimum_transaction_date=`echo $sql_statement | sql.e | column.e 0`
fi


document_body.sh $application

echo "<h1>`echo "$process_name" | format_initial_capital.e`</h1>"

if [ "$feeder_account" = "feeder_account" ]
then
	echo "<h3>Please select a Feeder Account</h3>"
	echo "</body></html>"
	exit 0
fi

heading="transaction_date_time,full_name,check_number,debit_amount,credit_amount,balance"

justification="left,left,right,right"

select="select journal.transaction_date_time,journal.full_name,check_number,debit_amount,credit_amount,balance"

journal_subquery="not exists ( select 1 from feeder_row where feeder_row.feeder_account = journal.account and feeder_row.full_name = journal.full_name and feeder_row.street_address = journal.street_address and feeder_row.transaction_date_time = journal.transaction_date_time )"

transaction_subquery="not exists ( select 1 from transaction where journal.full_name = transaction.full_name and journal.street_address = transaction.street_address and journal.transaction_date_time = transaction.transaction_date_time and memo = 'Opening entry')"

join="journal.full_name = transaction.full_name and journal.street_address = transaction.street_address and journal.transaction_date_time = transaction.transaction_date_time"

where="where account = '$feeder_account' and journal.transaction_date_time >= '$minimum_transaction_date' and $join and $journal_subquery and $transaction_subquery"

order="order by journal.transaction_date_time"

table_title=`echo "$feeder_account $minimum_transaction_date" | format_initial_capital.e`

echo "$select from journal,transaction $where $order;"	|
sql								|
html_table.e "$table_title" "$heading" "^" "$justification"	|
cat

echo "</body></html>"

exit 0

