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

head -500 $input_file | grep -vi 'insert into' > $create_sql

if [ "$?" -eq 0 ]
then
	cat $input_file | grep -i 'insert into' > $insert_sql
fi

exit $?
