:

echo "$0 $*" 1>&2

if  [ "$#" -lt 2 ]
then
	echo "Usage: $0 measurement_file execute_yn [mysql_datadir]" 1>&2
	exit 1
fi

measurement_file=$1
execute_yn=$2

if [ "$#" -eq 3 ]
then
	mysql_datadir=$3
else
	mysql_datadir=`mysql_datadir.sh`
fi

hydrology_datadir=$mysql_datadir/hydrology

ssd_directory="/ssd1/mysql"

remove_ssd ()
{
	ssd_directory=$1

	if [ -f $ssd_directory/measurement.MYD ]
	then
		if [ `whoami` != "mysql" ]
		then
			echo "ERROR in $0/remove_ssd(): you must be mysql" 1>&2
			exit 1
		fi

		rm $ssd_directory/measurement.MYD
		rm $ssd_directory/measurement.MYI
	fi
}

link_ssd ()
{
	ssd_directory=$1
	hydrology_datadir=$2

	if [ ! -f $ssd_directory/measurement.MYD ]
	then
		if [ `whoami` != "mysql" ]
		then
			echo "ERROR in $0/link_ssd(): you must be mysql" 1>&2
			exit 1
		fi

		file=measurement.MYD
		mv $hydrology_datadir/$file $ssd_directory
		ln -s $ssd_directory/$file $hydrology_datadir/$file

		file=measurement.MYI
		mv $hydrology_datadir/$file $ssd_directory
		ln -s $ssd_directory/$file $hydrology_datadir/$file
	fi
}

create_measurement ()
{
	measurement_file=$1

	zcat $measurement_file						      |
	grep -vi 'drop table'						      |
	head -40							      |
	sed "s/NOT NULL DEFAULT CURRENT_TIMESTAMP/DEFAULT CURRENT_TIMESTAMP/" |
	sed "s/NOT NULL DEFAULT '',/NOT NULL,/"				      |
	sed "s/NOT NULL DEFAULT '0000-00-00',/NOT NULL,/"		      |
	sed "s/NOT NULL DEFAULT 'null',/NOT NULL,/"			      |
	sed_data_directory.sh /ssd1/mysql /ssd1/mysql			      |
	tee /tmp/zcat_measurement_create_$$.sql				      |
	sql.e								      |
	cat
}

load_measurement ()
{
	measurement_file=$1

	zcat $measurement_file			|
	grep -i 'insert into'			|
	sed "s/'0000-00-00'/null/g"		|
	sed "s/'0000-00-00 00:00:00'/null/g"	|
	sql_quick.e
}

drop_measurement ()
{
	echo "drop table measurement;" | sql.e
}

if [ "$execute_yn" = "y" ]
then
	drop_measurement
	remove_ssd $ssd_directory
	create_measurement $measurement_file
	link_ssd $ssd_directory $hydrology_datadir
	load_measurement $measurement_file
else
	less $0
fi

exit 0
