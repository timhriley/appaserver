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

echo "drop table if exists process_generic_value;" | sql.e;echo "create table process_generic_value (value_folder char (50) not null,value_attribute char (60) not null,datatype_folder char (50),foreign_folder char (50),datatype_attribute char (60),date_attribute char (60),time_attribute char (60),datatype_aggregation_sum_yn char (1),datatype_bar_graph_yn char (1),datatype_scale_graph_zero_yn char (1),datatype_units_yn char (1),foreign_units_yn char (1)) engine MyISAM;" |sql.e 2>&1 |tee_appaserver.sh;echo "create unique index process_generic_value on process_generic_value (value_folder,value_attribute);" |sql.e 2>&1
exit 0

