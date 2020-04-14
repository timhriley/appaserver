:
if [ "$#" -ne 4 ]
then
	echo "Usage: $0 table yyyymmdd create_sql insert_sql" 1>&2
	exit 1
fi

table=$1
date=$2
create_sql=$3
insert_sql=$4

input_file="${table}_${date}.sql"

rm $input_file
rm $create_sql
rm $insert_sql
rm xa*.sql

exit 0
