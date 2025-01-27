:
# ---------------------------------------------------
# $APPASERVER_HOME/utility/mysqldump_offsite_rsync.sh
# ---------------------------------------------------
# Retired.
# ---------------------------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 son offsite_rsync_destination" 1>&2
	exit 1
fi

son=$1
offsite_rsync_destination=$2

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

offsite_rsync()
{
	source_directory=$1
	destination_directory=$2

	/usr/bin/time						\
	rsync -aq --delete	${source_directory}/		\
				${destination_directory} 1>&2
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

parse_parameter_file $parameter_file offsite_rsync_destination
offsite_rsync_destination=$results

if [ "$offsite_rsync_destination" != "" ]
then
	offsite_rsync $output_directory $offsite_rsync_destination
fi

exit 0
