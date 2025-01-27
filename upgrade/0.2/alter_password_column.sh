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

echo "alter table appaserver_user modify password char (56);" | sql.e
echo "delete from attribute where attribute = 'password';" | sql.e
echo "insert into attribute						     \
	(attribute,							     \
	hint_message,							     \
	appaserver_yn,							     \
	attribute_datatype,						     \
	width,								     \
	float_decimal_places,						     \
	post_change_javascript,						     \
	on_focus_javascript_function,					     \
	lookup_histogram_output_yn,					     \
	lookup_time_chart_output_yn)					     \
	values('password',null,'y','password',56,null,null,null,null,null);" |
sql.e

exit 0
