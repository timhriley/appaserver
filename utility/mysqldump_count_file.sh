:
# ------------------------------------------------
# $APPASERVER_HOME/utility/mysqldump_count_file.sh
#
# This is called from mysqldump_database.sh
# Retired.
# ------------------------------------------------

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 database parameter_file yyyy_mm_dd" 1>&2
	exit 1
fi

database=$1
parameter_file=$2
date_yyyy_mm_dd=$3
date_no_dashes=`echo $date_yyyy_mm_dd | sed 's/-//g'`

parse_parameter_file ()
{
	file=$1
	key=$2

	results=`cat $file | grep "^${key}=" | piece.e "=" 1`
}

error_exit_if_blank ()
{
	results=$1
	pattern=$2

	if [ "$results" = "" ]
	then
		echo "Error: cannot find $pattern in parameter_file." 1>&2
		exit 1
	fi
}

parse_parameter_file $parameter_file appaserver_config
appaserver_config=$results

parse_parameter_file $parameter_file exclude_table_list
exclude_table_list=$results

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

if [ ! -f "$appaserver_config" ]
then
	appaserver_config=/etc/appaserver.config
fi

parse_parameter_file $appaserver_config appaserver_error_directory
appaserver_error_directory=$results

if [ "$appaserver_error_directory" = "" ]
then
	appaserver_error_directory=/var/log/appaserver
fi

mysqldump_count_file=/var/backups/appaserver/mysqldump_count_${database}.dat
destination_file=${output_directory}/mysqldump_count_${database}_${date_no_dashes}.dat

cp $mysqldump_count_file $destination_file

exit 0
