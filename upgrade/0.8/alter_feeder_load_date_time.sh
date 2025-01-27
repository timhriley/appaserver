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

attribute=feeder_load_date_time
echo "alter table feeder_row modify $attribute char( 19 );" | sql.e
echo "alter table feeder_load_event modify $attribute char( 19 );" | sql.e
echo "update attribute set width = 19, attribute_datatype = 'date_time'  where attribute = '$attribute';" | sql.e

exit 0
