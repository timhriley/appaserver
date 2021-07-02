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

(
table_name=`get_table_name $application appaserver_user`
echo "alter table $table_name modify password char (56);" | sql.e
attribute_table=`get_table_name $application attribute`
echo "delete from $attribute_table where attribute = 'password';" | sql.e
echo "insert into $attribute_table					\
	(attribute,							\
	hint_message,							\
	appaserver_yn,							\
	attribute_datatype,						\
	width,								\
	float_decimal_places,						\
	post_change_javascript,						\
	on_focus_javascript_function,					\
	lookup_histogram_output_yn,					\
	lookup_time_chart_output_yn)					\
	values('password','An empty password will prevent logging in.','y','password',56,null,null,null,null,null);"	 |
sql.e									  
) 2>&1 | grep -vi duplicate
exit 0
