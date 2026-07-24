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

# Returns 0 if ELEMENT.element exists
# -----------------------------------
table_column_exists.sh element element

if [ $? -ne 0 ]
then
	exit 0
fi

(
echo "alter table transaction change transaction_lock_yn transaction_lock_y char (1);" | tee_appaserver.sh | sql.e 2>&1
echo "update appaserver_column set column_name = 'transaction_lock_y' where column_name = 'transaction_lock_yn';" | tee_appaserver.sh | sql.e 2>&1
echo "update table_column set column_name = 'transaction_lock_y' where column_name = 'transaction_lock_yn';" | tee_appaserver.sh | sql.e 2>&1
echo "update row_security_role_update set column_not_null = 'transaction_lock_y' where column_not_null = 'transaction_lock_yn';" | tee_appaserver.sh | sql.e 2>&1
) 2>&1 | grep -vi duplicate
exit 0

