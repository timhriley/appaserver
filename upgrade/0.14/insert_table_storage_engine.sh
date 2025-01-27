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
insert into storage_engine (storage_engine) values ('MyISAM');
insert into storage_engine (storage_engine) values ('InnoDB');
insert into storage_engine (storage_engine) values ('NDB');
insert into storage_engine (storage_engine) values ('federated');
insert into storage_engine (storage_engine) values ('memory');
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0
