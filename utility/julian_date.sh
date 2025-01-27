:
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 yyyy_mm_dd" >&2
	exit 1
fi

date_string=$1

julian_time_list.e $date_string $date_string 1		|
piece.e '=' 0						|
sed 's/00*$//'

exit 0
