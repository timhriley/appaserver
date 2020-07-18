:
# --------------------------------------------------------------------
# $APPASERVER_HOME/utility/mysqldump_archive_grandfather_father_son.sh
#
# This is called from mysqldump_database.sh and csvdump_database.sh
# --------------------------------------------------------------------
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 parameter_file yyyy_mm_dd" 1>&2
	exit 1
fi

parameter_file=$1
date_yyyy_mm_dd=$2
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

link_files()
{
	source_directory=$1
	destination_directory=$2
	date_no_dashes=$3

	pwd=`pwd`
	cd $source_directory

	for file in `	ls -1 2>&1 | grep -vi 'no such file'	|\
			grep $date_no_dashes`
	do
		rm -f $destination_directory/$file

echo "ln $file $destination_directory/$file" 1>&2

		ln $file $destination_directory/$file
	done

	cd $pwd
}

parse_parameter_file $parameter_file directory_root
directory_root=$results

parse_parameter_file $parameter_file output_directory
error_exit_if_blank "$results" output_directory
output_directory=$results

if [ "$directory_root" != "" ]
then
	# Expect output_directory=/var/backups/appaserver/$database/son
	# -------------------------------------------------------------
	output_directory=${directory_root}/$output_directory
fi

# Manage son
# ----------
parse_parameter_file $parameter_file son_keep_days
error_exit_if_blank "$results" son_keep_days
son_keep_days=$results
purge_old_filenames.sh $output_directory $son_keep_days

# Manage father 
# -------------
parse_parameter_file $parameter_file father_directory
error_exit_if_blank "$results" father_directory
father_directory=$results

if [ "$directory_root" != "" ]
then
	father_directory=${directory_root}/$father_directory
fi

echo "date_grandfather_father_son.sh $date_yyyy_mm_dd father" 1>&2

results=`date_grandfather_father_son.sh $date_yyyy_mm_dd father`

if [ $results -eq 1 ]
then
echo "link_files $output_directory $father_directory $date_no_dashes" 1>&2
	link_files $output_directory $father_directory $date_no_dashes
fi

parse_parameter_file $parameter_file father_keep_days
error_exit_if_blank "$results" father_keep_days
father_keep_days=$results
purge_old_filenames.sh $father_directory $father_keep_days

# Manage grandfather 
# ------------------
parse_parameter_file $parameter_file grandfather_directory
error_exit_if_blank "$results" grandfather_directory
grandfather_directory=$results

if [ "$directory_root" != "" ]
then
	grandfather_directory=${directory_root}/$grandfather_directory
fi

echo "date_grandfather_father_son.sh $date_yyyy_mm_dd grandfather" 1>&2

results=`date_grandfather_father_son.sh $date_yyyy_mm_dd grandfather`

if [ $results -eq 1 ]
then
echo "link_files $output_directory $grandfather_directory $date_no_dashes" 1>&2
	link_files $output_directory $grandfather_directory $date_no_dashes
fi

parse_parameter_file $parameter_file grandfather_keep_days
error_exit_if_blank "$results" grandfather_keep_days
grandfather_keep_days=$results
purge_old_filenames.sh $grandfather_directory $grandfather_keep_days

# Manage greatgrandfather 
# -----------------------
parse_parameter_file $parameter_file greatgrandfather_directory
error_exit_if_blank "$results" greatgrandfather_directory
greatgrandfather_directory=$results

if [ "$directory_root" != "" ]
then
	greatgrandfather_directory=${directory_root}/$greatgrandfather_directory
fi

echo "date_grandfather_father_son.sh $date_yyyy_mm_dd greatgrandfather" 1>&2
results=`date_grandfather_father_son.sh $date_yyyy_mm_dd greatgrandfather`

if [ $results -eq 1 ]
then

echo "link_files $output_directory $greatgrandfather_directory $date_no_dashes" 1>&2

	link_files $output_directory $greatgrandfather_directory $date_no_dashes
fi

parse_parameter_file $parameter_file greatgrandfather_keep_days
error_exit_if_blank "$results" greatgrandfather_keep_days
greatgrandfather_keep_days=$results
purge_old_filenames.sh $greatgrandfather_directory $greatgrandfather_keep_days

# Manage greatgreatgrandfather 
# ----------------------------
parse_parameter_file $parameter_file greatgreatgrandfather_directory
error_exit_if_blank "$results" greatgreatgrandfather_directory
greatgreatgrandfather_directory=$results

if [ "$directory_root" != "" ]
then
	greatgreatgrandfather_directory=${directory_root}/$greatgreatgrandfather_directory
fi

echo "date_grandfather_father_son.sh $date_yyyy_mm_dd greatgreatgrandfather" 1>&2

results=`date_grandfather_father_son.sh $date_yyyy_mm_dd greatgreatgrandfather`

if [ $results -eq 1 ]
then
echo "link_files	$output_directory			\
			$greatgreatgrandfather_directory	\
			$date_no_dashes" 1>&2

	link_files	$output_directory			\
			$greatgreatgrandfather_directory	\
			$date_no_dashes
fi

parse_parameter_file $parameter_file greatgreatgrandfather_keep_days
error_exit_if_blank "$results" greatgreatgrandfather_keep_days
greatgreatgrandfather_keep_days=$results
purge_old_filenames.sh	$greatgreatgrandfather_directory	\
			$greatgreatgrandfather_keep_days

exit 0
