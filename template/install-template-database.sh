#!/bin/bash
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in $0: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

if [ "$application" != "template" ]
then
	echo "Error in $0: you must connect to the template database." 1>&2
	exit 1
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
	results=`	cat $appaserver_config_file		|\
			grep mysql_password=			|\
			grep secret`

	if [ "$results" != "" ]
	then
		echo "You need to set the mysql password." 1>&2
		echo "Check $appaserver_config_file" 1>&2
		exit 1
	fi

	if [ $UID = 0 ]
	then
		echo "You cannot be root to run this." 1>&2
		echo "Don't: sudo $0" 1>&2
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
	echo "create database template;" | sql.e mysql
}

update_ssl_support ()
{
	echo "update appaserver_application set ssl_support_yn = 'n';" |
	sql.e
}

load_mysqldump_template ()
{
	cd $APPASERVER_HOME/template
	zcat mysqldump_template.sql.gz | sql.e
}

insert_user_nobody ()
{
	table="appaserver_user"
	field="login_name,password,user_date_format"

	echo "nobody|changeit|american"			|
	insert_statement.e table=$table field=$field	|
	sql.e

	table="role_appaserver_user"
	field="login_name,role"

	(
	echo "nobody|system"
	echo "nobody|supervisor"
	)						|
	insert_statement.e table=$table field=$field	|
	sql.e
}

if [ "$execute" != "execute" ]
then
	integrity_check
	less $0
	exit 0
fi

integrity_check
create_template_database
load_mysqldump_template
update_ssl_support
insert_user_nobody

echo "Next:"
echo "$ appaserver-upgrade"

echo ""
echo "Followed by:"
echo "$ template/install-extra.sh | less"

exit 0
