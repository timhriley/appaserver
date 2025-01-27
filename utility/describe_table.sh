:
# describe_table.sh
# -----------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 uidpwd table" 1>&2
	exit 1
fi

table_name=`toupper.e $2`

echo "select column_name			\
      from user_tab_columns 			\
      where table_name = '$table_name';" 	|
select.sh $1
