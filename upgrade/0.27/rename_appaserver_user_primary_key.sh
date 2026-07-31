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

service_work()
{
	echo "delete from foreign_column where table_name in ('hourly_service_work','fixed_service_work' ) and foreign_column = 'appaserver_full_name';" | sql.e
	echo "delete from foreign_column where table_name in ('hourly_service_work','fixed_service_work' ) and foreign_column = 'appaserver_contact_key';" | sql.e
}

login_default_role()
{
	echo "alter table login_default_role change full_name appaserver_full_name char (60);" | sql.e
	echo "update table_column set column_name = 'appaserver_full_name' where table_name = 'login_default_role' and column_name = 'full_name';" | sql.e

	if [ "$application" = "appahost" ]
	then
		echo "alter table login_default_role change contact_key appaserver_contact_key char (60);" | sql.e
		echo "update table_column set column_name = 'appaserver_contact_key' where table_name = 'login_default_role' and column_name = 'contact_key';" | sql.e
	fi
	
	echo "alter table login_default_role drop index login_default_role_unique;" | sql.e
	
	if [ "$application" = "appahost" ]
	then
		echo "create unique index login_default_role_unique on login_default_role (appaserver_full_name, appaserver_contact_key);" | sql.e
	else
		echo "create unique index login_default_role_unique on login_default_role (appaserver_full_name);" | sql.e
	fi
}

role_appaserver_user()
{
	echo "alter table role_appaserver_user change full_name appaserver_full_name char (60);" | sql.e
	echo "update table_column set column_name = 'appaserver_full_name' where table_name = 'role_appaserver_user' and column_name = 'full_name';" | sql.e

	if [ "$application" = "appahost" ]
	then
		echo "alter table role_appaserver_user change contact_key appaserver_contact_key char (60);" | sql.e
		echo "update table_column set column_name = 'appaserver_contact_key' where table_name = 'role_appaserver_user' and column_name = 'contact_key';" | sql.e
	fi
	
	echo "alter table role_appaserver_user drop index role_appaserver_user_unique;" | sql.e
	
	if [ "$application" = "appahost" ]
	then
		echo "create unique index role_appaserver_user_unique on role_appaserver_user (appaserver_full_name, appaserver_contact_key, role);" | sql.e
	else
		echo "create unique index role_appaserver_user_unique on role_appaserver_user (appaserver_full_name,role);" | sql.e
	fi
}

appaserver_user_entity()
{
	echo "alter table appaserver_user change full_name appaserver_full_name char (60);" | sql.e
	echo "update table_column set column_name = 'appaserver_full_name' where table_name = 'appaserver_user' and column_name = 'full_name';" | sql.e
	echo "insert into foreign_column ( table_name, related_table, related_column, foreign_column, foreign_key_index ) values ( 'appaserver_user', 'entity', 'null', 'appaserver_full_name', 1 );" | sql.e
	
	if [ "$application" = "appahost" ]
	then
		echo "alter table appaserver_user change contact_key appaserver_contact_key char (60);" | sql.e
		echo "update table_column set column_name = 'appaserver_contact_key' where table_name = 'appaserver_user' and column_name = 'contact_key';" | sql.e
		echo "insert into foreign_column ( table_name, related_table, related_column, foreign_column, foreign_key_index ) values ( 'appaserver_user', 'entity', 'null', 'appaserver_contact_key', 2 );" | sql.e
	fi
	
	echo "alter table appaserver_user drop index appaserver_user_unique;" | sql.e
	
	if [ "$application" = "appahost" ]
	then
		echo "create unique index appaserver_user_unique on appaserver_user (appaserver_full_name, appaserver_contact_key);" | sql.e
	else
		echo "create unique index appaserver_user_unique on appaserver_user (appaserver_full_name);" | sql.e
	fi
}

feeder_load_event()
{
	echo "alter table feeder_load_event change full_name appaserver_full_name char (60);" | sql.e
	echo "update table_column set column_name = 'appaserver_full_name' where table_name = 'feeder_load_event' and column_name = 'full_name';" | sql.e
	
	if [ "$application" = "appahost" ]
	then
		echo "alter table feeder_load_event change contact_key appaserver_contact_key char (60);" | sql.e
		echo "update table_column set column_name = 'appaserver_contact_key' where table_name = 'feeder_load_event' and column_name = 'contact_key';" | sql.e
	fi
}

session_appaserver_user()
{
	echo "alter table session change full_name appaserver_full_name char (60);" | sql.e
	echo "update table_column set column_name = 'appaserver_full_name' where table_name = 'session' and column_name = 'full_name';" | sql.e
	
	if [ "$application" = "appahost" ]
	then
		echo "alter table session change contact_key appaserver_contact_key char (60);" | sql.e
		echo "update table_column set column_name = 'appaserver_contact_key' where table_name = 'session' and column_name = 'contact_key';" | sql.e
	fi
}

appaserver_user_entity
session_appaserver_user
role_appaserver_user
login_default_role

# Returns 0 if FEEDER_LOAD_EVENT.full_name exists
# -----------------------------------------------
table_column_exists.sh feeder_load_event full_name

if [ $? -ne 0 ]
then
	exit 0
fi

feeder_load_event
service_work

exit 0

