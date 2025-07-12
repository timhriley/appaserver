#!/bin/sh
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_admin/post_purge.sh
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

post_email_address_block.sh
post_ufw_deny.sh


echo "delete from post_contact where exists ( select 1 from post where post_contact.email_address = post.email_address and post_contact.timestamp = post.timestamp and confirmation_received_date is not null );" | sql

echo "delete from post_signup where exists ( select 1 from post where post_signup.email_address = post.email_address and post_signup.timestamp = post.timestamp and confirmation_received_date is not null );" | sql

echo "delete from post where confirmation_received_date is not null;" | sql

exit $?
