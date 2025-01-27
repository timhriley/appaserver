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

create_document_root_appaserver ()
{
	document_root=$1

	appaserver_directory="${document_root}/appaserver"
	mkdir $appaserver_directory
}

create_document_root_template ()
{
	document_root=$1

	appaserver_directory="${document_root}/appaserver"
	mkdir $appaserver_directory/template
}

create_template_directories ()
{
	appaserver_data=$1
	appaserver_upload=$2
	appaserver_log=$3

	mkdir $appaserver_data/template
	ln -s $APPASERVER_HOME/zfonts $appaserver_data/template/.texlive2019
	ln -s $APPASERVER_HOME/zfonts $appaserver_data/template/.texlive2021
	mkdir $appaserver_upload/template
	touch $appaserver_log/appaserver_template.err
}

copy_document_root_template ()
{
	document_root=$1

	template_directory="${document_root}/appaserver/template"
	index_filename="$APPASERVER_HOME/template/index.php"
	style_filename="$APPASERVER_HOME/template/style.css"

	cat $index_filename				|
	sed 's/title_change_me/Template Application/'	|
	sed 's/application_change_me/template/'		|
	cat > $template_directory/index.php

	cp $style_filename $template_directory
}

copy_menu ()
{
	cp $APPASERVER_HOME/template/menu.dat /home/$SUDO_USER/
}

link_document_root ()
{
	appaserver_data=$1
	document_root=$2

	ln -s $APPASERVER_HOME $document_root/appaserver_home
	ln -s $appaserver_data $document_root/appaserver_data
	ln -s $APPASERVER_HOME/zmenu $document_root/zmenu
	ln -s $APPASERVER_HOME/zscal2 $document_root/zscal2
	ln -s $APPASERVER_HOME/zimages $document_root/zimages
}

# -----------------------------------------------
# Note: this tries to create /foo/log/appaserver.
# Carefull, /foo/log may not exist.
# -----------------------------------------------
create_appaserver_log_directory ()
{
	appaserver_log=$1

	mkdir $appaserver_log
}

create_log_file_template ()
{
	appaserver_log=$1

	template_log_file="${appaserver_log}/appaserver_template.err"
	touch $template_log_file
}

create_backups_appaserver ()
{
	backups_directory=$1

	mkdir ${backups_directory}/appaserver
}

create_backups_template ()
{
	backups_directory=$1

	template_directory="${backups_directory}/appaserver/template"
	mkdir $template_directory
}

copy_backups_template ()
{
	backups_directory=$1

	template_directory="${backups_directory}/appaserver"
	config_filename="mysqldump_template.config"

	cat $config_filename					|
	sed 's/change_me/template/'				|
	cat > $template_directory/mysqldump_template.config
}

create_appaserver_data ()
{
	appaserver_data=$1

	mkdir $appaserver_data
}

create_appaserver_upload ()
{
	appaserver_upload=$1

	mkdir $appaserver_upload
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

if [ "$execute" != "execute" ]
then
	integrity_check
	less $0
	exit 0
fi

integrity_check
create_appaserver_data $appaserver_data
create_appaserver_upload $appaserver_upload
create_appaserver_log_directory $appaserver_log
create_log_file_template $appaserver_log
create_backups_appaserver "/var/backups"
create_backups_template "/var/backups"
copy_backups_template "/var/backups"
link_document_root $appaserver_data $document_root
create_document_root_appaserver $document_root
create_template_directories $appaserver_data $appaserver_upload $appaserver_log
create_document_root_template $document_root
copy_document_root_template $document_root
copy_menu

echo "Next:"
echo "$ sudo ./install-protect.sh execute|noexecute"

exit 0
