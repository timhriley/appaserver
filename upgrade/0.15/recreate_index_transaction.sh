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
cat << shell_all_done
drop index transaction_additional_unique on transaction;
drop index transaction_check_number on transaction;
create unique index check_number_additional_unique on transaction (check_number);
create unique index transaction_date_time_additional_unique on transaction (transaction_date_time);
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0
