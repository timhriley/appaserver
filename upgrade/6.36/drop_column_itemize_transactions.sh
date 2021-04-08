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

# Returns 0 if BANK_UPLOAD.bank_date exists
# -----------------------------------------
folder_attribute_exists.sh $application bank_upload bank_date

if [ $? -ne 0 ]
then
	exit 1
fi

table_name=`get_table_name $application tax_form_line`
echo "alter table $table_name drop itemize_transactions_yn;" | sql.e
folder_attribute_table=`get_table_name $application folder_attribute`
echo "delete from $folder_attribute_table				\
	where attribute = 'itemize_transactions_yn';"			|
sql.e

attribute_table=`get_table_name $application attribute`
echo "delete from $attribute_table					\
	where attribute = 'itemize_transactions_yn';"			|
sql.e

exit 0
