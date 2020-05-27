:

echo "$0 $*" 1>&2

if  [ "$#" -lt 2 ]
then
	echo "Usage: $0 measurement_backup_file execute_yn [mysql_datadir]" 1>&2
	exit 1
fi

measurement_backup_file=$1
execute_yn=$2

if [ "$#" -eq 3 ]
then
	mysql_datadir=$3
else
	mysql_datadir=`mysql_datadir.sh`
fi

hydrology_datadir=$mysql_datadir/hydrology

usr3_directory="/usr3/mysql"

remove_usr3 ()
{
	usr3_directory=$1

	if [ -f $usr3_directory/measurement_backup.MYD ]
	then
		if [ `whoami` != "mysql" ]
		then
			echo "ERROR in $0/remove_usr3(): you must be mysql" 1>&2
			exit 1
		fi

		rm $usr3_directory/measurement_backup.MYD
		rm $usr3_directory/measurement_backup.MYI
	fi
}

link_usr3 ()
{
	usr3_directory=$1
	hydrology_datadir=$2

	if [ ! -f $usr3_directory/measurement_backup.MYD ]
	then
		if [ `whoami` != "mysql" ]
		then
			echo "ERROR in $0/link_usr3(): you must be mysql" 1>&2
			exit 1
		fi

		file=measurement_backup.MYD
		mv $hydrology_datadir/$file $usr3_directory
		ln -s $usr3_directory/$file $hydrology_datadir/$file

		file=measurement_backup.MYI
		mv $hydrology_datadir/$file $usr3_directory
		ln -s $usr3_directory/$file $hydrology_datadir/$file
	fi
}

create_measurement_backup ()
{
	measurement_backup_file=$1

	zcat $measurement_backup_file					      |
	grep -vi 'drop table'						      |
	head -40							      |
	sed "s/NOT NULL DEFAULT CURRENT_TIMESTAMP/DEFAULT CURRENT_TIMESTAMP/" |
	sed "s/NOT NULL DEFAULT '',/NOT NULL,/"				      |
	sed "s/NOT NULL DEFAULT '0000-00-00',/NOT NULL,/"		      |
	sed "s/NOT NULL DEFAULT 'null',/NOT NULL,/"			      |
	sed_data_directory.sh /usr3/mysql /usr3/mysql			      |
	tee /tmp/zcat_measurement_backup_create_$$.sql			      |
	sql.e								      |
	cat
}

load_measurement_backup ()
{
	measurement_backup_file=$1

	zcat $measurement_backup_file		|
	grep -i 'insert into'			|
	sed "s/'0000-00-00 00:00:00'/null/g"	|
	sed "s/'0000-00-00'/null/g"		|
	sql_quick.e
}

drop_measurement_backup ()
{
	echo "drop table measurement_backup;" | sql.e
}

if [ "$execute_yn" = "y" ]
then
	drop_measurement_backup
	remove_usr3 $usr3_directory
	create_measurement_backup $measurement_backup_file
	link_usr3 $usr3_directory $hydrology_datadir
	load_measurement_backup $measurement_backup_file
else
	less $0
fi

exit 0
