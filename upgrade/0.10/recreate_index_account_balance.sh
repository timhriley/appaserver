#!/bin/bash
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
	echo "$ . set_database" 1>&2
	exit 1
fi

# Returns 0 if ACCOUNT_BALANCE.full_name exists
# ---------------------------------------------
folder_attribute_exists.sh	$application		\
				account_balance		\
				full_name

if [ $? -ne 0 ]
then
	exit 0
fi

table_name=account_balance
index_name=account_balance

echo "alter table ${table_name} drop index ${index_name};" | sql.e
echo "create unique index ${index_name} on ${table_name} (full_name,street_address,account_number,date);" | sql.e

exit 0

