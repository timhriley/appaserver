:
# ------------------------------------------------
# $APPASERVER_HOME/utility/mysqldump_block_load.sh
# ------------------------------------------------
# Freely available software. See appaserver.org
# ------------------------------------------------

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

if  [ "$#" -ne 4 ]
then
	echo "Usage: $0 backup_file head_create destination_directory execute_yn" 1>&2
	exit 1
fi

backup_file=$1
head_create=$2
destination_directory=$3
execute_yn=$4

if [ "$destination_directory" = "destination_directory" ]
then
	destination_directory=""
fi

mysql_datadir=`mysql_datadir.sh`

if [ "$mysql_datadir" = "" ]
then
	mysql_datadir="/var/lib/mysql"
fi

application_datadir=$mysql_datadir/$application

remove_destination_files()
{
	destination_directory=$1
	table_name=$2

	if [ "$destination_directory" = "" ]
	then
		return
	fi

	if [ -f $destination_directory/${table_name}.MYD ]
	then
		if [ `whoami` != "mysql" ]
		then
			echo "ERROR in $0: you must be mysql" 1>&2
			exit 1
		fi

		rm $destination_directory/${table_name}.MYD
		rm $destination_directory/${table_name}.MYI
	fi
}

link_destination_directory()
{
	destination_directory=$1
	application_datadir=$2
	table_name=$3

	if [ "$destination_directory" = "" ]
	then
		return
	fi

	file=${table_name}.MYD

	if [ ! -f $destination_directory/$file ]
	then
		if [ `whoami` != "mysql" ]
		then
			echo "ERROR in $0: you must be mysql" 1>&2
			exit 1
		fi

		mv $application_datadir/$file $destination_directory
		ln -s $destination_directory/$file $application_datadir/$file

		file=${table_name}.MYI

		mv $application_datadir/$file $destination_directory
		ln -s $destination_directory/$file $application_datadir/$file
	fi
}

create_table()
{
	backup_file=$1
	head_create=$2
	destination_directory=$3
	table_name=$4

	DD=$destination_directory

	zcat $backup_file						      |
	grep -vi 'drop table'						      |
	head -${head_create}						      |
	sed "s/NOT NULL DEFAULT CURRENT_TIMESTAMP/DEFAULT CURRENT_TIMESTAMP/" |
	sed "s/NOT NULL DEFAULT '',/NOT NULL,/"				      |
	sed "s/NOT NULL DEFAULT '0000-00-00',/NOT NULL,/"		      |
	sed "s/NOT NULL DEFAULT 'null',/NOT NULL,/"			      |
	sed_data_directory.sh "$DD" "$DD"				      |
	tee /tmp/mysql_block_load_create_$$.sql				      |
	sql.e								      |
	cat
}

load_table()
{
	backup_file=$1

	zcat $backup_file			|
	grep -i 'insert into'			|
	sed "s/'0000-00-00'/null/g"		|
	sed "s/'0000-00-00 00:00:00'/null/g"	|
	sql_quick.e				|
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

	remove_destination_files "$destination_directory" $table_name

	create_table	$backup_file					\
			$head_create					\
			"$destination_directory"			\
			$table_name

	link_destination_directory	"$destination_directory"	\
					$application_datadir		\
					$table_name

	load_table $backup_file
else
	less $0
fi

exit 0
