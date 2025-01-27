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

# Returns 0 if FEEDER_PHRASE.feeder_phrase exists
# -----------------------------------------------
folder_attribute_exists.sh $application feeder_phrase feeder_phrase

if [ $? -ne 0 ]
then
	exit 0
fi

where="attribute = 'street_address'"

select.sh folder folder_attribute "$where"	|
while read table_name
do
	echo "alter table ${table_name} modify street_address char(60);" | sql.e
done

echo "update attribute set width = 60 where attribute = 'street_address';" | sql.e

exit 0

