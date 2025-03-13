#!/bin/bash
# ------------------------------------------------------
# $APPASERVER_HOME/src_predictive/insert_cash_revenue.sh
# ------------------------------------------------------

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "ERROR in `basename.e $0 n`: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

if [ "$#" -ne 9 ]
then
	echo "Usage: `basename.e $0 n` process full_name street_address transaction_date account transaction_amount memo program_name" 1>&2
	exit 1
fi

echo "$0 $*" 1>&2

process=$1
full_name="$2"
street_address="$3"
transaction_date=$4
credit_account="$5"
transaction_amount=$6
memo="$7"
program_name="$8"

document_body.sh

echo "<h1>`echo $process | format_initial_capital.e`</h1>"

if [	"$full_name" = "" -o				\
	"$full_name" = "full_name" ]
then
	echo "<h2> `now.sh 19` </h2>"
	echo "<h3>Please choose an entity.</h3>"
	echo "</body>"
	echo "</html>"
	exit 0
fi

if [	"$credit_account" = "" -o			\
	"$credit_account" = "account" -o		\
	"$credit_account" = "null" ]
then
	echo "<h2> `now.sh 19` </h2>"
	echo "<h3>Please choose an account.</h3>"
	echo "</body>"
	echo "</html>"
	exit 0
fi

if [	"$transaction_date" = "" -o			\
	"$transaction_date" = "transaction_date" ]
then
	echo "<h2> `now.sh 19` </h2>"
	echo "<h3>Please enter in a transaction date.</h3>"
	echo "</body>"
	echo "</html>"
	exit 0
fi

if [	"$transaction_amount" = "" -o			\
	"$transaction_amount" = "transaction_amount" ]
then
	echo "<h2> `now.sh 19` </h2>"
	echo "<h3>Please enter in a transaction amount.</h3>"
	echo "</body>"
	echo "</html>"
	exit 0
fi

# Build the entity
# ----------------
full_name_escaped=`echo $full_name | escape_character.e "'"`

if [ "$street_address" = "street_address" -o "$street_address" = "" ]
then
	street_address="unknown"
fi

# Check for duplication
# ---------------------
from=journal

where="full_name = '$full_name_escaped' and street_address = '$street_address' and account = '$credit_account' and transaction_date_time like '$transaction_date %' and credit_amount = $transaction_amount"

result=`echo "select count(1) from $from where $where;" | sql.e`

if [ "$result" -ge 1 ]
then
	echo "<h2> `now.sh 19` </h2>"
	echo "<h3>Duplication Error. Please follow this path: Insert->PB&J->Transaction.</h3>"
	echo "</body>"
	echo "</html>"
	exit 0
fi

# Get the debit_account
# ---------------------
where="hard_coded_account_key = 'cash_key'"

debit_account=`echo "	select account		\
			from account		\
			where $where;"		|
			sql.e`

if [ "$debit_account" = "" ]
then
	echo "ERROR in `basename.e $0 n`: cannot get account.hard_coded_account_key = '$hard_coded_account_key'" 1>&2
	exit 1
fi

# Build transaction_date_time
# ---------------------------
if [ "$transaction_date" = "" -o "$transaction_date" = "transaction_date" ]
then
	transaction_date=`now.sh ymd`
fi

transaction_date_time="${transaction_date} `now.sh seconds`"

# Build memo
# ----------
if [ "$memo" = "memo" ]
then
	memo=""
fi

insert_cash_transaction.sh	"$full_name_escaped"		\
				"$street_address"		\
				"$transaction_date_time"	\
				"$debit_account"		\
				"$credit_account"		\
				"$transaction_amount"		\
				""				\
				"$memo"				\
				"$program_name"

echo "</body>"
echo "</html>"

exit 0

