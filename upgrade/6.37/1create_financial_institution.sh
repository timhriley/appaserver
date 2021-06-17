#!/bin/sh
#create financial_institution

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

table_name=financial_institution
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,website_address char (50),login_name char (50),entity_password char (20)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address);" | sql.e


