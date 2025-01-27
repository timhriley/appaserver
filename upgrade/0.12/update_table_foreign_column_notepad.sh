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
update appaserver_table set notepad = 'Normally, the foreign keys are the same names as their corresponding primary keys. Moreover, they are in the same order as their corresponding primary keys. Frequently, the last foreign key may differ. If only the last foreign key differs, then set RELATION.related_column. Infrequently, the foreign keys are not in the same order, or multiple foreign key names differ. If so, then populate FOREIGN_COLUMN.' where table_name = 'foreign_column';
shell_all_done
) | sql.e
exit 0
