#!/bin/sh
#extract feeder_row

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

echo "delete from feeder_row where feeder_account = 'bank_of_america_credit_card' and feeder_load_date_time = '2022-09-04 18:32' and feeder_row_number = 2 and feeder_date = '2022-05-03';" | sql

echo "delete from feeder_row where feeder_account = 'bank_of_america_credit_card' and feeder_load_date_time = '2022-09-04 18:32' and feeder_row_number = 3 and feeder_date = '2022-04-18';" | sql

exit 0

