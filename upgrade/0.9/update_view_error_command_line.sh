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

echo "update process set command_line = 'view_appaserver_error_file.sh \$process line_count', process_group = 'view'  where process = 'view_appaserver_log_file';" | sql.e

echo "insert into process_group ( process_group ) values ( 'view' );" | sql.e 2>&1 | grep -vi duplicate

exit 0
