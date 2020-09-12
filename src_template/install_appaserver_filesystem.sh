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
	cgi_home=$1
	execute=$2

	if [ "$execute" = "execute" -a "$USER" != "root" ]
	then
		echo "Error: You must be root to run this." 1>&2
		echo "Try: sudo $0" 1>&2
		exit 1
	fi

	if [ ! -d "${cgi_home}" ]
	then
		echo "Warning: ${cgi_home} has not been created." 1>&2
	fi

	if [ "$DOCUMENT_ROOT" = "" ]
	then
		echo "Error: DOCUMENT_ROOT must be set." 1>&2
		echo "Try: sudo $0" 1>&2
		exit 1
	fi

	if [ "$APPASERVER_HOME" = "" ]
	then
		echo "Error: APPASERVER_HOME must be set." 1>&2
		echo "Try: sudo $0" 1>&2
		exit 1
	fi
}

create_document_root_appaserver ()
{
	execute=$1

	appaserver_directory="${DOCUMENT_ROOT}/appaserver"

	if [ "$execute" = "execute" ]
	then
		mkdir $appaserver_directory
	else
		echo "mkdir $appaserver_directory"
	fi
}

create_document_root_template ()
{
	execute=$1

	template_directory="${DOCUMENT_ROOT}/appaserver/template"

	if [ "$execute" = "execute" ]
	then
		mkdir $template_directory
		mkdir ${template_directory}/data
		mkdir ${template_directory}/images
	else
		echo "mkdir $template_directory"
		echo "mkdir ${template_directory}/data"
		echo "mkdir ${template_directory}/images"
	fi
}

copy_document_root_template ()
{
	execute=$1

	template_directory="${DOCUMENT_ROOT}/appaserver/template"
	logo_filename="../template/images/logo_appaserver.jpg"

	if [ "$execute" = "execute" ]
	then
		cp ../template/index.php $template_directory
		ln ${template_directory}/index.php ${template_directory}/data/index.php
		cp ../template/style.css $template_directory
		ln ${template_directory}/style.css ${template_directory}/data/style.css
		cp $logo_filename ${template_directory}/images
	else
		echo "cp ../template/index.php $template_directory"
		echo "ln ${template_directory}/index.php ${template_directory}/data/index.php"
		echo "cp ../template/style.css $template_directory"
		echo "ln ${template_directory}/style.css ${template_directory}/data/style.css"
		echo "cp $logo_filename ${template_directory}/images"
	fi
}

link_document_root ()
{
	execute=$1

	src_template_directory="${APPASERVER_HOME}/src_template"
	images_directory="${APPASERVER_HOME}/zimages"
	menu_directory="${APPASERVER_HOME}/zmenu"
	calendar_directory="${APPASERVER_HOME}/zscal2"
	javascript_directory="${APPASERVER_HOME}/javascript"

	if [ "$execute" = "execute" ]
	then
		ln -s $src_template_directory $DOCUMENT_ROOT/appaserver
		ln -s $images_directory $DOCUMENT_ROOT/appaserver
		ln -s $menu_directory $DOCUMENT_ROOT/appaserver
		ln -s $menu_directory $DOCUMENT_ROOT
		ln -s $calendar_directory $DOCUMENT_ROOT/appaserver
		ln -s $javascript_directory $DOCUMENT_ROOT/appaserver
	else
		echo "ln -s $src_template_directory $DOCUMENT_ROOT/appaserver"
		echo "ln -s $images_directory $DOCUMENT_ROOT/appaserver"
		echo "ln -s $menu_directory $DOCUMENT_ROOT/appaserver"
		echo "ln -s $menu_directory $DOCUMENT_ROOT"
		echo "ln -s $calendar_directory $DOCUMENT_ROOT/appaserver"
		echo "ln -s $javascript_directory $DOCUMENT_ROOT/appaserver"
	fi
}

# -----------------------------------------------
# Note: this tries to create /foo/log/appaserver.
# Carefull, /foo/log may not exist.
# -----------------------------------------------
create_appaserver_error_directory ()
{
	appaserver_error=$1
	execute=$2

	if [ "$execute" = "execute" ]
	then
		mkdir $appaserver_error
	else
		echo "mkdir $appaserver_error"
	fi
}

create_old_appaserver_error_file ()
{
	appaserver_error=$1
	execute=$2

	old_appaserver_error_file="${appaserver_error}/appaserver.err"

	if [ "$execute" = "execute" ]
	then
		touch $old_appaserver_error_file
	else
		echo "touch $old_appaserver_error_file"
	fi
}

create_error_file_template ()
{
	appaserver_error=$1
	execute=$2

	template_error_file="${appaserver_error}/appaserver_template.err"

	if [ "$execute" = "execute" ]
	then
		touch $template_error_file
	else
		echo "mkdir $template_error_file"
	fi
}

create_backups_appaserver ()
{
	backups_directory=$1
	execute=$2

	if [ "$execute" = "execute" ]
	then
		mkdir ${backups_directory}/appaserver
	else
		echo "mkdir ${backups_directory}/appaserver"
	fi
}

create_appaserver_data ()
{
	appaserver_data=$1
	execute=$2

	if [ "$execute" = "execute" ]
	then
		mkdir $appaserver_data
	else
		echo "mkdir $appaserver_data"
	fi
}

enable_apache ()
{
	if [ "$execute" = "execute" ]
	then
		a2enmod cgi
		a2enmod ssl
		a2ensite default-ssl
		apache2ctl restart
	else
		echo "a2enmod cgi"
		echo "a2enmod ssl"
		echo "a2ensite default-ssl"
		echo "apache2ctl restart"
	fi
}

label="cgi_home="
cgi_home=`	cat $appaserver_config_file	|\
	 	grep "^${label}"		|\
	 	sed "s/$label//"`

label="appaserver_error_directory="
appaserver_error=`	cat $appaserver_config_file	|\
	 		grep "^${label}"		|\
	 		sed "s/$label//"`

label="appaserver_data_directory="
appaserver_data=`	cat $appaserver_config_file	|\
	 		grep "^${label}"		|\
	 		sed "s/$label//"`

. $profile_file

integrity_check $cgi_home $execute
create_appaserver_error_directory $appaserver_error $execute
create_old_appaserver_error_file $appaserver_error $execute
create_appaserver_data $appaserver_data $execute
create_backups_appaserver "/var/backups" $execute
create_error_file_template $appaserver_error $execute
create_document_root_appaserver $execute
create_document_root_template $execute
copy_document_root_template $execute
link_document_root $execute
enable_apache $execute

echo "Next:"
echo "$ sudo ./protect_appaserver_filesystem.sh noexecute|execute"

exit 0
