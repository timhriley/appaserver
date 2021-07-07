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
	exit 1
fi

echo "insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_archive','feeder_account',null,'1',null,null,null,'y',null,null,null);" | sql.e

echo "update folder_attribute set display_order = 2 where folder = 'bank_upload_archive' and attribute = 'sequence_number';" | sql.e

echo "update folder_attribute set display_order = 3 where folder = 'bank_upload_archive' and attribute = 'bank_amount';" | sql.e

echo "update folder_attribute set display_order = 4 where folder = 'bank_upload_archive' and attribute = 'bank_running_balance';" | sql.e

echo "update folder_attribute set display_order = 5 where folder = 'bank_upload_archive' and attribute = 'bank_upload_date_time';" | sql.e

echo "insert into relation (folder,related_folder,related_attribute) values ('bank_upload_archive','feeder_account','null');" | sql.e

echo "alter table bank_upload_archive add feeder_account char (40);" | sql.e

echo "create index bank_upload_archive_feeder_account on bank_upload_archive (feeder_account);" | sql.e

feeder_account=`select.sh account account "hard_coded_account_key = 'cash_key'"`

echo "update bank_upload_archive set feeder_account = '$feeder_account';" | sql.e

exit 0
