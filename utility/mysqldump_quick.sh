:
# ---------------------------------------------------------------
# $APPASERVER_HOME/utility/mysqldump_quick.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$#" -lt 1 ]
then
	echo "Usage: `basename.e $0 n` database [tables]" 1>&2
	exit 1
fi

database=$1

export APPASERVER_DATABASE=$database

# destination_file=`pwd`/${database}_database.sql.gz
destination_file=/tmp/${database}_database.sql.gz

shift

input_file=`mysqldump.sh y $* | column.e 1`
mv $input_file $destination_file

results=$?

if [ $results = 0 ]
then
	echo "Created $destination_file"
fi

exit $results
