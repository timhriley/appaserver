#!/bin/sh
# ---------------
# roles4appaserver_user.sh
# ------------------------

# echo Starting: $0 $* 1>&2

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application login_name" 1>&2
	exit 1
fi

application=$1
login_name=$2

role_folder=`get_table_name $application role_folder`
role_appaserver_user=`get_table_name $application role_appaserver_user`
role_process=`get_table_name $application role_process`

(
echo "select $role_appaserver_user.role					\
      from $role_folder, $role_appaserver_user				\
      where $role_folder.role = $role_appaserver_user.role		\
	and $role_appaserver_user.login_name = '$login_name';"		|
sql.e;
echo "select $role_appaserver_user.role					\
      from $role_process, $role_appaserver_user				\
      where $role_process.role = $role_appaserver_user.role		\
	and $role_appaserver_user.login_name = '$login_name';"		|
sql.e
) | sort -u

