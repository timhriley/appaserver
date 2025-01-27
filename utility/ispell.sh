:
if [ "$#" -lt 1 ]
then
	echo "Usage: $0 word..." 1>&2
	exit 1
fi

while [ $# -gt 0 ]
do
	echo $1 | ispell -a | grep -v '@(#)' | grep -v '^*$' | grep -v '^$'
	shift
done

exit 0
