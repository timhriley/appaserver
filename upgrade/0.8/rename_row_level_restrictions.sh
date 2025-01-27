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
alter table row_level_restrictions rename as row_level_restriction;
update folder set folder = 'row_level_restriction' where folder = 'row_level_restrictions';
update folder_attribute set folder = 'row_level_restriction' where folder = 'row_level_restrictions';
update relation set related_folder = 'row_level_restriction' where related_folder = 'row_level_restrictions';
update role_folder set folder = 'row_level_restriction' where folder = 'row_level_restrictions';
update folder_operation set folder = 'row_level_restriction' where folder = 'row_level_restrictions';
alter table row_level_restriction drop index row_level_restrictions;
create unique index row_level_restriction on row_level_restriction ( row_level_restriction );
all_done

exit 0
