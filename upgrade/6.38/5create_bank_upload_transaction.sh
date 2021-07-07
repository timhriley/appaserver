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

folder="bank_upload_transaction"

echo "update folder set folder = '$folder' where folder = 'bank_upload_transaction_balance';" | sql.e

echo "update folder_attribute set folder = '$folder' where folder = 'bank_upload_transaction_balance';" | sql.e

echo "update relation set folder = '$folder' where folder = 'bank_upload_transaction_balance';" | sql.e

echo "update role_operation set folder = '$folder' where folder = 'bank_upload_transaction_balance';" | sql.e

echo "update role_folder set folder = '$folder' where folder = 'bank_upload_transaction_balance';" | sql.e

echo "insert into folder_attribute (folder,attribute,display_order) values ('$folder','feeder_account',11);" | sql.e

echo "insert into relation (folder,related_folder,related_attribute) values ('$folder','feeder_account','null');" | sql.e

echo "update folder set create_view_statement = 'create view bank_upload_transaction as select transaction.transaction_date_time, transaction.bank_date, transaction.bank_description, transaction.full_name, transaction.street_address, journal_ledger.account feeder_account, transaction.transaction_amount, bank_upload.bank_amount, journal_ledger.balance cash_running_balance, bank_upload.bank_running_balance, bank_upload.sequence_number from transaction, bank_upload, journal_ledger, feeder_account where transaction.bank_date = bank_upload.bank_date and transaction.bank_description = bank_upload.bank_description and transaction.full_name = journal_ledger.full_name and transaction.street_address = journal_ledger.street_address and transaction.transaction_date_time = journal_ledger.transaction_date_time and journal_ledger.account = feeder_account.feeder_account;' where folder = '$folder';" | sql.e

statement=`select.sh create_view_statement folder "folder = '$folder'"`

echo "$statement" | sql.e

echo "update folder_attribute set primary_key_index = null, display_order = 1 where folder = '$folder' and attribute = 'transaction_date_time';" | sql

echo "update folder_attribute set primary_key_index = null, display_order = 2 where folder = '$folder' and attribute = 'bank_date';" | sql

echo "update folder_attribute set primary_key_index = null, display_order = 3 where folder = '$folder' and attribute = 'bank_description';" | sql

echo "update folder_attribute set primary_key_index = null, display_order = 4 where folder = '$folder' and attribute = 'full_name';" | sql

echo "update folder_attribute set primary_key_index = null, display_order = 5 where folder = '$folder' and attribute = 'street_address';" | sql

echo "update folder_attribute set display_order = 6 where folder = '$folder' and attribute = 'transaction_amount';" | sql

echo "update folder_attribute set display_order = 7 where folder = '$folder' and attribute = 'bank_amount';" | sql

echo "update folder_attribute set display_order = 8 where folder = '$folder' and attribute = 'cash_running_balance';" | sql

echo "update folder_attribute set display_order = 9 where folder = '$folder' and attribute = 'bank_running_balance';" | sql

echo "update folder_attribute set display_order = 10 where folder = '$folder' and attribute = 'sequence_number';" | sql

exit 0
