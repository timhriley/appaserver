:
# $APPASERVER_HOME/utility/mysqldump_clone.sh
# ---------------------------------------------------------------
# Note: the destination_database must exist and be empty.
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

echo `basename.e $0 n` $* 1>&2

if [ $# -lt 2 ]
then
	echo "Usage: `basename.e $0 n` source_database destination_database [zaptable ...]" 1>&2
	exit 1
fi

source_db=$1
destination_db=$2

results=`echo "show tables;" | sql.e $destination_db | wc -l`

if [ $results -ge 1 ]
then
	echo "Error in `basename.e $0 n`: $destination_db is populated" 1>&2
	exit 1
fi

mysqldump_stdout.sh $source_db | sql.e $destination_db

shift
shift

while [ $# -ge 1 ]
do
	table=$1
	echo "delete from $table;" | sql.e $destination_db
	shift
done

exit 0
