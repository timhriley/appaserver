#!/bin/bash
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
appaserver_config_file="/etc/appaserver.config"

integrity_check ()
{
	if [ "$DOCUMENT_ROOT" = "" ]
	then
		echo "Error: DOCUMENT_ROOT must be set." 1>&2
		exit 1
	fi

	if [ "$CGI_HOME" = "" ]
	then
		echo "Error: CGI_HOME must be set." 1>&2
		exit 1
	fi
}

remove_document_root_appaserver ()
{

	rm -fr $DOCUMENT_ROOT/appaserver
	rm $DOCUMENT_ROOT/appaserver_home
	rm $DOCUMENT_ROOT/appaserver_data
	rm $DOCUMENT_ROOT/zmenu
	rm $DOCUMENT_ROOT/zscal2
	rm $DOCUMENT_ROOT/zimages
}

remove_appaserver_log_directory ()
{
	appaserver_log=$1

	rm -fr $appaserver_log
}

remove_backups_appaserver ()
{
	backups_directory=$1

	rm -fr ${backups_directory}/appaserver
}

remove_appaserver_data ()
{
	appaserver_data=$1

	rm -fr $appaserver_data
}

remove_appaserver_upload ()
{
	appaserver_upload=$1

	rm -fr $appaserver_upload
}

reset_cgi_home ()
{
	chmod u+w $CGI_HOME
	chmod g-ws $CGI_HOME
	chmod o+rx $CGI_HOME
	chown root:root $CGI_HOME
}

reset_document_root ()
{
	chmod g-ws $DOCUMENT_ROOT
	chmod o+rx $DOCUMENT_ROOT
	chown root:root $DOCUMENT_ROOT
}

remove_apache2 ()
{
	a2dissite appaserver80
	rm /etc/apache2/sites-available/appaserver80.conf
	a2ensite 000-default

	echo "Executing: systemctl restart apache2"
	systemctl restart apache2
}

label="log_directory="
appaserver_log=`	cat $appaserver_config_file	|\
	 		grep "^${label}"		|\
	 		sed "s/$label//"`

label="data_directory="
appaserver_data=`	cat $appaserver_config_file	|\
	 		grep "^${label}"		|\
	 		sed "s/$label//"`

label="upload_directory="
appaserver_upload=`	cat $appaserver_config_file	|\
	 		grep "^${label}"		|\
	 		sed "s/$label//"`

. $profile_file

if [ "$execute" != "execute" ]
then
	integrity_check
	less $0
	exit 0
fi

integrity_check
remove_appaserver_log_directory $appaserver_log
remove_appaserver_data $appaserver_data
remove_appaserver_upload $appaserver_upload
remove_backups_appaserver "/var/backups"
remove_document_root_appaserver
reset_cgi_home
reset_document_root
remove_apache2

exit 0
