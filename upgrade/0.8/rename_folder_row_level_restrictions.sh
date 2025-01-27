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

sql.e << all_done
alter table folder_row_level_restrictions rename as folder_row_level_restriction;
update folder set folder = 'folder_row_level_restriction' where folder = 'folder_row_level_restrictions';
update folder_attribute set folder = 'folder_row_level_restriction' where folder = 'folder_row_level_restrictions';
update relation set folder = 'folder_row_level_restriction' where folder = 'folder_row_level_restrictions';
update role_folder set folder = 'folder_row_level_restriction' where folder = 'folder_row_level_restrictions';
update process_parameter set folder = 'folder_row_level_restriction' where folder = 'folder_row_level_restrictions';
update folder_operation set folder = 'folder_row_level_restriction' where folder = 'folder_row_level_restrictions';
alter table folder_row_level_restriction drop index folder_row_level_restrictions;
create unique index folder_row_level_restriction on folder_row_level_restriction ( folder );
all_done

exit 0
