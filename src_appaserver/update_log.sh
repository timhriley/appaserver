:
if [ $# -ne 1 ]
then
	echo "Usage: $0 log_file" 1>&2
fi

log_file=$1

sed "s/\(set password = \)\('.*'\)\(.*\)/\1'secret'\3/" |
cat >> $log_file

exit 0
