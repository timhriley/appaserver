#!/bin/bash
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/program_journal.sh
# ---------------------------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 4 ]
then
	echo "Usage: $0 program element begin_date end_date" 1>&2
	exit 1
fi

program="$1"
parameter_element="$2"
begin_date=$3
end_date=$4

year=`date.e 0 | piece.e '-' 0`

if [ "$begin_date" = "" -o "$begin_date" = "begin_date" ]
then
	begin_date="${year}-01-01"
fi

if [ "$end_date" = "" -o "$end_date" = "end_date" ]
then
	end_date="${year}-12-31"
fi

begin_date_time="${begin_date} 00:00:00"
end_date_time="${end_date} 23:59:59"

common_select="journal.full_name,journal.transaction_date_time,journal.account"

account_join="journal.account = account.account"

subclassification_join="account.subclassification = subclassification.subclassification"

transaction_join="journal.full_name = transaction.full_name and journal.street_address = transaction.street_address and journal.transaction_date_time = transaction.transaction_date_time"

date_where="journal.transaction_date_time >= '${begin_date_time}' and journal.transaction_date_time <= '${end_date_time}'"

common_where="${date_where} and ${transaction_join} and ${subclassification_join} and ${account_join} and transaction.program_name = '$program'"

from="journal,transaction,account,subclassification"

function output_revenue()
{
element="revenue"

where="${common_where} and subclassification.element = '$element' and credit_amount != 0.0"

select="${common_select},credit_amount"

order="credit_amount desc"

echo "select $select from $from where $where order by $order;" | sql
}

function output_expense()
{
element="expense"

where="${common_where} and subclassification.element = '$element' and debit_amount != 0.0"

select="${common_select},debit_amount"

order="debit_amount desc"

echo "select $select from $from where $where order by $order;" | sql
}

function output_prepaid()
{
element="asset"
subclassification="prepaid"

where="${common_where} and subclassification.element = '$element' and subclassification.subclassification = '$subclassification' and debit_amount != 0.0"

select="${common_select},debit_amount"

order="debit_amount desc"

echo "select $select from $from where $where order by $order;" | sql
}

if [ "$parameter_element" = "revenue" ]
then
	output_revenue
elif [ "$parameter_element" = "expense" ]
then
	output_expense
elif [ "$parameter_element" = "prepaid" ]
then
	output_prepaid
else
	echo "Error in $0: invalid element" 1>&2
	exit 1
fi

exit 0
