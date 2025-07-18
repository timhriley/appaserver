#!/bin/bash
# ---------------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/insert_expense_transaction_process.sh
# ---------------------------------------------------------------------

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

echo "$0 $*" 1>&2

if [ "$#" -ne 8 ]
then
	echo "Usage: `basename.e $0 n` full_name street_address transaction_date_time debit_account credit_account transaction_amount check_number memo" 1>&2
	exit 1
fi

full_name="$1"
street_address="$2"
transaction_date_time="$3"
debit_account="$4"
credit_account="$5"
transaction_amount="$6"
check_number="$7"
memo="$8"

table="transaction"

field="full_name,street_address,transaction_date_time,transaction_amount,check_number,memo"

result=`echo "$full_name^$street_address^$transaction_date_time^$transaction_amount^$check_number^$memo"					|\
	insert_statement.e table=$table field=$field del='^'	|\
	sql.e 2>&1`

if [ "$result" != "" ]
then
	echo "<h2> `now.sh 19` </h2>"
	echo "<h3>$result</h3>"
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

exit 0

