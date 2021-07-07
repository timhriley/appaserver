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

folder="reoccurring_transaction"
attribute="bank_upload_feeder_phrase"

echo "alter table $folder drop $attribute;" |sql.e

echo "delete from folder_attribute where folder = '$folder' and attribute = '$attribute';" | sql.e

echo "delete from attribute where attribute = '$attribute';" | sql.e

exit 0
