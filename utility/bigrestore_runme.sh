:
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 table yyyymmdd" 1>&2
	exit 1
fi

table=$1
date=$2

create_sql="create.sql"
insert_sql="insert.sql"

bigrestore_gunzip.sh $table $date

if [ $? -ne 0 ]
then
	echo "$0: exiting early" 1>&2
	exit 1
fi

bigrestore_create_insert.sh $table $date $create_sql $insert_sql

if [ $? -ne 0 ]
then
	echo "$0: exiting early" 1>&2
	exit 1
fi

bigrestore_split.sh $insert_sql

if [ $? -ne 0 ]
then
	echo "$0: exiting early" 1>&2
	exit 1
fi

cat $create_sql | sql.e

if [ $? -ne 0 ]
then
	echo "$0: exiting early" 1>&2
	exit 1
fi

bigrestore_load.sh

if [ $? -ne 0 ]
then
	echo "$0: exiting early" 1>&2
	exit 1
fi

bigrestore_clean.sh $table $date $create_sql $insert_sql

exit $?
