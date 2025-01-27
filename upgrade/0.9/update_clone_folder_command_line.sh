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

echo "update process set command_line = 'clone_folder \$process table_name column_name old_data new_data delete_yn execute_yn' where process = 'clone_folder';" | sql.e

echo "delete from process_parameter where process = 'clone_folder' and prompt = 'output2file_yn';" | sql

echo "update process set process = 'clone_table' where process = 'clone_folder';" | sql

echo "update process_parameter set process = 'clone_table' where process = 'clone_folder';" | sql

echo "update role_process set process = 'clone_table' where process = 'clone_folder';" | sql

echo "delete from prompt where prompt = 'output2file_yn';" | sql

exit 0
