:
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 directory process" 1>&2
	exit 1
fi

directory=$1
process=$2

. /etc/profile

inotifywait -m -e modify --format %f $directory 2>/dev/null	|
while read newfile
do
	$process $newfile
done

exit 0
