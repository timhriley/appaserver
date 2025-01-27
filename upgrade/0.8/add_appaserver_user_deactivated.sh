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
alter table appaserver_user add column deactivated_yn char(1);
insert into attribute (attribute,attribute_datatype,width) values ('deactivated_yn','text',1);
insert into folder_attribute (folder,attribute,display_order) values ('appaserver_user','deactivated_yn',4);
all_done

exit 0
