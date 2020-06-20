:

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 measurement_backup_file" 1>&2
	exit 1
fi

measurement_backup_file=$1

mysqldump_block_load.sh	$measurement_backup_file	\
			40				\
			""				\
			""				\
			y

exit $?
