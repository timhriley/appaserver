#!/bin/sh
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_system/orphan_process_folder_list.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

# echo "Starting: $0 $*" 1>&2 <-- Goes to Apache's error file.

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
fi

if [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in $0: you must first . set_database" 1>&2
	exit 1
fi

echo "	select table_name				\
	from appaserver_table				\
	where exists					\
		( select table_name			\
		  from relation				\
		  where appaserver_table.table_name =	\
		        relation.table_name );"		|
sql.e							|
sort

exit 0
