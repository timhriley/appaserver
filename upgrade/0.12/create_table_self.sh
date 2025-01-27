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

# Returns 0 if self.full_name exists in MySQL
# -------------------------------------------
table_column_mysql_exists.sh self full_name

if [ $? -eq 0 ]
then
	exit 0
fi

(
cat << shell_all_done
create table self (full_name char (60) not null,street_address char (60) not null,credit_card_number char (20),credit_card_expiration_month_year char (4),credit_card_security_code char (3),credit_provider char (30),credit_provider_not_supported_yn char (1),invoice_amount_due double (10,2)) engine MyISAM;
create unique index self on self (full_name,street_address);
shell_all_done
) | sql.e
exit 0
