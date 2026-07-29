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

# Returns 0 if INVESTMENT_ACCOUNT.full_name exists
# ------------------------------------------------
table_column_exists.sh investment_account full_name

if [ $? -ne 0 ]
then
	exit 0
fi

echo "alter table investment_classification change investment_classification investment_medium char(15)" | tee_appaserver.sh | sql.e 2>&1
echo "alter table investment_classification rename as investment_medium" | tee_appaserver.sh | sql.e 2>&1
echo "alter table investment_account change investment_classification investment_medium char(15)" | tee_appaserver.sh | sql.e 2>&1
echo "alter table investment_medium drop index investment_classification;" | tee_appaserver.sh | sql.e 2>&1
echo "create unique index investment_medium_unique on investment_medium (investment_medium);" | tee_appaserver.sh | sql.e 2>&1
echo "update appaserver_table set table_name='investment_medium' where table_name='investment_classification';" | tee_appaserver.sh | sql.e 2>&1
echo "update role_table set table_name='investment_medium' where table_name='investment_classification';" | tee_appaserver.sh | sql.e 2>&1
echo "update relation set table_name='investment_medium' where table_name='investment_classification';" | tee_appaserver.sh | sql.e 2>&1
echo "update relation set related_table='investment_medium' where related_table='investment_classification';" | tee_appaserver.sh | sql.e 2>&1
echo "update table_column set table_name='investment_medium' where table_name='investment_classification';" | tee_appaserver.sh | sql.e 2>&1
echo "update foreign_column set table_name='investment_medium' where table_name='investment_classification';" | tee_appaserver.sh | sql.e 2>&1
echo "update table_row_level_restriction set table_name='investment_medium' where table_name='investment_classification';" | tee_appaserver.sh | sql.e 2>&1
echo "update process_parameter set table_name='investment_medium' where table_name='investment_classification';" | tee_appaserver.sh | sql.e 2>&1
echo "update table_operation set table_name='investment_medium' where table_name='investment_classification';" | tee_appaserver.sh | sql.e 2>&1
echo "update appaserver_column set column_name='investment_medium' where column_name='investment_classification';" | tee_appaserver.sh | sql.e 2>&1
echo "update table_column set column_name='investment_medium' where column_name='investment_classification';" | tee_appaserver.sh | sql.e 2>&1
exit 0

