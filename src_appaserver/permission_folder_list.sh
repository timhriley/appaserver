#!/bin/sh
# -----------------------------
# permission_folder_list.sh
# -----------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 3 ]
then
	echo \
	"Usage: $0 application role insert,update,lookup" 1>&2
	exit 1
fi

application=$1
role=$2
permission_list=`single_quotes_around $3`

echo "select role_folder.folder						\
      from role_folder							\
      where role = '$role'						\
	and permission in ( "$permission_list" );"			|
sql.e									|
sort -u

exit 0
