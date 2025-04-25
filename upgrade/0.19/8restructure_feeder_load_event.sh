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

# Returns 0 if ELEMENT.element exists
# -----------------------------------
table_column_exists.sh element element

if [ $? -ne 0 ]
then
	exit 0
fi

(
cat << shell_all_done
alter table feeder_load_event add full_name char( 60 );
alter table feeder_load_event add street_address char( 60 );
alter table feeder_load_event drop login_name;
alter table feeder_load_event change feeder_load_date_time feeder_load_date_time char(19) not null;

insert into table_column (table_name,column_name,display_order) values ('feeder_load_event','full_name',1);
insert into table_column (table_name,column_name,display_order) values ('feeder_load_event','street_address',2);
delete from table_column where table_name = 'feeder_load_event' and column_name = 'login_name';

update table_column set display_order = 3 where table_name = 'feeder_load_event' and column_name = 'feeder_load_filename';
update table_column set display_order = 4 where table_name = 'feeder_load_event' and column_name = 'account_end_date';
update table_column set display_order = 5 where table_name = 'feeder_load_event' and column_name = 'account_end_balance';

update feeder_load_event set full_name = (select full_name from self), street_address = (select street_address from self);

show create table feeder_load_event;
shell_all_done
) | sql.e

exit 0
