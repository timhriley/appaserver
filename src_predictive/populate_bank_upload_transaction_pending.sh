#!/bin/bash
# ---------------------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/populate_bank_upload_transaction_pending.sh
# ---------------------------------------------------------------------------

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

debit_select="full_name,street_address,concat( transaction_date_time, ' [', debit_amount, ']')"

credit_select="full_name,street_address,concat( transaction_date_time, ' [', credit_amount, ']')"

table="journal,account"

journal_where="	journal.account = account.account and	\
			account.hard_coded_account_key = 'cash_key'"

debit_amount_where="ifnull( debit_amount, 0 ) <> 0"
credit_amount_where="ifnull( credit_amount, 0 ) <> 0"

subquery_where="not exists (
		select 1						\
		from bank_upload_transaction				\
		where journal.full_name =			\
			bank_upload_transaction.full_name and		\
		      journal.street_address =			\
			bank_upload_transaction.street_address and	\
		      journal.transaction_date_time =		\
			bank_upload_transaction.transaction_date_time )"

debit_where="$journal_where and $subquery_where and $development_where and $debit_amount_where"

credit_where="$journal_where and $subquery_where and $development_where and $credit_amount_where"

(
echo "select $debit_select from $table where $debit_where;"		|
sql.e									|
cat									;
echo "select $credit_select from $table where $credit_where;"		|
sql.e									|
cat									;
) | sort

exit 0

