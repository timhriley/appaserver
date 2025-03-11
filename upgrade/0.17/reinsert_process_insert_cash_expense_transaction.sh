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
update process set command_line = 'insert_expense_transaction.sh $process full_name street_address transaction_date account transaction_amount check_number memo program_name' where process = 'insert_cash_expense_transaction';
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0
