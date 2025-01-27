:
if [ `uname -n` = "snook" ]
then
	/usr/ucb/echo -n $*
else
	echo -n $*
fi
