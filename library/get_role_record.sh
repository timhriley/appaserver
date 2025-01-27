#!/bin/sh
# --------------------------------------------
# get_role_record.sh
# --------------------------------------------
#
# Freely available software: see Appaserver.org
# --------------------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application role" 1>&2
	exit 1
fi

application_name=$1
role=$2

role_table=role
select="folder_count_yn,override_row_restrictions_yn,grace_no_cycle_colors_yn"

echo "select $select							\
      from $role_table							\
      where role = '$role';"						|
sql.e '^'

