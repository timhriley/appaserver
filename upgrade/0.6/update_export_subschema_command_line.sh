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

(
cat << shell_all_done
update process set command_line = 'export_subschema \$process table_name exclude_roles_yn', notepad = 'For a list of tables, this process exports from the following: table, relation, table_column, column, table_operation, role_table (maybe), row_security_role_update, table_row_level_restriction, and foreign_column.' where process = 'export_subschema';
insert into process_parameter ( process, folder, attribute, drop_down_prompt, prompt, display_order ) values ( 'export_subschema', 'null', 'null', 'null', 'exclude_roles_yn', 2 );
update process_parameter set display_order = 1 where process = 'export_subschema' and folder = 'folder';
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
