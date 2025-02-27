#!/bin/sh
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_admin/post_ufw_deny.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

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

if [ "$application" != "admin" ]
then
	echo "Error in $0: you must connect to database=admin" 1>&2
	exit 1
fi

where="confirmation_received_date is null" 

echo "select IP_address from post where $where;"	|
sql.e							|
xargs.e "sudo /usr2/ufw/ufw_deny {}"

exit $?
