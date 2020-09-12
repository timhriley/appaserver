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
	results=`	cat $appaserver_config_file		|\
			grep mysql_password=			|\
			grep secret`

	if [ "$results" != "" ]
	then
		echo "You need to set the mysql password." 1>&2
		echo "Check $appaserver_config_file" 1>&2
		exit 1
	fi

	if [ "$USER" = "root" ]
	then
		echo "You cannot be root to run this." 1>&2
		echo "Don't: sudo $0" 1>&2
		exit 1
	fi

	if [ "$DOCUMENT_ROOT" = "" ]
	then
		echo "DOCUMENT_ROOT must be set." 1>&2
		exit 1
	fi

	if [ "$APPASERVER_HOME" = "" ]
	then
		echo "APPASERVER_HOME must be set." 1>&2
		exit 1
	fi

	if [ ! -x ${APPASERVER_HOME}/src_appaserver/sql.e ]
	then
		echo "sql.e doesn't exist. Did you compile?"
		exit 1
	fi
}

create_template_database ()
{
	execute=$1

	if [ "$execute" = "execute" ]
	then
		echo "create database template;" | sql.e mysql
	else
		echo 'echo "create database template;" | sql.e mysql'
	fi
}

update_ssl_support ()
{
	execute=$1

	if [ "$execute" = "execute" ]
	then
		echo "update template_application set ssl_support_yn = 'n';" |
		sql.e
	else
		echo "echo \"update template_application set ssl_support_yn = 'n';\" | sql.e"

	fi
}

load_mysqldump_template ()
{
	execute=$1

	if [ "$execute" = "execute" ]
	then
		cd $APPASERVER_HOME/template
		zcat mysqldump_template.sql.gz | sql.e
	else
		echo 'cd $APPASERVER_HOME/template'
		echo "zcat mysqldump_template.sql.gz | sql.e"
	fi
}

. $profile_file

integrity_check
create_template_database $execute
load_mysqldump_template $execute
update_ssl_support $execute

echo "Next:"
echo "$ upgrade-appaserver-database template"

echo ""
echo "Followed by:"
echo '$ appaserver_virgin_install.sh `whoami` noexecute'

exit 0
