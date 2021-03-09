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

echo "delete from folder_attribute where folder = 'process_generic_datatype_folder' and attribute = 'process_generic_unit';" | sql

echo "delete from folder_attribute where folder = 'process_generic_datatype_folder' and attribute = 'datatype_attribute';" | sql

echo "delete from relation where folder = 'process_generic_value_folder' and folder = 'process_generic_units';" | sql

echo "delete from relation where folder = 'process_generic_value_folder' and folder = 'attribute';" | sql

echo "delete from folder where folder = 'process_generic_units';" | sql

echo "delete from role_folder where folder = 'process_generic_units';" | sql

echo "drop table process_generic_units;" | sql

echo "alter table process_generic_datatype_folder drop last_change;" | sql 2>&1 | grep -vi 'check that'

echo "insert into attribute (attribute,attribute_datatype,width,appaserver_yn) values ('datatype_units_yn','text',1,'y');" | sql

echo "insert into attribute (attribute,attribute_datatype,width,appaserver_yn) values ('foreign_units_yn','text',1,'y');" | sql

echo "insert into folder_attribute (folder,attribute,display_order) values ('process_generic_datatype_folder','datatype_units_yn',4);" | sql

echo "insert into folder_attribute (folder,attribute,display_order) values ('process_generic_datatype_folder','foreign_units_yn',5);" | sql

echo "update folder_attribute set display_order = 1 where folder = 'process_generic_datatype_folder' and attribute = 'datatype_aggregation_sum_yn';" | sql

echo "update folder_attribute set display_order = 2 where folder = 'process_generic_datatype_folder' and attribute = 'datatype_bar_graph_yn';" | sql

echo "update folder_attribute set display_order = 3 where folder = 'process_generic_datatype_folder' and attribute = 'datatype_scale_graph_zero_yn';" | sql

echo "update folder_attribute set display_order = 1 where folder = 'process_generic_value_folder' and attribute = 'date_attribute';" | sql

echo "update folder_attribute set display_order = 2 where folder = 'process_generic_value_folder' and attribute = 'time_attribute';" | sql

echo "update folder_attribute set display_order = 3 where folder = 'process_generic_value_folder' and attribute = 'value_attribute';" | sql

echo "update folder_attribute set display_order = 4 where folder = 'process_generic_value_folder' and attribute = 'datatype_folder';" | sql

echo "update folder_attribute set display_order = 5 where folder = 'process_generic_value_folder' and attribute = 'foreign_folder';" | sql

echo "alter table process_generic_datatype_folder add datatype_units_yn char (1);" | sql

echo "alter table process_generic_datatype_folder add foreign_units_yn char (1);" | sql

echo "update process_generic_datatype_folder set datatype_units_yn = 'y';" | sql

echo "update process_generic_datatype_folder set foreign_units_yn = 'y' where datatype_folder = 'parameter_unit';" | sql

exit 0
