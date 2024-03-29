#!/bin/sh
# ---------------------------------------------
# folder_count_list.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 3 ]
then
	echo \
	"Usage: $0 application role permission_list" 1>&2
	echo \
	"permission_list looks like: insert,update,lookup" 1>&2
	exit 1
fi

application=$1
role=$2
permission_list=$3

for folder_name in `role_folder_list.sh $application $role "$permission_list"`
do
	table_name=`get_table_name $application $folder_name`
	count=`echo "select count(*) from $table_name;" | sql_timeout.sh 1`
	echo "$folder_name^$count"
done

exit 0
