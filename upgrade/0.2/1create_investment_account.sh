#!/bin/sh
#create investment_account

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

table_name=investment_account
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,account_number char (50) not null,balance_latest double (14,2),investment_classification char (15),investment_purpose char (30),certificate_maturity_months integer,certificate_maturity_date date,interest_rate double (5,2),cost_basis double (12,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,account_number);" | sql.e


