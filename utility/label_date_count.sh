:
# utility/label_date_count.sh
# ---------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 starting_yyyy_mm_dd count" 1>&2
	exit 1
fi

starting_date=$1
count=$2

date_count.sh $starting_date $count				|
xargs.e "single_label_direct.sh '' '' {} '' '' '' '' 1"

exit 0
