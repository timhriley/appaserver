:
# list_exists.sh
#
# Tim Riley
# --------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 search_item" 1>&2
	exit 1
fi

while read list
do
	for item in $list
	do
		if [ "$item" = $1 ]
		then
			exit 0
		fi
	done
done

# Return UNIX false
# -----------------
exit 1
