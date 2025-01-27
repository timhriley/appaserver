:
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 new_size filename.ext" 1>&2
	exit 1
fi


new_size=$1
filename=$2

without_extension=`echo $filename | piece.e '.' 0`
extension=`echo $filename | piece.e '.' 1`
new_file=${without_extension}_resized_${new_size}.${extension}

convert -resize $new_size $filename $new_file

if [ "$?" -ne 0 ]
then
	echo "$0 exiting early." 1>&2
	exit 1
fi

echo "Created $new_file"

exit 0
