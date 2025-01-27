:
# ----------------------------------------------
# $APPASERVER_HOME/utility/mysqldump_database.sh
#-----------------------------------------------
# Retired and replaced with mysqldump_appaserver
#-----------------------------------------------
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 parameter_file" 1>&2
	exit 1
fi

parameter_file=$1

appaserver_backup_directory=`appaserver_backup_directory`

if [ "$appaserver_backup_directory" = "" ]
then
	echo "ERROR in `basename.e $0 n`: cannot get appaserver_backup_directory" 1>&2
	exit 1
fi

date_yyyy_mm_dd=`now.sh ymd`
appaserver_config=/etc/appaserver.config

echo "`basename.e $0 n` `basename.e $1 n` Beg at `date.e 0`"

# Returns $result
# ---------------
parse_parameter_file()
{
	file=$1
	key=$2

	result=`cat $file | grep "^${key}=" | piece.e '=' 1`
}

error_exit_if_blank()
{
	result=$1
	pattern=$2

	if [ "$result" = "" ]
	then
		echo "Error: cannot find $pattern in parameter_file." 1>&2
		exit 1
	fi
}

parse_parameter_file $parameter_file database
export APPASERVER_DATABASE=$result
database=$result

parse_parameter_file $parameter_file processes_in_parallel
error_exit_if_blank "$result" processes_in_parallel
processes_in_parallel=$result

parse_parameter_file $parameter_file big_table_list
big_table_list=$result

parse_parameter_file $parameter_file exclude_table_list
exclude_table_list=$result

parse_parameter_file $parameter_file each_line_insert
each_line_insert=$result

mysqldump_directory=${appaserver_backup_directory}/$database

# Make the directories, if first time for this parameter file.
# ------------------------------------------------------------
if [ ! -d $mysqldump_directory ]
then
	mysqldump_mkdir.sh ${appaserver_backup_directory} $database
fi

son_directory=${appaserver_backup_directory}/$database/son

# Build mysql_password_file
# -------------------------
mysql_password_file=/tmp/mysql_password_file_$$.dat
echo "[mysqldump]" > $mysql_password_file

chmod go-rw $mysql_password_file

grep '^password=' $appaserver_config >> $mysql_password_file

# If old config file
# ------------------
grep '^mysql_password=' $appaserver_config	|
sed 's/mysql_//'				|
cat >> $mysql_password_file

mysqluser=`grep ^mysql_user=$appaserver_config | piece.e '=' 1`

if [ "$mysqluser" == "" ]
then
	echo "ERROR in `basename.e $0 n`: cannot get mysqluser from $appaserver_config" 1>&2
	exit 1
fi

mysqldump_appaserver	$mysqluser			\
			$database			\
			$mysql_password_file		\
			$mysqldump_directory		\
			$processes_in_parallel		\
			"$each_line_insert"		\
			"$big_table_list"		\
			"$exclude_table_list"

return_value=$?

rm -f $mysql_password_file

if [ "$return_value" -ne 0 ]
then
	echo "mysqldump_fork.e returned an error." 1>&2
	exit 1
fi

# After the backup, run these:
# ----------------------------
mysqldump_appaserver_log.sh $database $parameter_file $date_yyyy_mm_dd
mysqldump_archive_grandfather_father_son.sh $parameter_file $date_yyyy_mm_dd
mysqldump_offsite_rsync.sh $database $parameter_file
mysqldump_offsite_scp.sh $parameter_file $date_yyyy_mm_dd

echo "`basename.e $0 n` `basename.e $* n` End at `date.e 0`"

exit 0
