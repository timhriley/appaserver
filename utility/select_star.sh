:
# select_star.sh
#
# Tim Riley
# --------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 owner table_name" 1>&2
	exit 1
fi

owner=$1
table=$2

echo "select * from $table;" | select.sh $owner
