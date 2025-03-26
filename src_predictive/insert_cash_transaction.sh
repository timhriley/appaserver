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

echo "$0 $*" 1>&2

if [ "$#" -ne 9 ]
then
	echo "Usage: `basename.e $0 n` full_name street_address transaction_date_time debit_account credit_account transaction_amount check_number memo program_name" 1>&2
	exit 1
fi

full_name="$(echo $1 | escape_security.e)"
street_address="$(echo $2 | escape_security.e)"
transaction_date_time="$(echo $3 | escape_security.e)"
debit_account="$(echo $4 | escape_security.e)"
credit_account="$(echo $5 | escape_security.e)"
transaction_amount="$(echo $6 | escape_security.e)"
check_number="$(echo $7 | escape_security.e)"
memo="$(echo $8 | escape_security.e)"
program_name="$(echo $9 | escape_security.e)"

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

