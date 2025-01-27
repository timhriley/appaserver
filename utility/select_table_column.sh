:
# select_table_column.sh
# ----------------------

if [ "$#" -lt 3 ]
then
	echo $0 uid_pwd table column [debug]
	exit 1
fi

uid_pwd=`get_uid_pwd.e $1`
table=$2
column=$3

if [ "$4" = "debug" ]
then
	set -x
fi

echo "select $column from $table order by $column;"	|
select.sh $uid_pwd
