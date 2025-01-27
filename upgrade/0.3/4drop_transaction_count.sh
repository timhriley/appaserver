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

# Returns 0 if FEEDER_PHRASE.feeder_phrase exists
# -----------------------------------------------
folder_attribute_exists.sh $application feeder_phrase feeder_phrase

if [ $? -ne 0 ]
then
	exit 0
fi

folder="journal_ledger"

attribute="transaction_count"
echo "alter table $folder drop $attribute;" | sql.e 2>&1 | grep -v '^ERROR'
echo "delete from folder_attribute where folder = '$folder' and attribute = '$attribute';" | sql.e
echo "delete from attribute where attribute = '$attribute';" | sql.e

exit 0
