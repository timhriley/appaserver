:

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 program" 1>&2
	exit 1
fi

program="$1"

where="program_name = '$program'"

echo "	select max( transaction_date_time )		\
	from transaction where $where;"			|
sql							|
column.e 0						|
cat

exit 0
