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

echo "delete from appaserver_column where column_name = 'omit_ajax_fill_drop_down_yn';" | sql
echo "delete from table_column where column_name = 'omit_ajax_fill_drop_down_yn';" | sql
echo "update relation set ajax_fill_drop_down_yn = 'y' where table_name = 'journal' and related_table = 'account';" | sql
echo "update relation set ajax_fill_drop_down_yn = null where table_name = 'account' and related_table = 'subclassification';" | sql

# Returns 0 if RELATION.omit_ajax_fill_drop_down_yn exists
# --------------------------------------------------------
table_column_exists.sh relation omit_ajax_fill_drop_down_yn

if [ $? -eq 0 ]
then
	echo "alter table relation drop column omit_ajax_fill_drop_down_yn;" | sql
fi


exit 0
