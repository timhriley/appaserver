:
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 insert_sql" 1>&2
	exit 1
fi

rows_per_file=2000000

insert_sql=$1

split -a5 --additional-suffix=.sql -l${rows_per_file} $insert_sql

exit $?
