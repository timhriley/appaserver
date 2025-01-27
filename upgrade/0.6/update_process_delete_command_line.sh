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

command_line="delete_folder_row \$session \$login_name \$role \$folder \$primary_data_list n"

echo "update process set command_line = '$command_line' where process = 'delete';" | sql.e

command_line="delete_folder_row \$session \$login_name \$role \$folder \$primary_data_list y"

echo "update process set command_line = '$command_line' where process = 'delete_isa_only';" | sql.e

exit 0

