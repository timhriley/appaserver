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

where="attribute = 'work_hours'"

select.sh folder folder_attribute "$where"	|
while read table_name
do
	echo "alter table ${table_name} modify work_hours float(10,4);" | sql.e
done

echo "update attribute set width = 10, float_decimal_places = 4 where attribute = 'work_hours';" | sql.e

exit 0

