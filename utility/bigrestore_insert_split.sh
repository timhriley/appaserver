:
if [ "$#" -ne 3 ]
then
	echo "Usage: $0 table yyyymmdd insert_sql" 1>&2
	exit 1
fi

table=$1
date=$2
insert_sql=$3

input_file="${table}_${date}.sql.gz"

zcat $input_file | grep -i 'insert into' > $insert_sql

exit $?
