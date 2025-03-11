#!/bin/bash
# ----------------------------------------------------------
# $APPASERVER_HOME/src_predictive/insert_cash_transaction.sh
# ----------------------------------------------------------

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
	echo "Usage: `basename.e $0 n` process full_name street_address transaction_date account transaction_amount check_number memo program_name" 1>&2
	exit 1
fi

echo "$0 $*" 1>&2

process=$1
full_name="$2"
street_address="$3"
transaction_date=$4
debit_account="$5"
transaction_amount=$6
check_number=$7
memo="$8"
program_name="$9"

cat << all_done
<html>
<head>
<link rel=stylesheet type="text/css" href="/appaserver/template/style.css">
</head>
<body>
<h1>`echo $process | format_initial_capital.e`</h1>
all_done

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

# Build where_clause
# ------------------
if [	"$check_number" = "" -o				\
	"$check_number" = "check_number" ]
then
	where="hard_coded_account_key = 'cash_key'"
else
	where="hard_coded_account_key = 'uncleared_checks_key'"
fi

credit_account=`echo "	select account		\
			from account		\
			where $where;"		|
			sql.e`

if [ "$credit_account" = "" ]
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

full_name_escaped=`echo $full_name | escape_character.e "'"`

if [ "$street_address" = "street_address" -o "$street_address" = "" ]
then
	street_address="unknown"
fi

# Check for duplication
# ---------------------
from=transaction

where="full_name = '$full_name_escaped' and street_address = '$street_address' and transaction_date_time like '$transaction_date %' and transaction_amount = $transaction_amount"

result=`echo "select count(1) from $from where $where;" | sql.e`

if [ "$result" -ge 1 ]
then
	echo "<h2> `now.sh 19` </h2>"
	echo "<h3>Duplication Error. You have to Insert->PB&J->Transaction instead.</h3>"
	echo "</body>"
	echo "</html>"
	exit 0
fi

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

# Insert transaction
# ------------------
table="transaction"

if [ "$program_name" != "" -a "$program_name" != "program_name" ]
then

	field="full_name,street_address,transaction_date_time,transaction_amount,check_number,memo,program_name"

	result=`echo "$full_name^$street_address^$transaction_date_time^$transaction_amount^$check_number^$memo^$program_name"				|\
		insert_statement.e table=$table field=$field del='^'	|\
		sql.e 2>&1`

else

	field="full_name,street_address,transaction_date_time,transaction_amount,check_number,memo"

	result=`echo "$full_name^$street_address^$transaction_date_time^$transaction_amount^$check_number^$memo"					|\
		insert_statement.e table=$table field=$field del='^'	|\
		sql.e 2>&1`

fi

if [ "$result" != "" ]
then
	echo "<h2> `now.sh 19` </h2>"
	echo "<h3>$result</h3>"
	echo "</body>"
	echo "</html>"
	exit 0
fi

table="journal"

# Insert debit journal
# --------------------
field="full_name,street_address,transaction_date_time,account,debit_amount"

echo "$full_name^$street_address^$transaction_date_time^$debit_account^$transaction_amount"							|
insert_statement.e table=$table field=$field del='^'		|
sql.e 2>&1							|
html_paragraph_wrapper.e

# Insert credit journal
# ---------------------
field="full_name,street_address,transaction_date_time,account,credit_amount"

echo "$full_name^$street_address^$transaction_date_time^$credit_account^$transaction_amount"							|
insert_statement.e table=$table field=$field del='^'		|
sql.e 2>&1							|
html_paragraph_wrapper.e

# Insert entity
# -------------
field="full_name,street_address"

echo "$full_name^$street_address"				|
insert_statement.e table=entity field=$field del='^'		|
sql.e 2>&1							|
grep -vi duplicate						|
html_paragraph_wrapper.e

# Execute the post change process for debit and credit accounts
# -------------------------------------------------------------
post_change_journal.sh		insert				\
				"$full_name"			\
				"$street_address"		\
				"$transaction_date_time"	\
				"$debit_account"		\
				preupdate_transaction_date_time	\
				preupdate_account

post_change_journal.sh		insert				\
				"$full_name"			\
				"$street_address"		\
				"$transaction_date_time"	\
				"$credit_account"		\
				preupdate_transaction_date_time	\
				preupdate_account

echo "<h2> `now.sh 19` </h2>"
echo "<h3>Process complete for transaction:"

transaction_html_display "$transaction_date_time"

echo "</body>"
echo "</html>"

exit 0

