:
# $APPASERVER_HOME/utility/fix_orphans.sh
# ---------------------------------------

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

if [ "$#" != 2 ]
then
	echo "Usage: $0 max_record_count delete_yn" 1>&2
	exit 1
fi

max_record_count=$1
delete_yn=$2

for folder_name in `echo "select folder from folder;" | sql.e`
do
	#echo "folder_name = $folder_name"

	if [ "$folder_name" = "null" ]
	then
		continue
	fi

	table_name=`get_table_name $application $folder_name`
	record_count=`echo "select count(*) from $table_name;" | sql.e 2>&1`

	results=`strncmp.e "$record_count" ERROR 5`

	if [ "$results" -eq 0 ]
	then
		echo "ERROR: table $table_name does not exist." 1>&2
		continue
	fi

	if [ "$max_record_count" -gt 0 ]
	then
		if [ "$record_count" -ge "$max_record_count" ]
		then
			continue
		fi
	fi

	data_list_yn=n
	execute_yn=y

	fix_orphans	fix_orphans		\
			$folder_name		\
			$data_list_yn		\
			$delete_yn		\
			$execute_yn		\
			stdout
done

exit 0
