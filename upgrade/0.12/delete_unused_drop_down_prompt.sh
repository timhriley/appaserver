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

echo "delete from drop_down_prompt where drop_down_prompt not in ( select distinct drop_down_prompt from process_parameter ) and drop_down_prompt not in ( select distinct drop_down_prompt from process_set_parameter );" | sql.e

echo "delete from drop_down_prompt_data where drop_down_prompt not in ( select distinct drop_down_prompt from drop_down_prompt );" | sql.e

exit 0
