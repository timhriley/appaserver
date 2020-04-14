:
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 table yyyymmdd" 1>&2
	exit 1
fi

table=$1
date=$2

input_file="${table}_${date}.sql.gz"

if [ -f $input_file ]
then
	gunzip -k $input_file
fi

exit $?
