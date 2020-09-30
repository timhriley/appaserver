#!/bin/bash
# src_timriley/accrue_salary_receivable.sh
# -----------------------------------------------
# Freely available software: see Appaserver.org
# -----------------------------------------------

echo "$0" "$*" 1>&2

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application process execute_yn" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)	# May have appended database
process_name=$2                       	# Assumed letters_and_underbars
execute_yn=$3

export DATABASE=$application

process_title=$(echo "$process_name" | format_initial_capital.e)

self=`echo "	select full_name,street_address		\
		from self;"				|
		sql.e '|'`

transaction_date_time=`now.sh 19`

days_passed=$(date_subtract.e `now.sh ymd` `now.sh ymd | piece.e '-' 0`-01-01)

salary_receivable_account=`echo "	select account			  \
					from account			  \
					where hard_coded_account_key =	  \
						'salary_receivable_key';" |
				sql.e`

if [ "$salary_receivable_account" = "" ]
then
	echo "Error: cannot find account with key = salary_receivable_key" 1>&2
	exit 1
fi

accrued_daily_amount=`echo "	select accrued_daily_amount		\
				from account				\
				where hard_coded_account_key =		\
					'salary_receivable_key';"	|
			sql.e`

salary_revenue_account=`echo "	select account			\
				from account			\
				where hard_coded_account_key =	\
					'salary_revenue_key';"	|
			sql.e`

if [ "$salary_revenue_account" = "" ]
then
	echo "Error: cannot find account with key = salary_revenue_key" 1>&2
	exit 1
fi

max_transaction_date=`echo "	select max( transaction_date_time )	\
				from journal_ledger			\
				where account =				\
					'$salary_receivable_account';"	|
			sql.e`

if [ "$max_transaction_date" = "" ]
then
	current_salary_receivable_balance="0"
else
	current_salary_receivable_balance=`				\
		echo "	select balance					\
			from journal_ledger				\
			where account = '$salary_receivable_account'	\
			  and transaction_date_time =			\
				'$max_transaction_date';"		|
		sql.e`
fi

salary_receivable_balance=`						\
	echo "$accrued_daily_amount|$days_passed"			|
	piece_arithmetic.e 0 1 multiply '|'				|
	piece.e '|' 2`

salary_receivable_difference=`						     \
	echo "$salary_receivable_balance|$current_salary_receivable_balance" |
		piece_arithmetic.e 0 1 subtract '|'			     |
		piece.e '|' 2`

a=$salary_receivable_account
b=$salary_revenue_account
c=$salary_receivable_difference
d="left,right,right"

transaction=\
"full_name,street_address,transaction_date_time,transaction_amount,memo"

journal=\
"full_name,street_address,transaction_date_time,account,debit_amount,credit_amount"

content_type_cgi.sh

echo "<html><head><link rel=stylesheet type=text/css href=/appaserver/$application/style.css></head>"
echo "<body><h1>$process_title</h1>"

if [ "$execute_yn" = "y" ]
then
	echo "$self|$transaction_date_time|$c|$process_title"		|
	insert_statement.e table=transaction field=$transaction del='|'	|
	sql.e

	echo "$self|$transaction_date_time|$a|$c|"			|
	insert_statement.e table=journal_ledger field=$journal del='|'	|
	sql.e

	echo "$self|$transaction_date_time|$b||$c"			|
	insert_statement.e table=journal_ledger field=$journal del='|'	|
	sql.e

	ledger_propagate $application "$transaction_date_time" '' $a $b

	echo "<h3>Process complete.</h3>"
else
	/bin/echo -e "$a|$c|\n$b||$c" | html_table.e '' ',Debit,Credit' '|' $d
fi

echo "</body>"

exit 0
