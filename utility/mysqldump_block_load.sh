:
# ---------------------------------------------------------------------
# $APPASERVER_HOME/utility/mysqldump_block_load.sh
# ---------------------------------------------------------------------
# This process parses and loads mysqldump's block option output format.
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------------

display_count=10000000

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

if  [ "$#" -lt 6 ]
then
	echo "Usage: $0 backup_file head_create data_directory index_directory separate_yn execute_yn [create_preprocess] [insert_preprocess]" 1>&2
	exit 1
fi

backup_file=$1
head_create=$2
data_directory=$3
index_directory=$4
separate_yn=$5
execute_yn=$6

if [ "$#" -ge 7 ]
then
	create_preprocess="$7"
else
	create_preprocess="cat"
fi

if [ "$#" -ge 8 ]
then
	insert_preprocess="$8"
else
	insert_preprocess="cat"
fi

if [ "$data_directory" = "data_directory" ]
then
	data_directory=""
fi

if [ "$index_directory" = "index_directory" ]
then
	index_directory=""
fi

mysql_datadir=`mysql_datadir.sh`

if [ "$mysql_datadir" = "" ]
then
	mysql_datadir="/var/lib/mysql"
fi

application_datadir=$mysql_datadir/$application

remove_destination_files()
{
	data_directory=$1
	index_directory=$2
	table_name=$3

	if [ "$data_directory" != "" -a -f $data_directory/${table_name}.MYD ]
	then
		if [ `whoami` != "mysql" ]
		then
			echo "ERROR in $0: you must be mysql" 1>&2
			exit 1
		fi

		rm $data_directory/${table_name}.MYD
	fi

	if [ "$index_directory" != "" -a -f $index_directory/${table_name}.MYI ]
	then
		if [ `whoami` != "mysql" ]
		then
			echo "ERROR in $0: you must be mysql" 1>&2
			exit 1
		fi

		rm $index_directory/${table_name}.MYI
	fi
}

link_destination_files()
{
	data_directory=$1
	index_directory=$2
	application_datadir=$3
	table_name=$4

	if [ "$data_directory" != "" ]
	then
		file=${table_name}.MYD

		if [ ! -f $data_directory/$file ]
		then
			if [ `whoami` != "mysql" ]
			then
				echo "ERROR in $0 1): you must be mysql" 1>&2
				exit 1
			fi

			mv $application_datadir/$file $data_directory
			ln -s $data_directory/$file $application_datadir/$file
		fi
	fi

	if [ "$index_directory" != "" ]
	then
		file=${table_name}.MYI

		if [ ! -f $index_directory/$file ]
		then
			if [ `whoami` != "mysql" ]
			then
				echo "ERROR in $0 2): you must be mysql" 1>&2
				exit 1
			fi

			mv $application_datadir/$file $index_directory
			ln -s $index_directory/$file $application_datadir/$file
		fi
	fi
}

create_table()
{
	backup_file=$1
	head_create=$2
	data_directory=$3
	index_directory=$4
	table_name=$5
	create_preprocess="$6"

	DD=$data_directory
	ID=$index_directory

	zcat $backup_file						      |
	grep -vi 'drop table'						      |
	head -${head_create}						      |
	sed "s/NOT NULL DEFAULT CURRENT_TIMESTAMP/DEFAULT CURRENT_TIMESTAMP/" |
	sed "s/NOT NULL DEFAULT '',/NOT NULL,/"				      |
	sed "s/NOT NULL DEFAULT '0000-00-00',/NOT NULL,/"		      |
	sed "s/NOT NULL DEFAULT 'null',/NOT NULL,/"			      |
	sed_data_directory.sh "$DD" "$ID"				      |
	$create_preprocess						      |
	tee /tmp/mysqldump_block_load_create_$$.sql			      |
	sql.e								      |
	cat
}

load_table_separate()
{
	backup_file=$1
	insert_preprocess="$2"
	display_count=$3

	zcat $backup_file			|
	mysqldump_block_insert_separate.sh	|
	$insert_preprocess			|
	count.e $display_count			|
	/usr/bin/nice sql_quick.e		|
	cat
}

load_table_block()
{
	backup_file=$1

	zcat $backup_file			|
	/usr/bin/nice sql_quick.e		|
	cat
}

drop_table()
{
	table_name=$1

	echo "drop table $table_name;"		|
	sql.e					|
	cat
}

if [ "$execute_yn" = "y" ]
then
	table_name=`echo $backup_file | sed 's/.sql.gz$//'`

	drop_table $table_name

	remove_destination_files	"$data_directory"	\
					"$index_directory"	\
					$table_name

	create_table	$backup_file				\
			$head_create				\
			"$data_directory"			\
			"$index_directory"			\
			$table_name				\
			"$create_preprocess"

	link_destination_files	"$data_directory"		\
				"$index_directory"		\
				$application_datadir		\
				$table_name

	if [ "$separate_yn" = 'y' ]
	then
		load_table_separate	$backup_file		\
					"$insert_preprocess"	\
					$display_count
	else
		load_table_block $backup_file
	fi

	purge_binary_logs.sh

else
	less $0
fi

exit 0
