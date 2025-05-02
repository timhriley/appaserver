#!/bin/bash
# -----------------------------------------------------------
# $APPASERVER_HOME/src_predictive/populate_expense_account.sh
# -----------------------------------------------------------

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

populate_account.sh many_table subclassification expense
populate_account.sh many_table subclassification revenue

exit 0

