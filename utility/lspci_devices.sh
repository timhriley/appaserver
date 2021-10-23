:

if [ "$#" -eq 1 ]
then
	controller="$1"
else
	controller=""
fi

if [ "$controller" != "" ]
then
	lspci					|
	while read record
	do
		c=`echo $record | piece.e ':' 1 | sed 's/[0-z][0-z]\.[0-z] //g'`
		if [ "$c" = "$controller" ]
		then
			slot=`echo $record | column.e 0`
			lspci -s $slot -x -vv
			echo "-------------------------"
		fi
	done
else
	lspci					|
	column.e 0				|
	while read slot
	do
		lspci -s $slot -x -vv
		echo "-------------------------"
	done
fi

exit 0
