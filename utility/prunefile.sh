# prunefile.sh
# ------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -lt 2 ]
then
	echo "Usage: $0 lines_to_leave_at_bottom filename [filename...]" 1>&2
	exit 1
fi

lines_to_leave=$1

while [ "$#" -gt 1 ]
do
	filename=$2
	echo "Pruning $filename" 1>&2
	tmpfile=/tmp/prunefile.$$
	
	tail -$lines_to_leave $filename > $tmpfile
	cp $tmpfile $filename
	rm -f $tmpfile
	shift
done

exit 0

