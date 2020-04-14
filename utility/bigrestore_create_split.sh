:
if [ "$#" -ne 3 ]
then
	echo "Usage: $0 table yyyymmdd create_sql" 1>&2
	exit 1
fi

table=$1
date=$2
create_sql=$3

input_file="${table}_${date}.sql.gz"

zcat $input_file | head -500 | grep -vi 'insert into' > $create_sql

exit $?
