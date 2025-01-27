:
# ------------------------------------
# utility/csvdump_database.sh
# ------------------------------------
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 database parameter_file" 1>&2
	exit 1
fi

database=$1
parameter_file=$2

date_yyyy_mm_dd=`date.e 0 | piece.e ':' 0`

echo "$0 starting at `date.e 0`"

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

parse_parameter_file $parameter_file directory_root
directory_root=$results

parse_parameter_file $parameter_file output_directory
error_exit_if_blank "$results" output_directory
output_directory=$results

parse_parameter_file $parameter_file processes_in_parallel
error_exit_if_blank "$results" processes_in_parallel
processes_in_parallel=$results

if [ "$directory_root" != "" ]
then
	output_directory=${directory_root}/$output_directory
fi

DATABASE=$database; export DATABASE

csvdump_fork.e		$database			\
			$output_directory		\
			$processes_in_parallel

return_value=$?

if [ "$return_value" -ne 0 ]
then
	echo "csvdump_fork.e returned an error." 1>&2
	exit 1
fi

mysqldump_archive_grandfather_father_son.sh $parameter_file $date_yyyy_mm_dd

echo "$0 ending at   `date.e 0`"

exit 0
