:

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 ip_address" 1>&2
	exit 1
fi

results=`nslookup $1 | grep 'answer:$'`

if [ "$results" != "" ]
then
	echo "valid"
else
	echo "invalid"
fi

exit 0
