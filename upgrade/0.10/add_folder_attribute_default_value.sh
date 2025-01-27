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
alter table folder_attribute add column default_value char(10);
insert into attribute (attribute,attribute_datatype,width) values ('default_value','text',10);
insert into folder_attribute (folder,attribute,display_order) values ('folder_attribute','default_value',10);
update folder_attribute set default_value = 'unknown' where folder = 'entity' and attribute = 'street_address';
update folder_attribute set default_value = 'null' where folder = 'process_parameter' and attribute = 'folder';
update folder_attribute set default_value = 'null' where folder = 'process_parameter' and attribute = 'attribute';
update folder_attribute set default_value = 'null' where folder = 'process_parameter' and attribute = 'drop_down_prompt';
update folder_attribute set default_value = 'null' where folder = 'process_parameter' and attribute = 'prompt';
update folder_attribute set default_value = 'null' where folder = 'process_set_parameter' and attribute = 'folder';
update folder_attribute set default_value = 'null' where folder = 'process_set_parameter' and attribute = 'attribute';
update folder_attribute set default_value = 'null' where folder = 'process_set_parameter' and attribute = 'drop_down_prompt';
update folder_attribute set default_value = 'null' where folder = 'process_set_parameter' and attribute = 'prompt';
update folder_attribute set default_value = 'null' where folder = 'relation' and attribute = 'related_attribute';
update folder_attribute set default_value = 'null' where folder = 'entity' and attribute = 'street_address';
all_done

exit 0
