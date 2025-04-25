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
alter table vendor add notepad text;
insert into table_column (table_name,column_name,display_order) values ('vendor','notepad',4);
update vendor set notepad = ( select notepad from entity where entity.full_name = vendor.full_name and entity.street_address = vendor.street_address and entity.notepad is not null );
show create table vendor;
shell_all_done
) | sql.e

select.sh full_name,street_address,notepad vendor "notepad is not null"

exit 0
