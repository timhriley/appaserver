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
alter table appaserver_user add full_name char( 60 );
alter table appaserver_user add street_address char( 60 );
alter table appaserver_user drop index appaserver_user;
alter table appaserver_user drop index appaserver_user_unique;
create unique index appaserver_user_unique on appaserver_user (full_name,street_address);
create unique index login_name_additional_unique on appaserver_user (login_name);

insert into table_column (table_name,column_name,primary_key_index) values ('appaserver_user','full_name',1);
insert into table_column (table_name,column_name,primary_key_index) values ('appaserver_user','street_address',2);
update table_column set primary_key_index = null, display_order = 1 where table_name = 'appaserver_user' and column_name = 'login_name';
update table_column set display_order = 2 where table_name = 'appaserver_user' and column_name = 'password';
update table_column set display_order = 3 where table_name = 'appaserver_user' and column_name = 'user_date_format';
update table_column set display_order = 4 where table_name = 'appaserver_user' and column_name = 'deactivated_yn';
update table_column set default_value = 'unknown' where table_name = 'appaserver_user' and column_name = 'street_address';
insert into relation (table_name,related_table,related_column,relation_type_isa_yn) values ('appaserver_user','entity','null','y');
update appaserver_table set subschema = 'entity' where table_name = 'appaserver_user';
update appaserver_user set full_name = (select full_name from self), street_address = (select street_address from self);
delete from table_column where table_name = 'appaserver_user' and column_name = 'person_full_name';
delete from appaserver_column where column_name = 'person_full_name';
alter table appaserver_user change login_name login_name char (50) null;
alter table appaserver_user change full_name full_name char (60) not null;
alter table appaserver_user change street_address street_address char (60) not null;
alter table appaserver_user drop column person_full_name;
show create table appaserver_user;
shell_all_done
) | sql.e

select.sh '*' appaserver_user

exit 0
