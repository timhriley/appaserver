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
alter table attribute drop column lookup_histogram_output_yn;
delete from folder_attribute where attribute = 'lookup_histogram_output_yn';
delete from attribute where attribute = 'lookup_histogram_output_yn';

alter table attribute drop column lookup_time_chart_output_yn;
delete from folder_attribute where attribute = 'lookup_time_chart_output_yn';
delete from attribute where attribute = 'lookup_time_chart_output_yn';

alter table attribute drop column appaserver_yn;
delete from folder_attribute where attribute = 'appaserver_yn';
delete from attribute where attribute = 'appaserver_yn';

alter table attribute drop column on_focus_javascript_function;
delete from folder_attribute where attribute = 'on_focus_javascript_function';
delete from attribute where attribute = 'on_focus_javascript_function';

alter table attribute drop column post_change_javascript;
delete from folder_attribute where folder = 'attribute' and attribute = 'post_change_javascript';
all_done

exit 0
