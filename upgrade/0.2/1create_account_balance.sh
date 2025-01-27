#!/bin/sh
#create account_balance

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

table_name=account_balance
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,account_number char (50) not null,date date not null,balance double (11,2),balance_change double (11,2),balance_change_percent integer) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,account_number,date);" | sql.e


