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

echo "alter table entity drop website_login;"	|tee_appaserver.sh			|sql.e 2>&1 				 
echo "delete from table_column where table_name='entity' and column_name='website_login';" | sql.e 2>&1
echo "delete from foreign_column where table_name='entity' and related_column='website_login';" | sql.e 2>&1
echo "delete from foreign_column where table_name='entity' and foreign_column='website_login';" | sql.e 2>&1
echo "delete from foreign_column where related_table='entity' and related_column='website_login';" | sql.e 2>&1
echo "delete from foreign_column where related_table='entity' and foreign_column='website_login';" | sql.e 2>&1
echo "delete from row_security_role_update where table_name='entity' and column_not_null='website_login';" | sql.e 2>&1
exit 0

