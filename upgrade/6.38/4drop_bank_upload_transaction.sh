#!/bin/sh
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
	exit 0
fi

folder="bank_upload_transaction"

echo "drop table $folder;" | sql.e 2>&1 | grep -vi unknown
echo "delete from folder where folder = '$folder';" | sql.e
echo "delete from relation where folder = '$folder';" | sql.e
echo "delete from relation where related_folder = '$folder';" | sql.e
echo "delete from folder_attribute where folder = '$folder';" | sql.e
echo "delete from role_operation where folder = '$folder';" | sql.e
echo "delete from role_folder where folder = '$folder';" | sql.e

folder="bank_upload_feeder_archive"

echo "drop view $folder;" | sql.e 2>&1 | grep -vi unknown
echo "delete from folder where folder = '$folder';" | sql.e
echo "delete from relation where folder = '$folder';" | sql.e
echo "delete from relation where related_folder = '$folder';" | sql.e
echo "delete from folder_attribute where folder = '$folder';" | sql.e
echo "delete from role_operation where folder = '$folder';" | sql.e
echo "delete from role_folder where folder = '$folder';" | sql.e

folder="bank_upload_archive"

echo "drop table $folder;" | sql.e
echo "delete from folder where folder = '$folder';" | sql.e
echo "delete from relation where folder = '$folder';" | sql.e
echo "delete from relation where related_folder = '$folder';" | sql.e
echo "delete from folder_attribute where folder = '$folder';" | sql.e
echo "delete from role_operation where folder = '$folder';" | sql.e
echo "delete from role_folder where folder = '$folder';" | sql.e

folder="bank_upload_event"

echo "drop table $folder;" | sql.e
echo "delete from folder where folder = '$folder';" | sql.e
echo "delete from relation where folder = '$folder';" | sql.e
echo "delete from relation where related_folder = '$folder';" | sql.e
echo "delete from folder_attribute where folder = '$folder';" | sql.e
echo "delete from role_operation where folder = '$folder';" | sql.e
echo "delete from role_folder where folder = '$folder';" | sql.e

folder="bank_upload"

echo "drop table $folder;" | sql.e
echo "delete from folder where folder = '$folder';" | sql.e
echo "delete from relation where folder = '$folder';" | sql.e
echo "delete from relation where related_folder = '$folder';" | sql.e
echo "delete from folder_attribute where folder = '$folder';" | sql.e
echo "delete from role_operation where folder = '$folder';" | sql.e
echo "delete from role_folder where folder = '$folder';" | sql.e

folder="bank_upload_transaction_balance"

echo "drop view $folder;" | sql.e 2>&1 | grep -vi unknown
echo "delete from folder where folder = '$folder';" | sql.e
echo "delete from relation where folder = '$folder';" | sql.e
echo "delete from relation where related_folder = '$folder';" | sql.e
echo "delete from folder_attribute where folder = '$folder';" | sql.e
echo "delete from role_operation where folder = '$folder';" | sql.e
echo "delete from role_folder where folder = '$folder';" | sql.e

exit 0
