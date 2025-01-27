:
if [ "$HTTP_HOST" != "" ]
then
	echo $HTTP_HOST
else
	ifconfig			|
	grep 'inet '			|
	column.e 1			|
	grep -v '^127'			|
	cat
fi

exit 0
