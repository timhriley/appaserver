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

command_line="rename_column \$process old_attribute folder attribute execute_yn"

echo "update process set command_line = '$command_line' where process = 'rename_column';" | sql.e

exit 0

