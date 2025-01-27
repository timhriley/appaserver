:
# trim_to_one_page.sh
# -------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 filename" 1>&2
	exit 1
fi

filename=$1

number_lines=`cat $filename | wc -l`

if [ "$number_lines" -lt 59 ]
then
	cat $filename
else
	head -15 $filename
	echo "                                 ..."
	tail -43 $filename
fi

exit 0
