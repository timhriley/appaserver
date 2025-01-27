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
appaserver_config_file="/etc/appaserver.config"

integrity_check ()
{
	apache_user=$1
	group=$2

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

	if [ "`umask`" != "0002" ]
	then
		echo "Error: umask needs to be set to 0002." 1>&2
		exit 1
	fi

	user_exists=`grep "^${apache_user}:" /etc/passwd`

	if [ "$user_exists" = "" ]
	then
		echo "Error: User ${apache_user} doesn't exist in /etc/passwd" 1>&2
		exit 1
	fi

	group_exists=`grep "^${group}:" /etc/group`

	if [ "$group_exists" = "" ]
	then
		echo "Error: Group $group doesn't exist in /etc/group" 1>&2
		exit 1
	fi

	if [ "$SUDO_USER" = "" ]
	then
		echo "Error: Cannot determine your regular user name." 1>&2
		exit 1
	fi

	user_exists=`grep "^${group}" /etc/group | grep $SUDO_USER`

	if [ "$user_exists" = "" ]
	then
		echo "Error: User $SUDO_USER doesn't belong to group $group in /etc/group" 1>&2
		exit 1
	fi
}

protect_menu ()
{
	chown $SUDO_USER:$SUDO_USER /home/$SUDO_USER/menu.dat
}

protect_cgi_home ()
{
	apache_user=$1
	group=$2

	chown $apache_user:$group ${CGI_HOME}
	chmod u-w ${CGI_HOME}
	chmod g+rwxs ${CGI_HOME}
	chmod o+rx ${CGI_HOME}
}

protect_directory ()
{
	apache_user=$1
	group=$2
	directory=$3

	chown $apache_user:$group $directory
	chmod g+rwxs $directory
	chmod o-rwx $directory
}

open_directory_read ()
{
	directory=$1

	chmod o+r $directory
}

open_directory_execute ()
{
	directory=$1

	chmod o+x $directory
}

protect_file ()
{
	apache_user=$1
	group=$2
	directory=$3
	file=$4

	chown $apache_user:$group $directory/$file
	chmod g+rw $directory/$file
	chmod o-rw $directory/$file
}

open_appaserver ()
{
	APPASERVER_HOME=$1

	cd $APPASERVER_HOME

	find . -type d -exec chmod o+rx {} \;
	find . -type f -exec chmod o+r {} \;
	find . -name '*.sh' -exec chmod o+rx {} \;
}

label="log_directory="
appaserver_log=`	cat $appaserver_config_file	|\
	 		grep "^${label}"		|\
	 		sed "s/$label//"`

if [ "$appaserver_log" = "" ]
then
	echo "ERROR in $0: cannot find label=log_directory in $appaserver_config_file" 1>&2
	exit 1
fi

label="data_directory="
appaserver_data=`	cat $appaserver_config_file	|\
	 		grep "^${label}"		|\
	 		sed "s/$label//"`

if [ "$appaserver_data" = "" ]
then
	echo "ERROR in $0: cannot find label=data_directory in $appaserver_config_file" 1>&2
	exit 1
fi

label="upload_directory="
appaserver_upload=`	cat $appaserver_config_file	|\
	 		grep "^${label}"		|\
	 		sed "s/$label//"`

if [ "$appaserver_upload" = "" ]
then
	echo "ERROR in $0: cannot find label=upload_directory in $appaserver_config_file" 1>&2
	exit 1
fi

label="backup_directory="
appaserver_backup=`	cat $appaserver_config_file	|\
	 		grep "^${label}"		|\
	 		sed "s/$label//"`

if [ "$appaserver_backup" = "" ]
then
	echo "ERROR in $0: cannot find label=backup_directory in $appaserver_config_file" 1>&2
	exit 1
fi

label="document_root="
document_root=`		cat $appaserver_config_file	|\
	 		grep "^${label}"		|\
	 		sed "s/$label//"`

if [ "$document_root" = "" ]
then
	echo "ERROR in $0: cannot find label=document_root in $appaserver_config_file" 1>&2
	exit 1
fi

. $profile_file

apache_user="www-data"
apache_group="www-data"
group="appaserver"

if [ "$execute" != "execute" ]
then
	integrity_check $apache_user $group
	less $0
	exit 0
fi

integrity_check $apache_user $group

protect_cgi_home $apache_user $group

protect_menu

protect_directory $apache_user $group $appaserver_log
open_directory_read $appaserver_log
protect_file $apache_user $group $appaserver_log appaserver_template.err

protect_directory $apache_user $group $appaserver_data
open_directory_read $appaserver_data
protect_directory $apache_user $group $appaserver_data/template

protect_directory $apache_user $group $appaserver_upload
open_directory_read $appaserver_upload
protect_directory $apache_user $group $appaserver_upload/template

protect_directory $apache_user $apache_user $appaserver_backup
protect_directory $apache_user $apache_user $appaserver_backup/template
protect_file $apache_user $apache_user $appaserver_backup mysqldump_template.config

protect_directory $apache_user $group $document_root
protect_directory $apache_user $group $document_root/appaserver
protect_directory $apache_user $group $document_root/appaserver/template
protect_file $apache_user $group $document_root/appaserver/template index.php
protect_file $apache_user $group $document_root/appaserver/template style.css
open_directory_read $document_root
open_directory_execute $document_root
open_directory_read $document_root/appaserver
open_directory_execute $document_root/appaserver

protect_file $apache_user $group /etc appaserver.config

# Anyone can read or execute all of Appaserver
# --------------------------------------------
open_appaserver $APPASERVER_HOME

echo "Next:"
echo "$ sudo ./install-apache.sh execute|noexecute"

exit 0
