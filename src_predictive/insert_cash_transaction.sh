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
	echo "Usage: `basename.e $0 n` process full_name street_address transaction_date account transaction_amount check_number memo fund" 1>&2
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
fund="$9"

content_type_cgi.sh

cat << all_done
<html>
<head>
<link rel=stylesheet type="text/css" href="/appaserver/$application/style.css">
</head>
<body>
<h1>`echo $process | format_initial_capital.e`</h1>
all_done

# Build where_clause
# ------------------
if [ "$fund" = "" -o "$fund" = "fund" ]
then
	where="hard_coded_account_key = 'cash_key'"
else
	where="hard_coded_account_key = 'cash_key' and fund = '$fund'"
fi

credit_account=`echo "	select account		\
			from account		\
			where $where;"		|
			sql.e`

if [ "$credit_account" = "" ]
then
	echo "ERROR in `basename.e $0 n`: cannot get account.hard_coded_account_key = 'cash_key'" 1>&2
	exit 1
fi

# Build transaction_date_time
# ---------------------------
if [ "$transaction_date" = "" -o "$transaction_date" = "transaction_date" ]
then
	transaction_date=`now.sh ymd`
fi

transaction_date_time="${transaction_date} `now.sh seconds`"

# Check for duplication
# ---------------------
full_name_escaped=`echo $full_name | escape_character.e "'"`

from=transaction

where="full_name = '$full_name_escaped' and street_address = '$street_address' and transaction_date_time like '$transaction_date %' and transaction_amount = $transaction_amount"

results=`echo "select count(1) from $from where $where;" | sql.e`

if [ "$results" -ge 1 ]
then
	echo "<h2> `now.sh 19` </h2>"
	echo "<h3>Duplication Error</h3>"
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

field="full_name,street_address,transaction_date_time,transaction_amount,check_number,memo"

results=`echo "$full_name^$street_address^$transaction_date_time^$transaction_amount^$check_number^$memo" | insert_statement.e table=$table field=$field del='^' | sql.e 2>&1`

if [ "$results" != "" ]
then
	echo "<h3>$results</h3>"
	echo "</body>"
	echo "</html>"
	exit 0
fi

table="journal_ledger"

# Insert debit journal_ledger
# ---------------------------
field="full_name,street_address,transaction_date_time,account,debit_amount"

echo "$full_name^$street_address^$transaction_date_time^$debit_account^$transaction_amount"							|
insert_statement.e table=$table field=$field del='^'		|
sql.e 2>&1							|
html_paragraph_wrapper.e

# Insert credit journal_ledger
# ----------------------------
field="full_name,street_address,transaction_date_time,account,credit_amount"

echo "$full_name^$street_address^$transaction_date_time^$credit_account^$transaction_amount"							|
insert_statement.e table=$table field=$field del='^'		|
sql.e 2>&1							|
html_paragraph_wrapper.e

# Execute the post change process for debit and credit accounts
# -------------------------------------------------------------
post_change_journal_ledger.sh	insert				\
				"$full_name"			\
				"$street_address"		\
				"$transaction_date_time"	\
				"$debit_account"		\
				preupdate_transaction_date_time	\
				preupdate_account

post_change_journal_ledger.sh	insert				\
				"$full_name"			\
				"$street_address"		\
				"$transaction_date_time"	\
				"$credit_account"		\
				preupdate_transaction_date_time	\
				preupdate_account

echo "<h2> `now.sh 19` </h2>"

echo "<h3>Process complete</h3>"


echo "</body>"
echo "</html>"

exit 0

