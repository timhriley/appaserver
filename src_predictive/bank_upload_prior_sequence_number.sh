#!/bin/bash
# --------------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/bank_upload_prior_sequence_number.sh
# --------------------------------------------------------------------

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

echo "$0 $*" 1>&2

if [ "$#" -eq 0 ]
then
	echo "Usage: $0 bank_date" 1>&2
	exit 1
fi

# May have time appended
# ----------------------
bank_date=`echo $1 | column.e 0`

# All selecting from bank_upload_transaction_balance
# --------------------------------------------------
from=bank_upload_transaction_balance

# Get the minimum transaction_date for this bank_date
# ---------------------------------------------------
select="min( transaction_date_time )"

where="bank_date = '$bank_date'"

min_transaction_date_time=`echo "select $select from $from where $where;" | sql.e`

# Get the maximum sequence number before the minimum transaction_date_time.
# -------------------------------------------------------------------------
select="max( sequence_number )"
where="transaction_date_time < '$min_transaction_date_time'"

max_sequence_number=`echo "select $select from $from where $where;" | sql.e`

if [ "$max_sequence_number" = "" ]
then
	max_sequence_number=1
fi

# ------------------------------------------------------------------------
# Get the transaction_date_time for this sequence_number.
# ------------------------------------------------------------------------

# Shouldn't need to use min() anymore.
# ------------------------------------
select="min( transaction_date_time )"
where="sequence_number = $max_sequence_number"

transaction_date_time=`echo "select $select from $from where $where;" | sql.e`

echo "$max_sequence_number^$transaction_date_time"

exit 0
