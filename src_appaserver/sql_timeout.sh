:

if [ "$#" -eq 1 ]
then
	run_seconds=$1
fi

if [ "$run_seconds" -ge 1 ]
then
	shift
	timeout $run_seconds sql $*
else
	sql
fi

exit $?
