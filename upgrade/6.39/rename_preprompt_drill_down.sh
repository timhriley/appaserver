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

echo "update attribute set attribute = 'drilldown_yn' where attribute = 'preprompt_yn';" | sql
echo "update folder_attribute set attribute = 'drilldown_yn' where folder = 'folder' and attribute = 'preprompt_yn';" | sql
echo "update folder_attribute set attribute = 'drilldown_yn' where folder = 'process_parameter' and attribute = 'preprompt_yn';" | sql
echo "update folder_attribute set attribute = 'drilldown_yn' where folder = 'process_set_parameter' and attribute = 'preprompt_yn';" | sql

echo "alter table folder change preprompt_yn drilldown_yn char (1);" | sql
echo "alter table process_parameter change preprompt_yn drilldown_yn char (1);" | sql
echo "alter table process_set_parameter change preprompt_yn drilldown_yn char (1);" | sql

exit 0
