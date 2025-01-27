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
   echo "Error in $0: you must first:" 1>&2
   echo "$ . set_database" 1>&2
   exit 1
fi

sql.e << all_done
update folder set folder = 'upgrade_script' where folder = 'upgrade_scripts';
update folder_attribute set folder = 'upgrade_script' where folder = 'upgrade_scripts';
update role_folder set folder = 'upgrade_script' where folder = 'upgrade_scripts';
update role_operation set folder = 'upgrade_script' where folder = 'upgrade_scripts';

alter table upgrade_scripts rename as upgrade_script;

delete from upgrade_script;

alter table upgrade_script drop index upgrade_scripts;
create unique index upgrade_script on upgrade_script ( appaserver_version, upgrade_script );
all_done

exit 0

