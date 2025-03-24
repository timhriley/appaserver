#!/bin/bash
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: sudo $0 execute|noexecute" 1>&2
	exit 1
fi

if [ $UID -ne 0 ]
then
	sudo -p "Restarting with sudo. Password: " bash $0 $*
	exit $?
fi

execute=$1

if [ "$execute" != "execute" ]
then
	less $0
	exit 0
fi

profile_file="/etc/profile"
appaserver_config_file="/etc/appaserver.config"

integrity_check ()
{
	if [ ! -r $appaserver_config_file ]
	then
		echo "Can't read $appaserver_config_file" 1>&2
		exit 1
	fi
}

append_etc_profile ()
{
	appaserver_home=$1
	document_root=$2

	echo "" >> ${profile_file}
	echo "# Added by $0" >> ${profile_file}
	echo "# ==============================================" >> ${profile_file}
	echo '# See $APPASERVER_HOME/template/group_centric.txt' >> ${profile_file}
	echo "umask 0002" >> ${profile_file}
	echo "" >> ${profile_file}
	echo "set -o vi" >> ${profile_file}
	echo "export APPASERVER_HOME=${appaserver_home}" >> ${profile_file}
	echo "export CGI_HOME=/usr/lib/cgi-bin" >> ${profile_file}
	echo "export APPASERVER_UNUSED_PARAMETER_FLAG=-Wunused-parameter" >> ${profile_file}
	echo "export DOCUMENT_ROOT=${document_root}" >> ${profile_file}
	echo 'export PATH=$PATH:$APPASERVER_HOME/utility' >> ${profile_file}
	echo 'export PATH=$PATH:$APPASERVER_HOME/src_appaserver' >> ${profile_file}
	echo 'export PATH=$PATH:$APPASERVER_HOME/src_system' >> ${profile_file}
	echo 'export PATH=$PATH:.' >> ${profile_file}
	echo "" >> ${profile_file}
	echo "# Prevent common Trojan horses" >> ${profile_file}
	echo "alias sl=ls" >> ${profile_file}
	echo "alias mkae=make" >> ${profile_file}
	echo "alias maek=make" >> ${profile_file}
	echo "alias mkea=make" >> ${profile_file}
	echo "alias mkea=make" >> ${profile_file}
	echo "alias amke=make" >> ${profile_file}
	echo "alias amek=make" >> ${profile_file}
	echo "" >> ${profile_file}
	echo "# Shortcuts" >> ${profile_file}
	echo "alias cda=\"cd \$APPASERVER_HOME\"" >> ${profile_file}
	echo "alias cdd=\"cd \$DOCUMENT_ROOT\"" >> ${profile_file}
	echo "alias lsd=\"ls -la | grep '^d'\"" >> ${profile_file}
	echo "alias lsdn=\"ls -1d * | sort_delimited_numbers.e '.' n\"" >> ${profile_file}
	echo "mkcd() { mkdir \"$1\" && chmod o-rx \"$1\" && cd \"$1\"; }" >> ${profile_file}
	echo "" >> ${profile_file}
	echo "export UTC_OFFSET=-8" >> ${profile_file}
	echo "export DATABASE=template" >> ${profile_file}
	echo "# Uncomment if APPASERVER_HOME and CGI_HOME are on different filesystems" >> ${profile_file}
	echo "#export APPASERVER_LINK_FLAGS=-s" >> ${profile_file}
}

integrity_check

label="appaserver_mount_point="
appaserver_home=`cat $appaserver_config_file	| \
		 grep "^${label}"		| \
		 sed "s/$label//"`

label="document_root="
document_root=`cat $appaserver_config_file	| \
		 grep "^${label}"		| \
		 sed "s/$label//"`

append_etc_profile $appaserver_home $document_root

echo "Next:"
echo "$ . /etc/profile"
echo "Then:"
echo "$ sudo ./install-filesystem.sh execute|noexecute"

exit 0
