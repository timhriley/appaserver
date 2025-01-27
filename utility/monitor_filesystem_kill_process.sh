:
# utility/monitor_filesystem_kill_process.sh
# ------------------------------------------
# -- Tim Riley
# ------------------------------------------

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 filesystem maximum_percent_used process_id" 1>&2
	exit 1
fi


filesystem=$1
maximum_percent_used=$2
process_id=$3

while true
do
	percent_used=`df -v | grep "^${filesystem} "		|\
		      column.e 5 				|\
		      piece.e '%' 0`
	if [ "$percent_used" -ge "$maximum_percent_used" ]
	then
		echo "$0 killing process id = $process_id" 1>&2
		kill $process_id
		sleep 1
		kill -9 $process_id 2>/dev/null
		exit 0
	fi
	sleep 10
done
exit 0
