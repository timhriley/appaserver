:
# find_each_pattern.sh
#
# Tim Riley
# --------------

while read find_pattern
do
	if [ "$#" -eq 1 -a "$1" = "prune" ]
	then
		ls -1 $find_pattern
	else
		find . -name "$find_pattern" -print
	fi
done

exit 0
