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

folder="transaction"

echo "insert into folder_attribute (folder,attribute,display_order) values ('$folder','bank_date',2);" | sql.e

echo "insert into folder_attribute (folder,attribute,display_order) values ('$folder','bank_description',3);" | sql.e

echo "alter table $folder add bank_date date;" | sql.e
echo "alter table $folder add bank_description char(140);" | sql.e

echo "insert into relation (folder,related_folder,related_attribute) values ('$folder','bank_upload','null');" | sql.e

exit 0
