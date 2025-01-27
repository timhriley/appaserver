:
# -----------------------------------------------------------------------
# $APPASERVER_HOME/utility/mysqldump_extract.sh
#
# This script extracts /var/backups/appaserver/$database/son/$date.sql.gz
# to /var/backups/appaserver/$database/son/untar/
# -----------------------------------------------------------------------
if [ "$#" -ne 3 ]
then
echo "Usage: $0 database parameter_file yyyy_mm_dd" 1>&2
echo "Note: have appaserver_var_backups.tar in current directory." 1>&2
	exit 1
fi

database=$1
parameter_file=$2
date_yyyy_mm_dd=$3
date_no_dashes=`echo $date_yyyy_mm_dd | sed 's/-//g'`

parse_parameter_file()
{
	file=$1
	key=$2

	results=`cat $file | grep "^${key}=" | piece.e "=" 1`
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

parse_parameter_file $parameter_file directory_root
error_exit_if_blank "$results" directory_root
directory_root=$results

# Expect results=son
# ------------------
parse_parameter_file $parameter_file output_directory
error_exit_if_blank "$results" output_directory

# backup_root=/var/backups
# ------------------------
backup_root=`echo $directory_root | sed 's|/appaserver.*$||'`

# master_archive=/var/backups/appaserver_var_backups.tar
# ----------------------------------------------------------------
master_archive=${backup_root}/appaserver_var_backups.tar

# son_directory=/var/backups/appaserver/$database/son
# ---------------------------------------------------
son_directory=${directory_root}/${results}

# untar_directory=/var/backups/appaserver/$database/son/untar
# -----------------------------------------------------------
untar_directory=${son_directory}/untar

# input_file=/var/backups/appaserver/$database/son/$database_{$date}.tar.gz
# -------------------------------------------------------------------------
input_file=${son_directory}/${database}_${date_no_dashes}.tar.gz

# archive_expression=appaserver/$database/son/*_$date*
# ----------------------------------------------------
archive_expression="appaserver/$database/son/*_$date_no_dashes*"

# Make a clean directory
# ----------------------
if [ ! -d $untar_directory ]
then
	mkdir $untar_directory
fi

rm -f $untar_directory/* 2>/dev/null

# Extract from the master archive the backup files
# ------------------------------------------------
cd $backup_root
tar xf $master_archive --wildcards "$archive_expression"

if [ $? -ne 0 ]
then
	echo "$0 exiting: failed to extract [$archive_expression] from [$master_archive]" 1>&2
	exit 1
fi

# Extract from the backup files to the untar directory
# ----------------------------------------------------
cd $untar_directory
tar xzf $input_file

if [ $? -ne 0 ]
then
	echo "$0 exiting: failed to extract from ${untar_directory}/${input_file}" 1>&2
	exit 1
fi

pwd

exit 0
