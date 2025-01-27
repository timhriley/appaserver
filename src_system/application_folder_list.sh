:
# application_folder_list.sh
# --------------------------

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

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 1 ]
then
	echo "Usage: `basename.e $0 n` system_folders_yn" 1>&2
	exit 1
fi

system_folders_yn=$1

if [ "$system_folders_yn" = "y" ]
then
	appaserver_folder_list $application | grep -v "^null$"
else
	temp_file1=/tmp/folder_list1_$$.tmp
	temp_file2=/tmp/folder_list2_$$.tmp

	appaserver_folder_list $application | sort > $temp_file1

	select.e	application=$application			\
			select=table_name				\
			folder=table	 				\
			where="table_name <> 'null'"			|
	sort -u								|
	cat > $temp_file2
	join -v 2 $temp_file1 $temp_file2
	rm $temp_file1 $temp_file2
fi

