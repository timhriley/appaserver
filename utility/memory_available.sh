:

top -b -Em | head -5 | tail -1 | column.e 8

if [ "$1" = "loop" ]
then
	sleep 5
	exec $0 $1
fi

exit 0
