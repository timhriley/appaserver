#!/bin/sh
#create cost_recovery

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

table_name=cost_recovery
echo "create table $table_name (asset_name char (30) not null,serial_label char (40) not null,tax_year integer not null,recovery_amount double (12,2),recovery_rate double (7,5)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (asset_name,serial_label,tax_year);" | sql.e


