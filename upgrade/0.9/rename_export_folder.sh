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

sql.e << all_done
update process set process = 'export_table', command_line = 'export_table \$process folder export_output' where process = 'export_folder';
update process_parameter set process = 'export_table' where process = 'export_folder';
update role_process set process = 'export_table' where process = 'export_folder';
all_done

exit 0
