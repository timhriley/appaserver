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

# Returns 0 if SELF.credit_card_number exists
# -------------------------------------------
table_column_exists.sh self credit_card_number

if [ $? -ne 0 ]
then
	echo "alter table self add credit_card_number char (20);" | tee_appaserver.sh | sql.e 2>&1
fi

# Returns 0 if SELF.credit_card_expiration_month_year exists
# ----------------------------------------------------------
table_column_exists.sh self credit_card_expiration_month_year

if [ $? -ne 0 ]
then
	echo "alter table self add credit_card_expiration_month_year char (4);" | tee_appaserver.sh | sql.e 2>&1
fi

# Returns 0 if SELF.credit_card_security_code exists
# --------------------------------------------------
table_column_exists.sh self credit_card_security_code

if [ $? -ne 0 ]
then
	echo "alter table self add credit_card_security_code char (3);" | tee_appaserver.sh | sql.e 2>&1
fi

# Returns 0 if SELF.credit_provider exists
# ----------------------------------------
table_column_exists.sh self credit_provider

if [ $? -ne 0 ]
then
	echo "alter table self add credit_provider char (30);" | tee_appaserver.sh | sql.e 2>&1
fi

# Returns 0 if SELF.invoice_amount_due exists
# -------------------------------------------
table_column_exists.sh self invoice_amount_due

if [ $? -ne 0 ]
then
	echo "alter table self add invoice_amount_due double (10,2);" | tee_appaserver.sh | sql.e 2>&1
fi

# Returns 0 if SELF.invoice_statement_current exists
# --------------------------------------------------
table_column_exists.sh self invoice_statement_current

if [ $? -ne 0 ]
then
	echo "alter table self add invoice_statement_current char (50);" | tee_appaserver.sh | sql.e 2>&1
fi

exit 0

