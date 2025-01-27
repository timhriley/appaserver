:
# utility/dynamic_execute.sh
# -------------------------

if [ "$#" -eq 1 ]
then
	delimiter=$1
else
	delimiter='!'
fi

while read input_line
do

	results=`strncmp.e "$input_line" "$delimiter" 1`
	if [ "$results" -eq 0 ]
	then
		execute=`echo $input_line | piece.e "$delimiter" 1`
		$execute
	else
		echo "$input_line"
	fi
done

exit 0
