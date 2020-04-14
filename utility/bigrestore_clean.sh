:
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 create_sql insert_sql" 1>&2
	exit 1
fi

create_sql=$1
insert_sql=$2

rm $create_sql
rm $insert_sql
rm xa*.sql

exit 0
