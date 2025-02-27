#!/bin/sh
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_admin/post_email_address_block.sh
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

key="email_address"

execute="update_statement.e table=email_address key=$key carrot=y | sql.e"

echo "select email_address, IP_address from post where $where;"	|
sql.e '^'							|
while read record
do
	email_address=`echo $record | piece.e '^' 0`
	ip_address=`echo $record | piece.e '^' 1`

	/usr2/ufw/ufw_whitelist.sh $ip_address

	if [ "$?" -ne 0 ]
	then
		echo "$email_address^blocked_yn^y" | $execute
	fi
done

exit 0
