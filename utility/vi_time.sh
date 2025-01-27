:
# vi_time.sh
# ----------
# Tim Riley
# ----------


if [ "$#" -lt 1 ]
then
	echo "Usage: $0 directory [-number]" 1>&2
	exit 1
fi

if [ "$#" -eq 2 ]
then
	head_amount=`expr $2 - 1`
else
	head_amount=-1
fi

file_to_vi="$1/`ls -t $1 | head $head_amount | tail -1`"
vi "$file_to_vi"
