:
if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must first . set_project" 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

if [ "$#" != 4 ]
then
	echo "Usage: $0 ignored begin_date end_date fund" 1>&2
	exit 1
fi

begin_date=$2
end_date=$3
fund=$4

time_ticker.e ^ daily $begin_date $end_date "" 2>/dev/null	|
piece.e '^' 0							|
while read as_of_date
do
	input_record=`trial_balance	$application		\
					session			\
					login_name		\
					role			\
					trial_balance		\
					"$fund"			\
					$as_of_date		\
					aggregation		\
					stdout			\
					subclassification_option|
			tail -1`

	debit=`echo $input_record | piece.e '^' 4`
	credit=`echo $input_record | piece.e '^' 5`

	if [ "$debit" != "$credit" ]
	then
		echo "${as_of_date}^$input_record"
		break
	fi
done

exit 0
