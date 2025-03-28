#!/bin/sh
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

echo "delete from appaserver_column where column_name = 'trigger_insert_update_yn';" | sql
echo "delete from table_column where column_name = 'trigger_insert_update_yn';" | sql

echo "update relation set omit_drillthru_yn = 'y' where table_name = 'prior_fixed_asset' and related_table = 'transaction';" | sql


# Returns 0 if RELATION.trigger_insert_update_yn exists
# -----------------------------------------------------
table_column_exists.sh relation trigger_insert_update_yn

if [ $? -eq 0 ]
then
	echo "alter table relation drop column trigger_insert_update_yn;" | sql
fi


exit 0
