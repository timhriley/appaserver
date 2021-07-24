#!/bin/sh
# ---------------------------------------------
# get_role_folder_insert_list.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 3 ]
then
	echo \
	"Usage: $0 application ignored role" 1>&2
	exit 1
fi

application=$1
#session=$2
role=$3

role_folder=`get_table_name $application role_folder`

echo "select folder							\
      	from $role_folder						\
      	where role = '$role'						\
	  and permission = 'insert';"					|
sql.e

