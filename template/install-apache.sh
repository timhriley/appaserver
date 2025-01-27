#!/bin/bash
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ $UID -ne 0 ]
then
	sudo -p "Restarting with sudo. Password: " bash $0 $*
	exit $?
fi

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 execute|noexecute" 1>&2
	exit 1
fi

execute=$1

profile_file="/etc/profile"

integrity_check ()
{
	if [ "$APPASERVER_HOME" = "" ]
	then
		echo "Error: APPASERVER_HOME must be set." 1>&2
		exit 1
	fi

	if [ "$SUDO_USER" = "" ]
	then
		echo "Error: Cannot determine your regular user name." 1>&2
		exit 1
	fi
}

. $profile_file

if [ "$execute" != "execute" ]
then
	integrity_check
	less $0
	exit 0
fi

configure_apache ()
{
	a2enmod cgi
}

copy_appaserver ()
{
	APPASERVER_HOME=$1

	cd /etc/apache2/sites-available
	cp $APPASERVER_HOME/template/appaserver80.conf .
}

protect_appaserver ()
{
	cd /etc/apache2/sites-available
	chown www-data:appaserver appaserver80.conf
	chmod g+w appaserver80.conf
}

engage_appaserver ()
{
	a2dissite 000-default
	a2ensite appaserver80
}

restart_apache ()
{
	echo "Executing: systemctl restart apache2"
	systemctl restart apache2
}

integrity_check
configure_apache
copy_appaserver $APPASERVER_HOME
protect_appaserver
engage_appaserver
restart_apache

echo "Next:"
echo "$ cd \$APPASERVER_HOME"
echo "$ make"

exit 0
