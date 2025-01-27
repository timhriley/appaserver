:
# utility/date_subtract_days.sh
# -----------------------------

if [ "$#" != 2 ]
then
	echo "Usage: $0 date days_ago" 1>&2
	exit 1
fi

date_string=$1
days_ago=$2

echo "$date_string|filler"		|
date_add_days.e -${days_ago} 0 0 '|'	|
piece.e '|' 0

exit 0
