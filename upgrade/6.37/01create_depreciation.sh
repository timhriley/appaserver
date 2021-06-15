#!/bin/sh
#create depreciation

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

table_name=depreciation
echo "create table $table_name (asset_name char (30) not null,serial_label char (40) not null,depreciation_date date not null,units_produced integer,depreciation_amount double (10,2),full_name char (60),street_address char (40),transaction_date_time datetime) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (asset_name,serial_label,depreciation_date);" | sql.e


