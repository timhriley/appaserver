#!/bin/sh
#create feeder_phrase

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

table_name=feeder_phrase
echo "create table $table_name (feeder_phrase char (50) not null,nominal_account char (60),full_name char (60),street_address char (60)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (feeder_phrase);" | sql.e

exit 0
