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

select="concat( feeder_account, '|', feeder_account, ' [', subclassification, ']' )"

# From
# ----
from="feeder_account,account"

# Order
# -----
order="feeder_account"

# Where
# -----
join="feeder_account.feeder_account = account.account"

where="subclassification = 'cash_or_equivalent' and $join"

echo "select $select from $from where $where order by $order;"		|
sql.e									|
string_initial_capital.e '|' 1						|
cat

exit 0
