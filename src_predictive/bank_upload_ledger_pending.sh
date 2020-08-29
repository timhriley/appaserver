#!/bin/bash
# -------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/bank_upload_ledger_pending.sh
# -------------------------------------------------------------

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

echo "$0 $*" 1>&2

if [ "$application" = "timriley" ]
then
	development_where="transaction_date_time >= '2018-01-01'"
else
	development_where="1 = 1"
fi

JOURNAL_TABLE="journal_ledger"

select="transaction_date_time,full_name,street_address,debit_amount,credit_amount"

order="transaction_date_time"

table="${JOURNAL_TABLE},account"

journal_where="${JOURNAL_TABLE}.account = account.account and		\
			account.hard_coded_account_key = 'cash_key'"

subquery_where="not exists (
		select 1						\
		from bank_upload_transaction				\
		where ${JOURNAL_TABLE}.full_name =			\
			bank_upload_transaction.full_name and		\
		      ${JOURNAL_TABLE}.street_address =			\
			bank_upload_transaction.street_address and	\
		      ${JOURNAL_TABLE}.transaction_date_time =		\
			bank_upload_transaction.transaction_date_time )"

where="$journal_where and $subquery_where and $development_where"

echo "select $select from $table where $where order by $order;"		|
sql.e									|
cat

exit 0
