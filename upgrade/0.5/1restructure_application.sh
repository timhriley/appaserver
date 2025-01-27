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

application_table="${application}_application"

attribute="only_one_primary_yn"
echo "alter table $application_table drop $attribute;" | sql.e 2>/dev/null
echo "delete from folder_attribute where attribute = '$attribute';" | sql.e
echo "delete from attribute where attribute = '$attribute';" | sql.e

attribute="appaserver_version"
echo "alter table $application_table drop $attribute;" | sql.e 2>/dev/null
echo "delete from folder_attribute where folder = 'application' and attribute = '$attribute';" | sql.e

attribute="database_date_format"
echo "alter table $application_table drop $attribute;" | sql.e 2>/dev/null
echo "delete from folder_attribute where attribute = '$attribute';" | sql.e
echo "delete from attribute where attribute = '$attribute';" | sql.e
echo "delete from relation where folder = 'application' and related_folder = 'date_formats' and related_attribute = 'database_date_format';" | sql.e

attribute="distill_directory"
echo "alter table $application_table drop $attribute;" | sql.e 2>/dev/null
echo "delete from folder_attribute where attribute = '$attribute';" | sql.e
echo "delete from attribute where attribute = '$attribute';" | sql.e

attribute="ghost_script_directory"
echo "alter table $application_table drop $attribute;" | sql.e 2>/dev/null
echo "delete from folder_attribute where attribute = '$attribute';" | sql.e
echo "delete from attribute where attribute = '$attribute';" | sql.e

attribute="grace_free_option_yn"
echo "alter table $application_table drop $attribute;" | sql.e 2>/dev/null
echo "delete from folder_attribute where attribute = '$attribute';" | sql.e
echo "delete from attribute where attribute = '$attribute';" | sql.e

attribute="grace_output"
echo "alter table $application_table drop $attribute;" | sql.e 2>/dev/null
echo "delete from folder_attribute where attribute = '$attribute';" | sql.e
echo "delete from attribute where attribute = '$attribute';" | sql.e
echo "delete from relation where folder = 'application' and related_folder = 'grace_output';" | sql.e

attribute="prepend_http_protocol_yn"
echo "alter table $application_table drop $attribute;" | sql.e 2>/dev/null
echo "delete from folder_attribute where attribute = '$attribute';" | sql.e
echo "delete from attribute where attribute = '$attribute';" | sql.e

attribute="chart_email_command_line"
echo "alter table $application_table drop $attribute;" | sql.e 2>/dev/null
echo "delete from folder_attribute where attribute = '$attribute';" | sql.e
echo "delete from attribute where attribute = '$attribute';" | sql.e

old_attribute="frameset_menu_horizontal_yn"
new_attribute="menu_horizontal_yn"
echo "alter table $application_table change $old_attribute $new_attribute char(1);" | sql.e
echo "update folder_attribute set attribute = '$new_attribute' where attribute = '$old_attribute';" | sql.e
echo "update attribute set attribute = '$new_attribute' where attribute = '$old_attribute';" | sql.e

exit 0
