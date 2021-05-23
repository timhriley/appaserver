:
# $APPASERVER_HOME/utility/mysqldump_quick.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 database" 1>&2
	exit 1
fi

database=$1

destination_file=`pwd`/${database}_database.sql.gz

grep '^password' /etc/appaserver.config

input_file=`mysqldump.sh ${database} y | column.e 1`
mv $input_file $destination_file

results=$?

if [ $results = 0 ]
then
	echo "Created $destination_file"
fi

exit $results
