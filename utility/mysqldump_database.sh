:
# ----------------------------------------------
# $APPASERVER_HOME/utility/mysqldump_database.sh
#
# This is the backup driver program.
#-----------------------------------------------
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 parameter_file" 1>&2
	exit 1
fi

parameter_file=$1

date_yyyy_mm_dd=`now.sh ymd`
appaserver_config=/etc/appaserver.config
mysqluser=root
mysql_password_file=/tmp/mysql_password_file_$$.dat

echo "`basename.e $0 n` `basename.e $1 n` Beg at `date.e 0`"

# Returns $results
# ----------------
parse_parameter_file()
{
	file=$1
	key=$2

	results=`cat $file | grep "^${key}=" | piece.e '=' 1`
}

error_exit_if_blank()
{
	results=$1
	pattern=$2

	if [ "$results" = "" ]
	then
		echo "Error: cannot find $pattern in parameter_file." 1>&2
		exit 1
	fi
}

parse_parameter_file $parameter_file database
export DATABASE=$results
export APPASERVER_DATABASE=$results
database=$results

parse_parameter_file $parameter_file directory_root
directory_root=$results

parse_parameter_file $parameter_file output_directory
error_exit_if_blank "$results" output_directory
output_directory=$results

parse_parameter_file $parameter_file processes_in_parallel
error_exit_if_blank "$results" processes_in_parallel
processes_in_parallel=$results

parse_parameter_file $parameter_file each_line_insert
each_line_insert=$results

parse_parameter_file $parameter_file big_table_list
big_table_list=$results

parse_parameter_file $parameter_file exclude_table_list
exclude_table_list=$results

if [ "$directory_root" != "" ]
then
	# Expect output_directory=/var/backups/appaserver/$database/son
	# -------------------------------------------------------------
	output_directory=${directory_root}/$output_directory
fi

# Build mysql_password_file
# -------------------------
echo "[mysqldump]" > $mysql_password_file

chmod go-rw $mysql_password_file

grep '^password=' $appaserver_config >> $mysql_password_file

grep '^mysql_password=' $appaserver_config	|
sed 's/mysql_//'				|
cat >> $mysql_password_file

# Make the directories, if first time for this parameter file.
# ------------------------------------------------------------
if [ ! -d $output_directory -a "$directory_root" != "" ]
then
	new_directory="${directory_root}"
	mkdir $new_directory
	chmod o-rwx $new_directory

	new_directory="${directory_root}/son"
	mkdir $new_directory
	chmod o-rwx $new_directory

	new_directory="${directory_root}/father"
	mkdir $new_directory
	chmod o-rwx $new_directory

	new_directory="${directory_root}/grandfather"
	mkdir $new_directory
	chmod o-rwx $new_directory

	new_directory="${directory_root}/greatgrandfather"
	mkdir $new_directory
	chmod o-rwx $new_directory

	new_directory="${directory_root}/greatgreatgrandfather"
	mkdir $new_directory
	chmod o-rwx $new_directory
fi

mysqldump_fork.e	$mysqluser			\
			$database			\
			$mysql_password_file		\
			$output_directory		\
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
