:
if [ "$#" -ne 3 ]
then
	echo "Usage: $0 from_yyyy_mm_dd to_yyyy_mm_dd ancestor"
	exit 1
fi

from_date=$1
to_date=$2
ancestor=$3

for date in `date_count.sh $from_date 9999`
do
	if [ $date = "$to_date" ]
	then
		break
	fi

	results=`date_grandfather_father_son.sh $date $ancestor`

	# If not the ancestor, then purge.
	# --------------------------------
	if [ "$results" -eq 0 ]
	then
		date_without_dashes=`echo $date | sed 's/-//g'`

		filename="*${date_without_dashes}*"
		rm -f $filename
	fi

done

exit 0
