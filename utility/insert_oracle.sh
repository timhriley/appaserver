:
# ~riley/utility/insert_oracle.sh
# -------------------------------

if [ "$#" -lt 3 ]
then
	echo "Usage: $0 uid_pwd table field1,field2,... [debug]" 1>&2
	exit 1
fi

uid_pwd=`get_uid_pwd.e $1`

if [ "$uid_pwd" = "" ]
then
	exit 1
fi

table=$2
field_list=$3

if [ "$4" = "debug" ]
then
	insert.e $table $field_list		|
	sqlplus $uid_pwd
else
	insert.e $table	$field_list		|
	sqlplus $uid_pwd 			|
	grep_sqlplus_insert.sh
fi

exit $?
