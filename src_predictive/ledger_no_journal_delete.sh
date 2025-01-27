:
# $APPASERVER_HOME/src_predictive/ledger_no_journal_delete.sh
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

# Input
# -----
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 minimum_transaction_date" 1>&2
	exit 1
fi

minimum_transaction_date=$1

table=transaction
field=full_name,street_address,transaction_date_time

ledger_debit_credit_audit "$minimum_transaction_date"		|
grep 'no_journal$'						|
delete_statement table=$table field=$field delimiter='^'	|
cat

exit 0

