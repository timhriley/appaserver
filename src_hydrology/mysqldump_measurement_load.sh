:

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 measurement_file [data_directory] [index_directory]" 1>&2
	exit 1
fi

measurement_file=$1

if [ "$#" -ge 2 ]
then
	data_directory=$2
fi

if [ "$#" -ge 3 ]
then
	data_directory=$3
fi

# create_preprocess="mysqldump_hydrology_replace_index.sh"

mysqldump_block_load.sh	$measurement_file		\
			40				\
			"$data_directory"		\
			"$index_directory"		\
			y

exit $?
