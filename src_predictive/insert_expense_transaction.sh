#!/bin/bash
# -------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/insert_expense_transaction.sh
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
	echo "ERROR in `basename.e $0 n`: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

if [ "$#" -ne 9 ]
then
	echo "Usage: `basename.e $0 n` process feeder_account full_name street_address transaction_date account transaction_amount check_number memo" 1>&2
	exit 1
fi

process="$1"
feeder_account="$2"
full_name="$3"
street_address="$4"
transaction_date="$5"
debit_account="$6"
transaction_amount="$7"
check_number="$8"
memo="$9"

document_body.sh

echo "<h1>`echo $process | format_initial_capital.e`</h1>"

if [	"$feeder_account" = "" -o	\
	"$feeder_account" = "feeder_account" ]
then
	echo "<h2> `now.sh 19` </h2>"
	echo "<h3>Please choose a feeder account .</h3>"
	echo "</body>"
	echo "</html>"
	exit 0
fi

credit_account="$feeder_account"

if [	"$full_name" = "" -o				\
	"$full_name" = "full_name" ]
then
	echo "<h2> `now.sh 19` </h2>"
	echo "<h3>Please choose an entity.</h3>"
	echo "</body>"
	echo "</html>"
	exit 0
fi

if [	"$debit_account" = "" -o			\
	"$debit_account" = "account" -o			\
	"$debit_account" = "null" ]
then
	echo "<h2> `now.sh 19` </h2>"
	echo "<h3>Please choose a nominal account.</h3>"
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

if [	"$check_number" != "" -a			\
	"$check_number" != "check_number" ]
then
	where="check_number = '$check_number'"

	count=`echo "	select count(1)		\
			from transaction	\
			where $where;"		|
			sql.e`

	if [ $count -eq 1 ]
	then
		echo "<h2> `now.sh 19` </h2>"
		echo "<h3>Check number $check_number already exists.</h3>"
		echo "</body>"
		echo "</html>"
		exit 0
	fi
fi

# Build the entity
# ----------------
if [ "$street_address" = "street_address" -o "$street_address" = "" ]
then
	street_address="any"
fi

# Check for duplication
# ---------------------
from=journal

where="full_name = '$full_name' and street_address = '$street_address' and account = '$debit_account' and transaction_date_time like '$transaction_date %' and debit_amount = $transaction_amount"

result=`echo "select count(1) from $from where $where;" | sql.e`

if [ "$result" -ge 1 ]
then
	echo "<h2> `now.sh 19` </h2>"
	echo "<h3>Duplication Error. Please follow this path: Insert --> Transaction -->Transaction</h3>"
	echo "</body>"
	echo "</html>"
	exit 0
fi

# Get the credit_account if check
# -------------------------------
if [	"$check_number" != "" -a			\
	"$check_number" != "check_number" ]
then
	where="hard_coded_account_key = 'uncleared_checks'"

	credit_account=`echo "	select account		\
				from account		\
				where $where;"		|
				sql.e`

	if [ "$credit_account" = "" ]
	then
		echo "ERROR in $0: cannot get account where = '$where'" 1>&2
		exit 1
	fi
	exit 1
fi

# Build transaction_date_time
# ---------------------------
if [ "$transaction_date" = "" -o "$transaction_date" = "transaction_date" ]
then
	transaction_date=`now.sh ymd`
fi

transaction_date_time="${transaction_date} `now.sh seconds`"

# Build check_number
# ------------------
if [ "$check_number" = "check_number" ]
then
	check_number=""
fi

# Build memo
# ----------
if [ "$memo" = "memo" ]
then
	memo=""
fi

insert_expense_transaction_process.sh	"$full_name"			\
					"$street_address"		\
					"$transaction_date_time"	\
					"$debit_account"		\
					"$credit_account"		\
					"$transaction_amount"		\
					"$check_number"			\
					"$memo"

echo "<h2> `now.sh 19` </h2>"
echo "<h3>Process complete for transaction:"

transaction_html_display "$transaction_date_time"

echo "</body>"
echo "</html>"

exit 0

