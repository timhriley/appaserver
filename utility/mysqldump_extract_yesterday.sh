# Must . this file
# ---------------------------------------------------------------
# $APPASERVER_HOME/utility/mysqldump_extract_yesterday.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ $# -lt 1 ]
then
	echo "Usage: . mysqldump_extract_yesterday.sh database [days_ago] [backup_directory]" 1>&2
	echo "Note: days_ago defaults to 0, which should represent yesterday." 1>&2
	echo "Note: backup_directory defaults to current directory." 1>&2
else
	database=$1

	if [ $# -gt 1 ]
	then
		days_ago=$2
	else
		days_ago=0
	fi

	if [ $# -gt 2 ]
	then
		backup_directory=$3
	else
		backup_directory="."
	fi

	db=$database

	mysqldump_config=$backup_directory/appaserver/mysqldump_${db}.config

	cd $(mysqldump_extract.sh	$database		\
					$mysqldump_config	\
					`now.sh ymd $days_ago`)

	export DATABASE=$db
fi

# -----------------
# No exit statement
# -----------------
