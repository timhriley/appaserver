:
# utility/grep_or.sh
# ------------------
#
# Tim Riley
# ------------------


if [ "`uname -n`" = "snook" ]
then
	grep_executable=/usr/xpg4/bin/grep
else
	grep_executable=grep
fi

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 \"bar_list\" filename [filename ...]" 1>&2
	exit 1
fi

bar_list=$1
shift
grep_arguments=

for item in `echo "$bar_list" | tr '|' '\012'`
do
	grep_arguments="$grep_arguments -e $item"
done

$grep_executable $grep_arguments $*
