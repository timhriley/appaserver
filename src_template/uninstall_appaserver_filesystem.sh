:
# Freely available software. See appaserver.org
# ---------------------------------------------

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
	execute=$1

	if [ "$execute" = "execute" -a "$USER" != "root" ]
	then
		echo "Error: You must be root to run this." 1>&2
		exit 1
	fi

	if [ "$DOCUMENT_ROOT" = "" ]
	then
		echo "Error: DOCUMENT_ROOT must be set." 1>&2
		exit 1
	fi

	if [ "$APPASERVER_HOME" = "" ]
	then
		echo "Error: APPASERVER_HOME must be set." 1>&2
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
	execute=$1

	appaserver_directory="${DOCUMENT_ROOT}/appaserver"
	zmenu_link="${DOCUMENT_ROOT}/zmenu"

	if [ "$execute" = "execute" ]
	then
		rm -fr $appaserver_directory
		rm $zmenu_link
	else
		echo "rm -fr $appaserver_directory"
		echo "rm $zmenu_link"
	fi
}

remove_appaserver_error_directory ()
{
	appaserver_error=$1
	execute=$2

	if [ "$execute" = "execute" ]
	then
		rm -fr $appaserver_error
	else
		echo "rm -fr $appaserver_error"
	fi
}

remove_old_appaserver_error_file ()
{
	execute=$1

	old_appaserver_error_file="/var/log/appaserver.err"

	if [ "$execute" = "execute" ]
	then
		rm $old_appaserver_error_file
	else
		echo "rm $old_appaserver_error_file"
	fi
}

remove_backups_appaserver ()
{
	backups_directory=$1
	execute=$2

	if [ "$execute" = "execute" ]
	then
		rm -fr ${backups_directory}/appaserver
	else
		echo "rm -fr ${backups_directory}/appaserver"
	fi
}

remove_appaserver_data ()
{
	appaserver_data=$1
	execute=$2

	if [ "$execute" = "execute" ]
	then
		rm -fr $appaserver_data
	else
		echo "rm -fr $appaserver_data"
	fi
}

reset_cgi_home ()
{
	execute=$1

	if [ "$execute" = "execute" ]
	then
		chmod g-ws $CGI_HOME
		chgrp root $CGI_HOME
	else
		echo "chmod g-ws $CGI_HOME"
		echo "chgrp root $CGI_HOME"
	fi
}

enable_apache_cgi ()
{
	if [ "$execute" = "execute" ]
	then
		a2enmod cgi
		apache2ctl restart
	else
		echo "a2enmod cgi"
		echo "apache2ctl restart"
	fi
}

label="appaserver_error_directory="
appaserver_error=`	cat $appaserver_config_file	|\
	 		grep "^${label}"		|\
	 		sed "s/$label//"`

label="appaserver_data_directory="
appaserver_data=`	cat $appaserver_config_file	|\
	 		grep "^${label}"		|\
	 		sed "s/$label//"`

. $profile_file

integrity_check $execute
remove_appaserver_error_directory $appaserver_error $execute
remove_old_appaserver_error_file $execute
remove_appaserver_data $appaserver_data $execute
remove_backups_appaserver "/var/backups" $execute
remove_document_root_appaserver $execute
reset_cgi_home $execute

exit 0
