#!/bin/sh
#create feeder_row

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

table_name=feeder_row
echo "drop table if exists $table_name;" | sql.e
echo "create table $table_name (feeder_account char (30) not null,feeder_load_date_time char (16) not null,row_number integer not null,feeder_date date,full_name char (60),street_address char (60),transaction_date_time datetime,file_row_description char (140),file_row_amount double (12,2),file_row_balance double (12,2),calculate_balance double (12,2),check_number integer,feeder_phrase char (50)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (feeder_account,feeder_load_date_time,row_number);" | sql.e
echo "create unique index ${table_name}_additional_unique on $table_name (feeder_account,full_name,street_address,transaction_date_time);" | sql.e


