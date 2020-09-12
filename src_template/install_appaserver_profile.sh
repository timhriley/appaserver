:
# Freely available software. See appaserver.org
# ---------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: sudo $0 execute|noexecute" 1>&2
	exit 1
fi

execute=$1

profile_file="/etc/profile"
appaserver_config_file="/etc/appaserver.config"

integrity_check ()
{
	if [ "$USER" != "root" ]
	then
		echo "You must be root to run this." 1>&2
		exit 1
	fi

	if [ ! -r $appaserver_config_file ]
	then
		echo "Can't read $appaserver_config_file" 1>&2
		exit 1
	fi
}

append_etc_profile_display ()
{
	appaserver_home=$1
	document_root=$2

	echo "Will append the following to ${profile_file}"
	echo "--------------------------------------------"

	echo "# Added by $0"
	echo "# =============================================="
	echo "# See group_centric.txt"
	echo "umask 0002"
	echo ""
	echo "export APPASERVER_HOME=${appaserver_home}"
	echo "export CGI_HOME=/usr/lib/cgi-bin"
	echo "export APPASERVER_UNUSED_PARAMETER_FLAG=-Wunused-parameter"
	echo "export DOCUMENT_ROOT=${document_root}"
	echo 'export PATH=$PATH:$APPASERVER_HOME/utility:$APPASERVER_HOME/src_appaserver:.'
	echo "export UTC_OFFSET=-8"
	echo "export DATABASE=template"
	echo "# Uncomment if APPASERVER_HOME and CGI_HOME are on different filesystems"
	echo "#export APPASERVER_LINK_FLAGS=-s"
}

append_etc_profile_execute ()
{
	appaserver_home=$1
	document_root=$2

	echo "" >> ${profile_file}
	echo "# Added by $0" >> ${profile_file}
	echo "# ==============================================" >> ${profile_file}
	echo "# See group_centric.txt" >> ${profile_file}
	echo "umask 0002" >> ${profile_file}
	echo "" >> ${profile_file}
	echo "export APPASERVER_HOME=${appaserver_home}" >> ${profile_file}
	echo "export CGI_HOME=/usr/lib/cgi-bin" >> ${profile_file}
	echo "export APPASERVER_UNUSED_PARAMETER_FLAG=-Wunused-parameter" >> ${profile_file}
	echo "export DOCUMENT_ROOT=${document_root}" >> ${profile_file}
	echo 'export PATH=$PATH:$APPASERVER_HOME/utility:$APPASERVER_HOME/src_appaserver:.' >> ${profile_file}
	echo 'export UTC_OFFSET=-8' >> ${profile_file}
	echo 'export DATABASE=template' >> ${profile_file}
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

if [ "$execute" = "execute" ]
then
	append_etc_profile_execute $appaserver_home $document_root
else
	append_etc_profile_display $appaserver_home $document_root
fi

echo "Next:"
echo "$ . /etc/profile"

exit 0
