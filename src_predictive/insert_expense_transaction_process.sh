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

if [ "$#" -lt 8 ]
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

# Integrity check fund_name
# -------------------------
if [ "$#" -eq 9 ]
then
	fund_name="$9"
fi

table_column_exists.sh fund fund_name

if [ $? -eq 0 ]
then
	fund_field=",fund_name"
	fund_datum="^$fund_name"
fi

table="transaction"

field="full_name,street_address,transaction_date_time,transaction_amount,check_number,memo${fund_field}"

result=`echo "$full_name^$street_address^$transaction_date_time^$transaction_amount^$check_number^$memo${fund_datum}"				|\
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
field="full_name,street_address,transaction_date_time,account,debit_amount${fund_field}"

echo "$full_name^$street_address^$transaction_date_time^$debit_account^$transaction_amount${fund_datum}"					|
insert_statement.e table=$table field=$field del='^'		|
sql.e 2>&1							|
html_paragraph_wrapper.e

# Insert credit journal
# ---------------------
field="full_name,street_address,transaction_date_time,account,credit_amount${fund_field}"

echo "$full_name^$street_address^$transaction_date_time^$credit_account^$transaction_amount${fund_datum}"					|
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
				preupdate_account		\
				"$fund_name"

post_change_journal.sh		insert				\
				"$full_name"			\
				"$street_address"		\
				"$transaction_date_time"	\
				"$credit_account"		\
				preupdate_transaction_date_time	\
				preupdate_account		\
				"$fund_name"

exit 0

