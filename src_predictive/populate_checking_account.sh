#!/bin/bash
# ------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/populate_checking_account.sh
# ------------------------------------------------------------

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

echo $0 $* 1>&2

# Select
# ======

# Select chart_account_number?
# ----------------------------
select_chart_account_number=0

table_column_exists.sh account chart_account_number

# Zero means true
# ---------------
exists_chart_account_number=$?

if [ $exists_chart_account_number = 0 ]
then
	chart_account_number_count=`echo			\
		"select count(1)				\
		 from account					\
		 where chart_account_number is not null;"	|
		 sql.e`

	if [ "$chart_account_number_count" -ge 1 ]
	then
		# One means false
		# ---------------
		select_chart_account_number=1
	fi
fi

if [ $select_chart_account_number -eq 1 ]
then
	select="concat( account, '|', account, '---', ifnull( chart_account_number, '' ) )"
else
	select="concat( account, '|', account, ' [', account.subclassification, ']' )"
fi

# From
# ----
from="account"

# Order
# -----
order="account"

# Where
# -----
where="hard_coded_account_key = 'checking'"

echo "select $select from $from where $where order by $order;"		|
sql.e									|
string_initial_capital.e '|' 1						|
cat

exit 0
