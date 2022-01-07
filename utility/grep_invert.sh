:
# $APPASERVER_HOME/utility/grep_invert.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 grep_invert_file" 1>&2
	exit 1
fi

grep_invert_file=$1

while read line
do
	results=`grep -i "^${line}$" $grep_invert_file`

	if [ "$results" = "" ]
	then
		echo $line
	fi
done

exit 0
