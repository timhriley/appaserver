:

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 execute_yn [data/index_directory]" 1>&2
	exit 1
fi

execute_yn=$1

if [ "$#" -eq 2 ]
then
	data_directory="$2"
	index_directory="$2"
fi

hydrology_file=hydrology.tar.gz
measurement_file=measurement.sql.gz
measurement_backup_file=measurement_backup.sql.gz

key=mysqldump_hydrology_load

# Start the timer.
# ----------------
start_time.e $key 2>&1

# Start the work.
# ---------------
mysqldump_hydrology_untar.sh $hydrology_file $execute_yn

if [ "$?" -ne 0 ]
then
	echo "$0 aborting" 1>&2
	exit 1
fi

if [ "$execute_yn" = "y" ]
then
	# Send to background
	# ------------------
	output=hydrology.out

	nohup /usr/bin/time mysqldump_hydrology_cat_sql.sh 1>$output 2>&1 &

	# Send to background
	# ------------------
	output=measurement_backup.out

	nohup								 \
	/usr/bin/time							 \
	mysqldump_measurement_backup_cat_sql.sh	$measurement_backup_file \
		1>$output 2>&1 &

	# Don't send to background
	# ------------------------
	output=measurement.out

	# create_preprocess="mysqldump_hydrology_replace_index.sh"
	create_preprocess=""

	/usr/bin/time						\
	mysqldump_block_load.sh	$measurement_file		\
				40				\
				"$data_directory"		\
				"$index_directory"		\
				$execute_yn			\
				"$create_preprocess"		\
		1>$output 2>&1

	# Stop the timer.
	# ---------------
	stop_time.e $key 2>&1

else
	# Stop the timer.
	# ---------------
	stop_time.e $key 2>/dev/null

	less $0
fi

exit 0

