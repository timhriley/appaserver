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

# javascript_folders
# ------------------
echo "alter table folder add javascript_filename char (80);" | sql.e 2>&1 | grep -vi duplicate

echo "select folder, javascript_filename from javascript_folders;"	|
sql '|'									|
sed 's/|/|javascript_filename|/'					|
tr '|' '^'								|
update_statement.e table=folder key=folder carrot=y			|
sql

echo "insert into folder_attribute ( folder, attribute, display_order ) values ( 'folder', 'javascript_filename', 16 );" | sql 2>&1 | grep -vi duplicate

echo "delete from folder_attribute where folder = 'javascript_folders';" | sql
echo "delete from folder where folder = 'javascript_folders';" | sql
echo "delete from role_folder where folder = 'javascript_folders';" | sql
echo "delete from role_operation where folder = 'javascript_folders';" | sql
echo "delete from relation where folder = 'javascript_folders';" | sql
echo "drop table javascript_folders;" | sql 2>&1 | grep -vi 'unknown table'

# javascript_processes
# --------------------
echo "alter table process drop last_change;" | sql 2>&1 | grep -v 'check that'
echo "alter table process add javascript_filename char (80);" | sql.e 2>&1 | grep -vi duplicate

echo "select process, javascript_filename from javascript_processes;"	|
sql '|'									|
sed 's/|/|javascript_filename|/'					|
tr '|' '^'								|
update_statement.e table=process key=process carrot=y			|
sql

echo "insert into folder_attribute ( folder, attribute, display_order ) values ( 'process', 'javascript_filename', 9 );" | sql 2>&1 | grep -vi duplicate

echo "delete from folder_attribute where folder = 'javascript_processes';" | sql
echo "delete from folder where folder = 'javascript_processes';" | sql
echo "delete from role_folder where folder = 'javascript_processes';" | sql
echo "delete from role_operation where folder = 'javascript_processes';" | sql
echo "delete from relation where folder = 'javascript_processes';" | sql
echo "drop table javascript_processes;" | sql 2>&1 | grep -vi 'unknown table'

# javascript_process_sets
# -----------------------
echo "alter table process_set drop last_change;" | sql 2>&1 | grep -v 'check that'
echo "alter table process_set add javascript_filename char (80);" | sql.e 2>&1 | grep -vi duplicate

echo "select process_set, javascript_filename from javascript_process_sets;" |
sql '|'									     |
sed 's/|/|javascript_filename|/'					     |
tr '|' '^'								     |
update_statement.e table=process_set key=process_set carrot=y		     |
sql

echo "insert into folder_attribute ( folder, attribute, display_order ) values ( 'process_set', 'javascript_filename', 8 );" | sql 2>&1 | grep -vi duplicate

echo "delete from folder_attribute where folder = 'javascript_process_sets';" | sql
echo "delete from folder where folder = 'javascript_process_sets';" | sql
echo "delete from role_folder where folder = 'javascript_process_sets';" | sql
echo "delete from role_operation where folder = 'javascript_process_sets';" | sql
echo "delete from relation where folder = 'javascript_process_sets';" | sql
echo "drop table javascript_process_sets;" | sql 2>&1 | grep -vi 'unknown table'

# Subschema
# ---------
echo "delete from subschemas where subschema = 'javascript';" | sql

exit 0
