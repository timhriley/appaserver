:
if [ "$#" -lt 2 ]
then
	echo "Usage: $0 table yyyymmdd [resume_sql]" 1>&2
	exit 1
fi

table=$1
date=$2

if [ "$#" -eq 3 ]
then
	resume_sql=$3
fi

create_sql="create.sql"
insert_sql="insert.sql"

if [ "$resume_sql" = "" ]
then
	bigrestore_grep_create.sh $table $date > $create_sql

	if [ $? -ne 0 ]
	then
		echo "$0: bigrestore_grep_create.sh exiting early" 1>&2
		exit 1
	fi
fi

if [ "$resume_sql" = "" ]
then
	bigrestore_grep_insert.sh $table $date > $insert_sql

	if [ $? -ne 0 ]
	then
		echo "$0: bigrestore_grep_insert.sh exiting early" 1>&2
		exit 1
	fi
fi

if [ "$resume_sql" = "" ]
then
	bigrestore_split.sh $insert_sql

	if [ $? -ne 0 ]
	then
		echo "$0: bigrestore_split.sh exiting early" 1>&2
		exit 1
	fi
fi

if [ "$resume_sql" = "" ]
then
	cat $create_sql | sql.e

	if [ $? -ne 0 ]
	then
		echo "$0: sql.e exiting early" 1>&2
		exit 1
	fi
fi

bigrestore_insert.sh "$resume_sql"

if [ $? -ne 0 ]
then
	echo "$0: bigrestore_insert.sh exiting early" 1>&2
	exit 1
fi

bigrestore_clean.sh $create_sql $insert_sql

exit $?
