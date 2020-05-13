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
	group=$1
	execute=$2

	if [ "$execute" = "execute" -a "$USER" != "root" ]
	then
		echo "Error: You must be root to run this." 1>&2
		echo "Try: sudo $0" 1>&2
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

	if [ "`umask`" != "0002" ]
	then
		echo "Error: umask needs to be set to 0002." 1>&2
		exit 1
	fi

	group_exists=`grep "^${group}" /etc/group`

	if [ "$group_exists" = "" ]
	then
		echo "Error: Group $group doesn't exist in /etc/group" 1>&2
		exit 1
	fi

	if [ "$SUDO_USER" = "" ]
	then
		echo "Warning: Cannot determine your regular user name." 1>&2
		return
	fi

	user_exists=`grep "^${group}" /etc/group | grep $SUDO_USER`

	if [ "$user_exists" = "" ]
	then
		echo "Error: User $SUDO_USER doesn't belong to group $group in /etc/group" 1>&2
		exit 1
	fi
}

protect_cgi_home ()
{
	group=$1
	execute=$2

	if [ "$execute" = "execute" ]
	then
		chgrp $group ${CGI_HOME}
		chmod g+rwxs ${CGI_HOME}
	else
		echo "chgrp $group ${CGI_HOME}"
		echo "chmod g+rwxs ${CGI_HOME}"
	fi
}

protect_document_root_template ()
{
	group=$1
	execute=$2

	template_directory="${DOCUMENT_ROOT}/appaserver/template"

	if [ "$execute" = "execute" ]
	then
		chgrp $group $template_directory
		chgrp $group $template_directory/index.php
		chgrp $group $template_directory/style.css
		chmod g+rwxs $template_directory
		chmod o-w $template_directory
		chmod g+w ${template_directory}/index.php
		chmod g+w ${template_directory}/style.css

		chgrp $group ${template_directory}/data
		chmod g+rwxs $template_directory/data
		chmod o-w ${template_directory}/data

		chgrp $group ${template_directory}/images
		chgrp $group ${template_directory}/images/logo_appaserver.jpg
		chmod g+rwxs $template_directory/images
		chmod o-w ${template_directory}/images
		chmod g+w ${template_directory}/images/logo_appaserver.jpg
	else
		echo "chgrp $group $template_directory"
		echo "chgrp $group $template_directory/index.php"
		echo "chgrp $group $template_directory/style.css"
		echo "chmod g+rwxs $template_directory"
		echo "chmod o-w $template_directory"
		echo "chmod g+w ${template_directory}/index.php"
		echo "chmod g+w ${template_directory}/style.css"

		echo "chgrp $group ${template_directory}/data"
		echo "chmod g+rwxs $template_directory/data"
		echo "chmod o-w ${template_directory}/data"

		echo "chgrp $group ${template_directory}/images"
		echo "chgrp $group ${template_directory}/images/logo_appaserver.jpg"
		echo "chmod g+rwxs $template_directory/images"
		echo "chmod o-w ${template_directory}/images"
		echo "chmod g+w ${template_directory}/images/logo_appaserver.jpg"
	fi
}

protect_document_root_appaserver ()
{
	group=$1
	execute=$2

	if [ "$execute" = "execute" ]
	then
		chgrp $group $DOCUMENT_ROOT/appaserver
		chmod g+rwxs $DOCUMENT_ROOT/appaserver
		chmod o-rwx $DOCUMENT_ROOT/appaserver
	else
		echo "chgrp $group $DOCUMENT_ROOT/appaserver"
		echo "chmod g+rwxs $DOCUMENT_ROOT/appaserver"
		echo "chmod o-rwx $DOCUMENT_ROOT/appaserver"
	fi
}

protect_appaserver_error_directory ()
{
	group=$1
	appaserver_error=$2
	execute=$3

	if [ "$execute" = "execute" ]
	then
		chgrp $group $appaserver_error
		chmod g+rwxs $appaserver_error
		chmod o-rwx $appaserver_error
	else
		echo "chgrp $group $appaserver_error"
		echo "chmod g+rwxs $appaserver_error"
		echo "chmod o-rwx $appaserver_error"
	fi
}

protect_appaserver_home ()
{
	group=$1
	execute=$2

	if [ "$execute" = "execute" ]
	then
		chgrp $group $APPASERVER_HOME
		chmod g+rwxs $APPASERVER_HOME
		chmod o-w $APPASERVER_HOME
	else
		echo "chgrp $group $APPASERVER_HOME"
		echo "chmod g+rwxs $APPASERVER_HOME"
		echo "chmod o-w $APPASERVER_HOME"
	fi
}

protect_old_appaserver_error_file ()
{
	group=$1
	execute=$2

	old_appaserver_error_file="/var/log/appaserver.err"

	if [ "$execute" = "execute" ]
	then
		chgrp $group $old_appaserver_error_file
		chmod g+rw $old_appaserver_error_file
		chmod o-r $old_appaserver_error_file
	else
		echo "chgrp $group $old_appaserver_error_file"
		echo "chmod g+rw $old_appaserver_error_file"
		echo "chmod o-rw $old_appaserver_error_file"
	fi
}

protect_error_file_template ()
{
	group=$1
	appaserver_error=$2
	execute=$3

	template_error_file="${appaserver_error}/appaserver_template.err"

	if [ "$execute" = "execute" ]
	then
		chgrp $group $template_error_file
		chmod g+rw $template_error_file
		chmod o-rw $template_error_file
	else
		echo "chgrp $group $template_error_file"
		echo "chmod g+rw $template_error_file"
		echo "chmod o-rw $template_error_file"
	fi
}

protect_appaserver_config ()
{
	group=$1
	execute=$2

	if [ "$execute" = "execute" ]
	then
		chgrp $group $appaserver_config_file
		chmod g+rw $appaserver_config_file
		chmod o-rw $appaserver_config_file
	else
		echo "chgrp $group $appaserver_config_file"
		echo "chmod g+rw $appaserver_config_file"
		echo "chmod o-rw $appaserver_config_file"
	fi
}

protect_appaserver_data ()
{
	group=$1
	appaserver_data=$2
	execute=$3

	if [ "$execute" = "execute" ]
	then
		chgrp $group $appaserver_data
		chmod g+rwxs $appaserver_data
		chmod o-rwx $appaserver_data
	else
		echo "chgrp $group $appaserver_data"
		echo "chmod g+rwxs $appaserver_data"
		echo "chmod o-rwx $appaserver_data"
	fi
}

chgrp_appaserver_tree ()
{
	group=$1
	execute=$2

	if [ "$execute" = "execute" ]
	then
		cd $APPASERVER_HOME
		find . -type d -exec chmod g+wxs {} \;
		find . -exec chmod g+w {} \;
		find . -exec chgrp $group {} \;
	else
		echo "cd $APPASERVER_HOME"
		echo "find . -type d -exec chmod g+wxs {} \;"
		echo "find . -exec chmod g+w {} \;"
		echo "find . -exec chgrp $group {} \;"
	fi
}

label="appaserver_group="
group=`	cat $appaserver_config_file	|\
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

integrity_check $group $execute
protect_cgi_home $group $execute
protect_appaserver_home $group $execute
protect_appaserver_error_directory $group $appaserver_error $execute
protect_old_appaserver_error_file $group $execute
protect_appaserver_data $group $appaserver_data $execute
protect_appaserver_config $group $execute
protect_error_file_template $group $appaserver_error $execute
protect_document_root_appaserver $group $execute
protect_document_root_template $group $execute
chgrp_appaserver_tree $group $execute

exit 0
